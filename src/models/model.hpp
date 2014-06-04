#ifndef MODEL_HPP_
#define MODEL_HPP_

#include "utils/headers.hpp"
#include "grid2D.hpp"
#include "grid3D.hpp"

class Model : public QObject {
    Q_OBJECT

    public:
        Model(int nbIter) : m_nbIter(nbIter), m_pause(false), m_stop(false) {}
		virtual ~Model() {};

    protected:
        virtual void initComputation() = 0;
        virtual void computeStep(int i) = 0;
        virtual void finishComputation() = 0;

    public slots:
        void startComputing();
        void pauseComputing(bool b);
        void stopComputing();

    /* Copy signals, inheritance does not work properly */
    signals:
        void stepComputed(const Grid2D<float> *grid);
        void stepComputed(const Grid3D<float> *grid);
        void finished();

    private:
        int m_nbIter;
        QMutex m_mutex;
        QWaitCondition m_cond;
        bool m_pause, m_stop;
};

#endif
