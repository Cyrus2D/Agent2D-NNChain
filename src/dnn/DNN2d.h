//
// Created by aref on 2/13/19.
//

#ifndef NN1_DNN2D_H
#define NN1_DNN2D_H

#include "../chain_action/predict_state.h"
#include <rcsc/player/player_agent.h>
#include "DeepNueralNetwork.h"
#include <cmath>


using namespace std;

class Polar {
public:
    double teta;
    double r;

    Polar() {
        teta = 0;
        r = 0;
    }

    Polar(double _teta, double _r) {
        teta = _teta;
        r = _r;
    }

};

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

class DNN2d : public DeepNueralNetwork {
public:
    bool readed;
    string file;

    DNN2d(string _file) : file(_file) {
        readed = false;
        cout << "Updating" << endl;
        updata_weights();
        cout << "Updating Done" << endl;
    }

    bool updata_weights() {
        if (readed)
            return true;
        readed = true;
        return ReadFromTensorFlow(file);
    }

    MatrixXd make_input(const rcsc::WorldModel &state) {
        vector<double> x;
//        const rcsc::AbstractPlayerObject *kicker = find_kicker(state);
//        cout << "Kicker Found: " << kicker->unum() << endl;

        // Ball
        Polar ball_p = make_polar(state.self().pos(), state.ball().pos());
        Polar ball_vel = make_polar(rcsc::Vector2D(0, 0), state.ball().vel());
        x.push_back(state.ball().pos().x);
        x.push_back(state.ball().pos().y);
        x.push_back(ball_p.r);
        x.push_back(ball_p.teta);
        x.push_back(ball_vel.r);
        x.push_back(ball_vel.teta);

        // TM
        int kicker_unum = find_kicker(state);
        cout << "Polars(kicker): " << kicker_unum << endl;
        for (int i = 1; i <= 11; i++) {
            const rcsc::AbstractPlayerObject *tm = state.ourPlayer(i);
            cout << "i: " << i << endl;
            if (i == kicker_unum) {
                x.push_back(tm->pos().x);
                x.push_back(tm->pos().y);
            } else {
                Polar player_p = make_polar(state.self().pos(), tm->pos());
                x.push_back(player_p.r);
                x.push_back(player_p.teta);
                cout << i << "p: " << player_p.r << " " << player_p.teta << endl;
            }
            Polar playerToGoal = make_polar(tm->pos(), rcsc::Vector2D(52.5, 0));
            x.push_back(playerToGoal.r);
            x.push_back(playerToGoal.teta);
            cout << i << "g: " << playerToGoal.r << " " << playerToGoal.teta << endl;
        }

        cout << "OPP" << endl;

        // OPP
        for (int i = 1; i <= 11; i++) {
            const rcsc::AbstractPlayerObject *tm = state.theirPlayer(i);
            Polar player_p = make_polar(state.self().pos(), tm->pos());
            x.push_back(player_p.r);
            x.push_back(player_p.teta);
            Polar playerToGoal = make_polar(tm->pos(), rcsc::Vector2D(52.5, 0));
            x.push_back(playerToGoal.r);
            x.push_back(playerToGoal.teta);
        }
        cout << "x size: " << x.size();
        return vector_ro_matrix(x);
    }

    void display() {
        cout << "mInput: " << mInput.cols() << ", " << mInput.rows() << endl;
        for (int i = 0; i < mLayers.size(); i++) {
            cout << "layer: " << i << endl;
            cout << "W: " << mLayers[i].mWeight.cols() << ", " << mLayers[i].mWeight.rows() << endl;
            cout << "B: " << mLayers[i].mBios.cols() << ", " << mLayers[i].mBios.rows() << endl;
        }
    }

    int max_output() {
        double max = -10000000;
        double index = -1;
        for (int i = 0; i < mOutput.rows(); i++) {
            if (mOutput(i, 0) > max) {
                max = mOutput(i, 0);
                index = i;
            }
        }
        cout << "Max: " << max << " unum: " << index + 1 << endl;
        return index;
    }

private:
    const int find_kicker(const rcsc::WorldModel &state) {
//        const rcsc::AbstractPlayerObject* opp = state.getOpponentNearestToBall(10);
        const rcsc::AbstractPlayerObject *tm = state.getTeammateNearestToBall(10);

        return tm->unum();
    }

    MatrixXd vector_ro_matrix(vector<double> &x) {
        MatrixXd inp(x.size(), 1);
        for (int i = 0; i < x.size(); i++)
            inp(i, 0) = x[i];
        return inp;
    }

};


#endif //NN1_DNN2D_H
