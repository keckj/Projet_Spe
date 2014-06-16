#ifndef EXAMPLE_MODEL_HPP
#define EXAMPLE_MODEL_HPP

#include "utils/headers.hpp"
#include "model.hpp"
#include "grid2D.hpp"

class ExampleModel : public Model {
    Q_OBJECT

    public:
        ExampleModel(int nbIter, 
				std::map<QString, bool> *renderedVars,
				std::map<QString, int> *initialCondIds);
        ~ExampleModel();

        void initComputation() override;
        void computeStep(int i) override;
        void finishComputation() override;
    
    signals:
        void stepComputed(const Grid2D<float> *grid);
        void stepComputed(const Grid3D<float> *grid);
};

#endif
