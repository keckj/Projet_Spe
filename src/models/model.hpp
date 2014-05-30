#ifndef MODEL_HPP_
#define MODEL_HPP_

#include "utils/headers.hpp"
#include "grid2D.hpp"
#include "grid3D.hpp"

class Model : public QObject {
    Q_OBJECT

    public:
        explicit Model(int nbIter) : m_nbIter(nbIter) {}

    public slots:
        virtual void startComputing() = 0;

    /* Copy signals, inheritance does not work properly */
    signals:
        void stepComputed(const Grid2D<float> *grid);
        void stepComputed(const Grid3D<float> *grid);
        void finished();

    protected:
        int m_nbIter;
};

#endif
