#ifndef MODEL_HPP_
#define MODEL_HPP_

#include "utils/headers.hpp"
#include "grid2D.hpp"
#include "grid3D.hpp"

class Model : public QObject {
    Q_OBJECT

    public:
        Model(int nbIter, unsigned int width = 200, unsigned int height = 200, unsigned int length = 1); 
		virtual ~Model() {};

    protected:
        virtual void initComputation() = 0;
        virtual void computeStep(int i) = 0;
        //virtual std::map<std::string, GLuint> computeStep(int i) = 0;
        virtual void finishComputation() = 0;

    public slots:
        void startComputing();
        void pauseComputing(bool b);
        void stopComputing();
        //TODO
        //void addTexture(QString texName);
        //void removeTexture(QString texName);

    /* Copy signals, inheritance does not work properly */
    signals:
        void stepComputed(const Grid2D<float> *grid);
        void stepComputed(const Grid3D<float> *grid);
        void stepComputed(const QMap<QString, GLuint> &texMap);
        void finished();

    protected:
        unsigned int m_width, m_height, m_length;

    private:
        int m_nbIter;
        QMutex m_mutex;
        QWaitCondition m_cond;
        bool m_pause, m_stop;
};

#endif
