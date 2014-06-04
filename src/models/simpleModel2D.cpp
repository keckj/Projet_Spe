
#include "headers.hpp"
#include "simpleModel2D.hpp"
#include "simpleModel2D.moc"
#include "grid2D.hpp"
#include "utils.hpp"

#include <algorithm>
#include <thread>
#include <vector>

SimpleModel2D::SimpleModel2D(int nbIter,
			unsigned int width_, unsigned int height_,
			float epsilon_, float k_, float d_,
			float mu_1_, float mu_2_,
			float alpha_1_, float alpha_2_) :
	Model(nbIter),
	_width(width_), _height(height_),
	_dh(0), _dt(0), 
	_e1(0), _e2(0), _r1(0), _r2(0),
	_epsilon(epsilon_), _k(k_), _d(d_),
	_mu_1(mu_1_), _mu_2(mu_2_),
	_alpha_1(alpha_1_), _alpha_2(alpha_2_)
{
	_e1 = new Grid2D<float>(_width, _height, 0.01f);
	_e2 = new Grid2D<float>(_width, _height, 0.01f);
	_r1 = new Grid2D<float>(_width, _height, 0.01f);
	_r2 = new Grid2D<float>(_width, _height, 0.01f);

	_dh = _e1->dh();
	_dt = computeOptimalTimestep();
	
	log_console->debugStream() << "Created a Simple Model 2D !";
	log_console->debugStream() << "dh " << _dh << " !";
	log_console->debugStream() << "dt " << _dt << " !";
	log_console->debugStream() << "Size " << _e1->size() << " !";
}
	
SimpleModel2D::SimpleModel2D(unsigned int nbIter,
		std::map<std::string, Argument> *args, 
		unsigned int width_, unsigned int height_) :
	Model(nbIter),
	_width(width_), _height(height_)
{
	_epsilon = args->at("epsilon")();
	_d = args->at("d")();
	_mu_1 = args->at("mu_1")();
	_mu_2 = args->at("mu_2")();
	_alpha_1 = args->at("alpha_1")();
	_alpha_2 = args->at("alpha_2")();
}


SimpleModel2D::~SimpleModel2D() {
	delete _e1;
	delete _e2;
	delete _r1;
	delete _r2;
}
		
void SimpleModel2D::initComputation() {
	float r2 = 0.005;
	for (unsigned int j = 0; j < _height; j++) {
		for (unsigned int i = 0; i < _width; i++) {
			(*_e1)(i,j) = 0.0f;	
			(*_r1)(i,j) = 0.0f;	
			
			if(SQUARE(i*_dh - _e1->realWidth()/2.0f) + SQUARE(j*_dh - _e1->realHeight()/2.0f) < r2)
				(*_e1)(i,j) = 1.0f;	
		}
	}
}


void SimpleModel2D::computeStep(int i) {
	
	unsigned long workSize = _e1->size();	

	//unsigned int nThread = 4;
	//unsigned long subworkSize = workSize/nThread;
	//unsigned long lastWork = workSize%nThread;

	//std::list<std::thread> threads;
	//for (unsigned long j = 0; j < nThread; j++) {
		//threads.push_back(std::thread(&SimpleModel2D::subStep, this, j*subworkSize, subworkSize));
	//}
	//subStep(nThread*subworkSize, lastWork);
	//for (auto& it : threads) {
		//it.join();
	//}
	//
	subStep(0,workSize);
	
	emit stepComputed(_e1);

	std::swap(_e1, _e2);
	std::swap(_r1, _r2);
}

void SimpleModel2D::subStep(unsigned long offset, unsigned long subworkSize) {
	unsigned int i, j;
	for (unsigned long id = offset; id < offset+subworkSize; id++) {
		j = id / _width;
		i = id % _width;
		(*_e2)(i,j) = (*_e1)(i,j) + _dt * (L(i,j) + F(i,j)); 
		(*_r2)(i,j) = (*_r1)(i,j) + _dt * G(i,j); 
	}
}


void SimpleModel2D::finishComputation() {
}

float SimpleModel2D::computeOptimalTimestep() {
	float lhs = SQUARE(_dh) / 
		(4*_d + _k*SQUARE(_dh)/4 *(SQUARE(_alpha_1+1) + 1));

	float rhs = 1 /
		(_epsilon + _mu_1/_mu_2 * SQUARE(_dh)/4 * SQUARE(_alpha_1 + 1));

	return 0.95*std::min(lhs, rhs);
}

float SimpleModel2D::F(unsigned int i, unsigned int j) {
	float E_ijk = (*_e1)(i,j);
	float R_ijk = (*_r1)(i,j);
	return -_k*E_ijk*(E_ijk - _alpha_1)*(E_ijk - 1) - E_ijk*R_ijk;
}

float SimpleModel2D::G(unsigned int i, unsigned int j) {
	float E_ijk = (*_e1)(i,j);
	float R_ijk = (*_r1)(i,j);
	return (_epsilon + _mu_1*R_ijk/(E_ijk + _mu_2)) * (-R_ijk - _k*E_ijk*(E_ijk-_alpha_2-1));
}

float SimpleModel2D::L(unsigned int i, unsigned int j) {
	float E_ijk = (*_e1)(i,j);
	float E_left = (*_e1)(i == 0 ? 0 : i - 1, j);
	float E_right = (*_e1)(i == _width-1 ? _width-1 : i + 1, j);
	float E_up = (*_e1)(i, j == 0 ? 0 : j - 1);
	float E_down = (*_e1)(i, j == _height-1 ? _height-1 : j + 1);

	return _d/(_dh*_dh) * (E_left + E_right + E_up + E_down - 4*E_ijk);
}

std::map<std::string, Argument> *SimpleModel2D::getArguments() {
	
	std::map<std::string,Argument> *args = new std::map<std::string,Argument>;

	args->emplace("epsilon", Argument(0.01f, 0.001f, 0.1f, WidgetType::SLIDER));
	args->emplace("d",Argument(1e-5f, 5e-5f, 1e-4f, WidgetType::SLIDER));
	args->emplace("mu_1",Argument(0.01f, 0.07f, 0.5f, WidgetType::SLIDER));
	args->emplace("mu_2",Argument(0.1f, 0.3f, 0.5f, WidgetType::SLIDER));
	args->emplace("alpha_1",Argument(0.1f, 0.2f, 0.5f, WidgetType::SLIDER));
	args->emplace("alpha_2",Argument(0.1f, 0.1f, 0.5f, WidgetType::SLIDER));

	return args;
}
