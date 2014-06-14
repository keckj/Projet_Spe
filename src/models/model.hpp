#ifndef MODEL_HPP_
#define MODEL_HPP_

#include "utils/headers.hpp"
#include "grid2D.hpp"
#include "grid3D.hpp"

class Model : public QObject {
    Q_OBJECT

    public:
        Model(int nbIter, std::map<QString, bool> *renderedVars, unsigned int width = 512, unsigned int height = 512, unsigned int length = 1); 
		virtual ~Model() {};

    public slots:
        void startComputing();
        void pauseComputing(bool b);
        void stopComputing();

    /* Copy signals, inheritance does not work properly */
    signals:
        void updateDisplay(const QMap<QString, GLuint> &texMap);
        void stepComputed();
        void finished();

    protected:
        unsigned int m_width, m_height, m_length;
        std::map<QString, bool> *m_renderedVars;
        QMap<QString, GLuint> m_mappedTextures;

        virtual void initComputation() = 0;
        virtual void computeStep(int i) = 0;
        virtual void finishComputation() = 0;
    
    private:
        QMutex m_mutex;
        QWaitCondition m_cond;
        int m_nbIter;
        bool m_pause, m_stop;
};

#endif
