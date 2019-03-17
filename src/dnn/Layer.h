#ifndef LAYER
#define LAYER

#include "Function.h"
#include <eigen3/Eigen/Dense>
using Eigen::MatrixXd;
class Layer{
public:
    MatrixXd mInput;
    MatrixXd mOutput;
    MatrixXd mWeight;
    MatrixXd mBios;

    Layer() = delete;
    Layer(const MatrixXd weight, const MatrixXd bios);
    Layer(const MatrixXd weight, const MatrixXd bios, Function function);
    void Calculate();
    void Calculate(MatrixXd input);
    std::function<void(MatrixXd &)> mFunction;
};

#endif
