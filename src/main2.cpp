
#include "headers.hpp"
#include "utils.hpp"
#include "mainWindow.hpp"

#include "grid1D.hpp"
#include "grid2D.hpp"
#include "grid3D.hpp"
#include "simpleSystem.hpp"
#include "solver.hpp"

#include <cassert>
#include <ctime>
#include <vector>
#include <iostream>
#include <map>

#if _N_MAIN == 1

int main(int argc, char** argv) {
	log4cpp::initLogs();

	unsigned int width=200u, height=200u;
	std::cout << "Je suis le main 1 !" << std::endl;
	Grid2D<float> e(1.0,1.0,width,height);
	Grid2D<float> r(1.0,1.0,width,height);

	float r2 = 0.005;
	for (unsigned int j = 0; j < e.height(); j++) {
		for (unsigned int i = 0; i < e.width(); i++) {
			e(i,j) = 0.0f;	
			r(i,j) = 0.0f;	
			
			if(SQUARE(i*e.dh() - e.realWidth()/2.0f) + SQUARE(j*e.dh() - e.realHeight()/2.0f) < r2)
				e(i,j) = 1.0f;	
		}
	}

	std::map<std::string, Grid<float> *> map;
	map["e"] = &e;
	map["r"] = &r;

	float alpha1=0.2f, alpha2=0.1f;
	float mu1=0.07, mu2=0.3;
	float k=8.0f, epsilon=0.01, d=5e-5;
	SimpleSystem<float> system(&map, epsilon, k, d, mu1, mu2, alpha1, alpha2);
	Solver<float> solver;

	unsigned int nIterations = solver.solve(system, 10.0f);

	makeGnuPlotScript("data/e.plot", "data/prefix_", "e", "_suffix.dat",0,nIterations,0,width,0,height,0.1,true,"data/e.gif");
	execGnuPlotScript("data/e.plot");
	showGif("data/e.gif");

	exit(EXIT_SUCCESS);
}

#endif
