#include "headers.hpp"
#include "model.hpp"
#include "model.moc"
#include "log.hpp"


Model::Model(int nbIter, unsigned int width, unsigned int height, unsigned int length) : 
            m_width(width), m_height(height), m_length(length), m_nbIter(nbIter), m_pause(false), m_stop(false) {
}

void Model::startComputing() {
		
	log_console->debugStream() << "Start Computing.";

    initComputation();

    for (int i = 0; i < m_nbIter; i++) {
        m_mutex.lock();
        while (m_pause) {
            m_cond.wait(&m_mutex);
        }
        bool stop = m_stop;
        m_mutex.unlock();
        
        if (stop) {
            break;
        }

        computeStep(i);
        //emit(computeStep(i)); TODO TODO TODO map<string variable, GLuint texture> *
    }
    finishComputation();
    emit finished();
}

void Model::pauseComputing(bool b) {
    m_mutex.lock();
    m_pause = b;
    if (!b)
        m_cond.wakeAll();
    m_mutex.unlock();
}

void Model::stopComputing() {
    m_mutex.lock();
    m_stop = true;
    m_cond.wakeAll();
    m_mutex.unlock();
}
