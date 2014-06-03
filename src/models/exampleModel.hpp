#ifndef EXAMPLE_MODEL_HPP
#define EXAMPLE_MODEL_HPP

#include "utils/headers.hpp"
#include "model.hpp"
#include "grid2D.hpp"

class ExampleModel : public Model {
    Q_OBJECT

    public:
        ExampleModel(int nbIter);
        ~ExampleModel();

        void initComputation();
        void computeStep(int i);
        void finishComputation();
    
    signals:
        void stepComputed(const Grid2D<float> *grid);
        void stepComputed(const Grid3D<float> *grid);
};

#endif
