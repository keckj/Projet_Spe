#ifndef MODEL_HPP_
#define MODEL_HPP_

#include "utils/headers.hpp"
#include "grid2D.hpp"
#include "grid3D.hpp"
#include "initialCond.hpp"

class Model : public QObject {
    Q_OBJECT

    public:
        Model(int nbIter, 
			std::map<QString, bool> *renderedVars, 
			std::map<QString,int> *initialCondsId, 
			unsigned int width = 512, unsigned int height = 512, unsigned int length = 1); 

		virtual ~Model();
		
		static Display *solverDisplay;
		static GLXContext solverContext;
		static Window solverWindow;
		static Colormap solverColormap;

    protected:
        unsigned int m_width, m_height, m_length;
        std::map<QString, bool> *m_renderedVars;
        std::map<QString, int> *m_initialCondsId;
        std::map<QString, InitialCond<float>*> m_initialConds;
        QMap<QString, GLuint> m_mappedTextures;
        QMap<QString, GLuint> m_mappedTexturesGui;

        virtual void initComputation() = 0;
        virtual void computeStep(int i) = 0;
        virtual void finishComputation() = 0;
    private:
        QMutex m_mutex;
        QWaitCondition m_cond;
        int m_nbIter;
        bool m_pause, m_stop;
    
	public slots:
        void startComputing();
        void pauseComputing(bool b);
        void stopComputing();

    signals:
        void updateDisplay(const QMap<QString, GLuint> &texMap);
        void stepComputed();
        void finished();

		void showCodeEditor(int);
		void codeSubmitted(const QString &);
};

#endif
