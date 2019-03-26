#ifndef FUNCTION_H
#define FUNCTION_H

#include <eigen3/Eigen/Dense>
#include <iostream>
using Eigen::MatrixXd;
using namespace std;

enum class Function
{
    Linear, Sigmoid, ReLu, SoftMax
};
Function StringToFunction(std::string function);
//static void LinearFunction(MatrixXd & output);
static void LinearFunction(MatrixXd & output){
    for(int i = 0; i < output.rows(); i++){
        for(int j = 0; j < output.cols(); j++){
            output(i, j) = output(i, j);
        }
    }
}

static void ReLuFunction(MatrixXd & output){
    for(int i = 0; i < output.rows(); i++){
        for(int j = 0; j < output.cols(); j++){
            if( output(i, j) < 0)
                output(i, j) = 0;
            else
                output(i, j) = output(i, j);
        }
    }
}
static void SoftMaxFunction(MatrixXd & output){
    double sum = 0;
    for(int i = 0; i < output.rows(); i++){
        for(int j = 0; j < output.cols(); j++){
            sum += exp(output(i, j));
        }
    }
    for(int i = 0; i < output.rows(); i++){
        for(int j = 0; j < output.cols(); j++){
            output(i, j) = exp(output(i, j)) / sum;
        }
    }
}
static void SigmoidFunction(MatrixXd & output){
    for(int i = 0; i < output.rows(); i++){
        for(int j = 0; j < output.cols(); j++){
            output(i, j) = 1.0 / (1.0 + exp(-output(i, j)));
        }
    }
}

//static void ReLuFunction(MatrixXd & output);
//static void SoftMaxFunction(MatrixXd & output);
//static void SigmoidFunction(MatrixXd & output);
#endif
