
#ifndef COMPUTETHREAD_HPP
#define COMPUTETHREAD_HPP

#include "utils/headers.hpp"
#include "grid2D.hpp"

class ComputeThread : public QThread {
    Q_OBJECT

    public:
        ComputeThread(QObject *parent_ = 0);
        ~ComputeThread();

        void compute(); // todo, starts the compute thread

    signals:
        void stepComputed(const Grid2D<float> &grid);

    public slots:
        void stop();

    protected:
        void run();

    private:
        // Stuff to make the thread sleep if it has nothing to do 
        QMutex m_mutex; // dedicace Mounier
        QWaitCondition m_cond;
        bool m_abort;

};

#endif
