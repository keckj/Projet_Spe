#include "headers.hpp"
#include "model.hpp"
#include "model.moc"

void Model::startComputing() {
    initComputation();
    for (int i = 0; i < m_nbIter; i++) {
        m_mutex.lock();
        if (m_stop) {
            emit finished();
            m_mutex.unlock();
            return;
        }
        while (m_pause) {
            m_cond.wait(&m_mutex);
        }
        m_mutex.unlock();
        computeStep(i);
        //TODO computeStep returns pointer to texture/grid
        //emit stepComputed(ptr)
    }
    finishComputation();
    emit finished();
}

void Model::pauseComputing(bool b) {
    m_mutex.lock();
    m_pause = b;
    m_mutex.unlock();
}

void Model::stopComputing() {
    m_mutex.lock();
    m_stop = true;
    m_mutex.unlock();
}
