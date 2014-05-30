#include "utils/headers.hpp"
#include "exampleModel.hpp"
#include "exampleModel.moc"
#include "grid2D.hpp"

ExampleModel::ExampleModel(int nbIter) : Model(nbIter) {
}

ExampleModel::~ExampleModel() {
}

void ExampleModel::startComputing() {
    Grid2D<float> grid(1.0,1.0,100u,100u,true);
    for (int i = 0; i < m_nbIter; i++) {
        std::cout << "step=" << i << std::endl;
        emit stepComputed(&grid);
        QCoreApplication::processEvents(); // check for pending events
    }
    std::cout << "finish" << std::endl;
    emit finished();
}

