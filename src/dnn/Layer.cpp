#include "Layer.h"
#include "Function.h"
using Eigen::MatrixXd;
Layer::Layer(const MatrixXd weight, const MatrixXd bios){
    mWeight = weight;
    mBios = bios;
    mFunction = LinearFunction;
}
Layer::Layer(const MatrixXd weight, const MatrixXd bios, Function function){
    mWeight = weight;
    mBios = bios;
    switch (function) {
    case Function::Linear:
        mFunction = LinearFunction;
        break;
    case Function::ReLu:
        mFunction = ReLuFunction;
        break;
    case Function::SoftMax:
//        mFunction = SoftMaxFunction;
        break;
    case Function::Sigmoid:
//        mFunction = SigmoidFunction;
        break;
    }

}
void Layer::Calculate(){
    mOutput = mWeight * mInput;
    mOutput += mBios;
    mFunction(mOutput);
}
void Layer::Calculate(MatrixXd input){
    mInput = input;
    Calculate();
}
