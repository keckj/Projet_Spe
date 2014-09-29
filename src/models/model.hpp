#ifndef MODEL_HPP_
#define MODEL_HPP_

#include "headers.hpp"
#include "grid2D.hpp"
#include "grid3D.hpp"
#include "initialCond.hpp"

class Model : public QObject {
    Q_OBJECT

    public:
        Model(unsigned int nbIter, 
			std::map<QString, bool> *renderedVars, 
			std::map<QString,int> *initialCondsId, 
			unsigned int width = 512, unsigned int height = 512, unsigned int length = 1,
			bool save=false, const QString &directory="", unsigned int saveRate=0); 

		virtual ~Model();
		
		static Display *solverDisplay;
		static GLXContext solverContext;
		static Window solverWindow;
		static Colormap solverColormap;

    protected:
        unsigned int m_width, m_height, m_length;
		unsigned int m_stepId;
        std::map<QString, bool> *m_renderedVars;
        std::map<QString, int> *m_initialCondsId;
        std::map<QString, InitialCond<float>*> m_initialConds;
        QMap<QString, GLuint> m_mappedTextures;
        QMap<QString, GLuint> m_mappedTexturesGui;
		
		bool m_save;
		const QString m_saveDirectory;
		unsigned int m_saveRate;
		std::map<std::string, Grid<float>*> m_gridsToSave;
	
        virtual void initComputation() = 0;
        virtual void computeStep(int i) = 0;
        virtual void finishComputation() = 0;
		virtual void saveGrid(std::string varName, std::string prefix, std::string suffix);
    private:
        QMutex m_mutex;
        QWaitCondition m_cond;
        unsigned int m_nbIter;
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
