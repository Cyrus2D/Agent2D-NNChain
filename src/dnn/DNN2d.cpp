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

void DNN2d::display() {
    return;
    cout << "mInput: " << mInput.cols() << ", " << mInput.rows() << endl;
    for (int i = 0; i < mLayers.size(); i++) {
        cout << "layer: " << i << endl;
        cout << "W: " << mLayers[i].mWeight.cols() << ", " << mLayers[i].mWeight.rows() << endl;
        cout << "B: " << mLayers[i].mBios.cols() << ", " << mLayers[i].mBios.rows() << endl;
    }
}

std::pair<int, double> DNN2d::max_output() {
    double max = -10000000;
    double index = -1;
    for (int i = 0; i < mOutput.rows(); i++) {
        if (mOutput(i, 0) > max) {
            max = mOutput(i, 0);
            index = i;
        }
    }
    return std::make_pair(index, max);
}

const int DNN2d::find_kicker(const rcsc::WorldModel &state) {
//        const rcsc::AbstractPlayerObject* opp = state.getOpponentNearestToBall(10);
    const rcsc::AbstractPlayerObject *tm = state.getTeammateNearestToBall(10);

    return tm->unum();
}




void wm2vector(const rcsc::WorldModel &wm, vector<rcsc::Vector2D> &tm_pos, vector<rcsc::Vector2D> &opp_pos, rcsc::Vector2D &ball_pos, rcsc::Vector2D &ball_vel)
{
    for (int i = 1; i <= 11; i++){
        const rcsc::AbstractPlayerObject * tm = wm.ourPlayer(i);
        if(tm != nullptr && tm->unum() > 0){
            tm_pos.push_back(tm->pos());
        }else{
            tm_pos.push_back(rcsc::Vector2D(-100,0));
        }
    }
    for (int i = 1; i <= 11; i++){
        const rcsc::AbstractPlayerObject * tm = wm.theirPlayer(i);
        if(tm != nullptr && tm->unum() > 0){
            opp_pos.push_back(tm->pos());
        }else{
            opp_pos.push_back(rcsc::Vector2D(-100,0));
        }
    }
    ball_pos = wm.ball().pos();
    ball_vel = wm.ball().vel();
}

vector<double> vector2feature(const rcsc::WorldModel& wm, vector<rcsc::Vector2D> &tm_pos, vector<rcsc::Vector2D> &opp_pos, rcsc::Vector2D &ball_pos, rcsc::Vector2D &ball_vel_)
{

    double min_dist = 1000;
    int min_dist_i = 0;
    for (int i = 1; i <= 11; i++){
        const rcsc::AbstractPlayerObject *kicker = wm.ourPlayer(i);
        if(kicker != nullptr && kicker->unum() > 0){
            double dist = kicker->pos().dist(ball_pos);
            if(dist < min_dist){
                min_dist = dist;
                min_dist_i = i;
            }
        }
    }

    int kicker_unum = 0;
    rcsc::Vector2D kickerpos = wm.ball().pos();

    if(min_dist_i > 0){
        const rcsc::AbstractPlayerObject *kicker = wm.ourPlayer(min_dist_i);
        kicker_unum = min_dist_i;
        kickerpos = kicker->pos();
    }

    vector<double> res;
    Polar ball_p = make_polar(kickerpos, ball_pos);
    Polar ball_vel = make_polar(rcsc::Vector2D(0, 0), ball_vel_);
    res.push_back(ball_pos.x);
    res.push_back(ball_pos.y);
    res.push_back(ball_p.r);
    res.push_back(ball_p.teta);
    res.push_back(ball_vel.r);
    res.push_back(ball_vel.teta);
    // TM
    for (int i = 1; i <= 11; i++) {
        if (i == kicker_unum) {
            res.push_back(tm_pos[i-1].x);
            res.push_back(tm_pos[i-1].y);
        } else {
            Polar player_p = make_polar(kickerpos, tm_pos[i-1]);
            res.push_back(player_p.r);
            res.push_back(player_p.teta);
        }
        Polar playerToGoal = make_polar(tm_pos[i-1], rcsc::Vector2D(52.5, 0));
        res.push_back(playerToGoal.r);
        res.push_back(playerToGoal.teta);
    }


    // OPP
    for (int i = 1; i <= 11; i++) {
        Polar player_p = make_polar(kickerpos, opp_pos[i-1]);
        res.push_back(player_p.r);
        res.push_back(player_p.teta);
        Polar playerToGoal = make_polar(opp_pos[i-1], rcsc::Vector2D(52.5, 0));
        res.push_back(playerToGoal.r);
        res.push_back(playerToGoal.teta);
    }

    return res;
}

Eigen::MatrixXd vector_to_matrix(vector<double> &x) {
    MatrixXd inp(x.size(), 1);
    for (int i = 0; i < x.size(); i++)
        inp(i, 0) = x[i];
    return inp;
}
