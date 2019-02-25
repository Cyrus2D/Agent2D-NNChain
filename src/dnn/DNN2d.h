//
// Created by aref on 2/13/19.
//

#ifndef NN1_DNN2D_H
#define NN1_DNN2D_H

#include "../chain_action/predict_state.h"
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
    if (dx == 0)
        dx = 0.00000001;
    teta = atan(dy / dx);
    if (dx < 0)
        teta += M_PI;
    if (teta < 0)
        teta = M_PI * 2 + teta;

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

    MatrixXd make_input(const PredictState &state) {
        vector<double> x;
        const rcsc::AbstractPlayerObject *kicker = find_kicker(state);
        cout << "Kicker Found" << endl;

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
            if (i == state.self().unum()) {
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

        cout << "OPP" << endl;

        // OPP
        for (int i = 1; i <= 11; i++) {
            const rcsc::AbstractPlayerObject *tm = state.theirPlayer(i);
            if (i == state.self().unum()) {
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
    const rcsc::AbstractPlayerObject *find_kicker(const PredictState &state) {
        double min_dist;
        const rcsc::AbstractPlayerObject *kicker;

        for (int i = 1; i <= 11; i++) {
            const rcsc::AbstractPlayerObject *tm = state.ourPlayer(i);
            double dist = tm->pos().dist(state.ball().pos());
            if (dist < min_dist) {
                kicker = tm;
                min_dist = dist;
            }
        }

        for (int i = 1; i <= 11; i++) {
            const rcsc::AbstractPlayerObject *opp = state.theirPlayer(i);
            double dist = opp->pos().dist(state.ball().pos());
            if (dist < min_dist) {
                kicker = opp;
                min_dist = dist;
            }
        }

        return kicker;
    }

    MatrixXd vector_ro_matrix(vector<double> &x) {
        MatrixXd inp(x.size(), 1);
        for (int i = 0; i < x.size(); i++)
            inp(i, 0) = x[i];
        return inp;
    }

};


#endif //NN1_DNN2D_H
