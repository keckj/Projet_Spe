#include "headers.hpp"
#include "model.hpp"
#include "model.moc"
#include "log.hpp"

void Model::startComputing() {
		
	log_console->debugStream() << "Start Computing.";

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

		//log_console->debugStream() << "Computing step " << i << ".";
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
