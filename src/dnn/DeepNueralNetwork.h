#ifndef DEEPNN
#define DEEPNN

#include "Layer.h"
#include <vector>
#include <fstream>
#include <sstream>
#include <string>

using Eigen::MatrixXd;
class DeepNueralNetwork {
public:
    MatrixXd mInput;
    MatrixXd mOutput;
    std::vector<Layer> mLayers;

    void AddLayer(const Layer layer);

    void Calculate();

    void Calculate(MatrixXd input);

    bool ReadFromKeras(std::string file);

    bool ReadFromTensorFlow(std::string file);
};

#endif
