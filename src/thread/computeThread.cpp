
#include "utils/headers.hpp"
#include "computeThread.hpp"
#include "computeThread.moc"
#include "grid2D.hpp"

ComputeThread::ComputeThread(QObject *parent_) : 
    QThread(parent_) {

    m_abort = false;
}

ComputeThread::~ComputeThread() {
    m_mutex.lock();
    m_abort = true;
    m_cond.wakeOne(); // wake up the thread if it is sleeping
    m_mutex.unlock();
    wait(); // wait till run has exited
}

void ComputeThread::compute() {
    //TODO init data if needed
    m_abort = false;
    if (!isRunning())
        start(); // starts the thread and calls run
}

void ComputeThread::run() {
    //OPENCL
    
    //Loop {
        //gpu
        //emit stepComputed(grid);
        if (m_abort)
            return;
    //}
}

void ComputeThread::stop() {
    m_mutex.lock();
    m_abort = true;
    m_mutex.unlock();
}


