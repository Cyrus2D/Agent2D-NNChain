//
// Created by aref on 2/13/19.
//

#ifndef NN1_DNN2D_H
#define NN1_DNN2D_H

#include "../chain_action/predict_state.h"
#include <rcsc/player/player_agent.h>
#include "DeepNueralNetwork.h"
#include <cmath>

using Eigen::MatrixXd;
using namespace std;

class Polar {
public:
    double teta;
    double r;

    Polar();

    Polar(double _teta, double _r);

};

Polar make_polar(rcsc::Vector2D start, rcsc::Vector2D endp);
class DNN2d : public DeepNueralNetwork {
public:
    bool readed;
    string file;

    static DNN2d *ins;
    static DNN2d *i(string _file_name =  "");

    DNN2d(string _file);

    bool updata_weights() ;

    MatrixXd make_input(const rcsc::WorldModel &state);

    void display() ;

    int max_output();

private:
    const int find_kicker(const rcsc::WorldModel &state) ;

    MatrixXd vector_ro_matrix(vector<double> &x) ;

};


#endif //NN1_DNN2D_H
