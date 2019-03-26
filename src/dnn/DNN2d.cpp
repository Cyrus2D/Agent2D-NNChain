#include "DNN2d.h"
#include <cmath>

using Eigen::MatrixXd;
using namespace std;

Polar::Polar() {
    teta = 0;
    r = 0;
}

Polar::Polar(double _teta, double _r) {
    teta = _teta;
    r = _r;
}


Polar make_polar(rcsc::Vector2D start, rcsc::Vector2D endp) {
    double r, teta;

    // TETA
    double dy = endp.y - start.y;
    double dx = endp.x - start.x;
    if (dx == 0 && dy == 0)
        teta = 0;
    else
        teta = atan2(dy, dx);
//    if (dx == 0)
//        dx = 0.00000001;
//    teta = atan(dy / dx);
//    if (dx < 0)
//        teta += M_PI;
//    if (teta < 0)
//        teta = M_PI * 2 + teta;

    // R
    r = sqrt(pow(endp.x - start.x, 2) + pow(endp.y - start.y, 2));
    return Polar(teta, r);
}

DNN2d *DNN2d::i(string _file_name) {
    if (ins == NULL)
        ins = new DNN2d(_file_name);
    return ins;
}

DNN2d::DNN2d(string _file) : file(_file) {
    readed = false;
    updata_weights();
}

bool DNN2d::updata_weights() {
    if (readed)
        return true;
    readed = true;
    return ReadFromTensorFlow(file);
}

MatrixXd DNN2d::make_input(const rcsc::WorldModel &state) {
    vector<double> x;
    int kicker_unum = find_kicker(state);
    const rcsc::AbstractPlayerObject *kicker = state.ourPlayer(kicker_unum);

    // Ball
    Polar ball_p = make_polar(kicker->pos(), state.ball().pos());
    Polar ball_vel = make_polar(rcsc::Vector2D(0, 0), state.ball().vel());
    x.push_back(state.ball().pos().x);
    x.push_back(state.ball().pos().y);
    x.push_back(ball_p.r);
    x.push_back(ball_p.teta);
    x.push_back(ball_vel.r);
    x.push_back(ball_vel.teta);

    // TM
    for (int i = 1; i <= 11; i++) {
        const rcsc::AbstractPlayerObject *tm = state.ourPlayer(i);
        if (i == kicker_unum) {
            x.push_back(tm->pos().x);
            x.push_back(tm->pos().y);
        } else {
            Polar player_p = make_polar(kicker->pos(), tm->pos());
            x.push_back(player_p.r);
            x.push_back(player_p.teta);
        }
        Polar playerToGoal = make_polar(tm->pos(), rcsc::Vector2D(52.5, 0));
        x.push_back(playerToGoal.r);
        x.push_back(playerToGoal.teta);
    }


    // OPP
    for (int i = 1; i <= 11; i++) {
        const rcsc::AbstractPlayerObject *tm = state.theirPlayer(i);
        Polar player_p = make_polar(kicker->pos(), tm->pos());
        x.push_back(player_p.r);
        x.push_back(player_p.teta);
        Polar playerToGoal = make_polar(tm->pos(), rcsc::Vector2D(52.5, 0));
        x.push_back(playerToGoal.r);
        x.push_back(playerToGoal.teta);
    }
    return vector_ro_matrix(x);
}

MatrixXd DNN2d::make_input(int kicker_unum,
                           rcsc::Vector2D *tmpos,
                           rcsc::Vector2D *opppos,
                           rcsc::Vector2D ballpos,
                           rcsc::Vector2D ballvel) {
    vector<double> x;
    const rcsc::Vector2D kickerpos = tmpos[kicker_unum - 1];

    // Ball
    Polar ball_p = make_polar(kickerpos, ballpos);
    Polar ball_vel = make_polar(rcsc::Vector2D(0, 0), ballvel);
    x.push_back(ballpos.x);
    x.push_back(ballpos.y);
    x.push_back(ball_p.r);
    x.push_back(ball_p.teta);
    x.push_back(ball_vel.r);
    x.push_back(ball_vel.teta);

    // TM
    for (int i = 1; i <= 11; i++) {
        if (i == kicker_unum) {
            x.push_back(tmpos[i - 1].x);
            x.push_back(tmpos[i - 1].y);
        } else {
            Polar player_p = make_polar(kickerpos, tmpos[i - 1]);
            x.push_back(player_p.r);
            x.push_back(player_p.teta);
        }
        Polar playerToGoal = make_polar(tmpos[i - 1], rcsc::Vector2D(52.5, 0));
        x.push_back(playerToGoal.r);
        x.push_back(playerToGoal.teta);
    }


    // OPP
    for (int i = 1; i <= 11; i++) {
        Polar player_p = make_polar(kickerpos, opppos[i - 1]);
        x.push_back(player_p.r);
        x.push_back(player_p.teta);
        Polar playerToGoal = make_polar(opppos[i - 1], rcsc::Vector2D(52.5, 0));
        x.push_back(playerToGoal.r);
        x.push_back(playerToGoal.teta);
    }
    return vector_ro_matrix(x);

}

void DNN2d::display() {
    return;
    cout << "mInput: " << mInput.cols() << ", " << mInput.rows() << endl;
    for (int i = 0; i < mLayers.size(); i++) {
        cout << "layer: " << i << endl;
        cout << "W: " << mLayers[i].mWeight.cols() << ", " << mLayers[i].mWeight.rows() << endl;
        cout << "B: " << mLayers[i].mBios.cols() << ", " << mLayers[i].mBios.rows() << endl;
    }
}

int DNN2d::max_output() {
    double max = -10000000;
    double index = -1;
    for (int i = 0; i < mOutput.rows(); i++) {
        if (mOutput(i, 0) > max) {
            max = mOutput(i, 0);
            index = i;
        }
    }
    return index;
}

const int DNN2d::find_kicker(const rcsc::WorldModel &state) {
//        const rcsc::AbstractPlayerObject* opp = state.getOpponentNearestToBall(10);
    const rcsc::AbstractPlayerObject *tm = state.getTeammateNearestToBall(10);

    return tm->unum();
}

MatrixXd DNN2d::vector_ro_matrix(vector<double> &x) {
    MatrixXd inp(x.size(), 1);
    for (int i = 0; i < x.size(); i++)
        inp(i, 0) = x[i];
    return inp;
}

