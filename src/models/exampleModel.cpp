#include "utils/headers.hpp"
#include "exampleModel.hpp"
#include "exampleModel.moc"
#include "grid2D.hpp"

ExampleModel::ExampleModel(int nbIter) : Model(nbIter) {
}

ExampleModel::~ExampleModel() {
}

void ExampleModel::initComputation() {}

void ExampleModel::computeStep(int i) {
    Grid2D<float> *grid = new Grid2D<float>(1.0,1.0,100u,100u,true);
    emit stepComputed(grid);
}

void ExampleModel::finishComputation() {}

