
#ifndef SIMPLEMODEL2D_H
#define SIMPLEMODEL2D_H

#include "model.hpp"
#include "argument.hpp"

class SimpleModel2D : public Model {
    Q_OBJECT

    public:
        /*SimpleModel2D(int nbIter,
			unsigned int width = 200, unsigned int height = 200,
			float epsilon_=0.01f, float k_=8.0f, float d_=5e-5,
			float mu_1_=0.07, float mu_2_=0.3f,
			float alpha_1_=0.2f, float alpha_2_=0.1f);
        */
		SimpleModel2D(unsigned int nbIter,
				std::map<QString, Argument> *args, 
                std::map<QString, bool> *vars,
				std::map<QString,int> *initialCondsId, 
				unsigned int width = 200, unsigned int height = 200);

        ~SimpleModel2D();

        void initComputation() override;
        void computeStep(int i) override;
        void finishComputation() override;

		static std::map<QString, Argument> *getArguments();
        static std::map<QString, bool> *getVariables();

	private:
		float computeOptimalTimestep();
		void subStep(unsigned long offset, unsigned long subworkSize);
	
		float F(unsigned int i, unsigned int j);
		float G(unsigned int i, unsigned int j);
		float L(unsigned int i, unsigned int j);
	
		unsigned int _width, _height;
		float _dh, _dt;
		Grid2D<float> *_e1, *_e2, *_r1, *_r2;

		float _epsilon, _k, _d;
		float _mu_1, _mu_2;
		float _alpha_1, _alpha_2;
    
    signals:
		void stepComputed(const Grid2D<float> *grid);
		void stepComputed(const Grid3D<float> *grid);
};



#endif /* end of include guard: SIMPLEMODEL2D_H */
