#include "Function.h"
#include <iostream>
#include <cmath>
using namespace std;
using Eigen::MatrixXd;
Function StringToFunction(std::string function)
{
    if (function.compare("linear") == 0)
    {
        return Function::Linear;
    }
    if (function.compare("relu") == 0)
    {
        return Function::ReLu;
    }
    if (function.compare("sigmoid") == 0)
    {
        return Function::Sigmoid;
    }
    if (function.compare("softmax") == 0)
    {
        return Function::SoftMax;
    }
}
//void LinearFunction(MatrixXd & output){
//    for(int i = 0; i < output.rows(); i++){
//        for(int j = 0; j < output.cols(); j++){
//            output(i, j) = output(i, j);
//        }
//    }
//}
//void ReLuFunction(MatrixXd & output){
//    for(int i = 0; i < output.rows(); i++){
//        for(int j = 0; j < output.cols(); j++){
//            if( output(i, j) < 0)
//                output(i, j) = 0;
//            else
//                output(i, j) = output(i, j);
//        }
//    }
//}
//void SoftMaxFunction(MatrixXd & output){
//    double sum = 0;
//    for(int i = 0; i < output.rows(); i++){
//        for(int j = 0; j < output.cols(); j++){
//            sum += exp(output(i, j));
//        }
//    }
//    for(int i = 0; i < output.rows(); i++){
//        for(int j = 0; j < output.cols(); j++){
//            output(i, j) = exp(output(i, j)) / sum;
//        }
//    }
//}
//void SigmoidFunction(MatrixXd & output){
//    for(int i = 0; i < output.rows(); i++){
//        for(int j = 0; j < output.cols(); j++){
//            output(i, j) = 1.0 / (1.0 + exp(-output(i, j)));
//        }
//    }
//}
