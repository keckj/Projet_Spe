
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

#if _N_MAIN == 0

using namespace log4cpp;
using namespace utils;

int main(int argc, char** argv) {
	
	log4cpp::initLogs();
	std::map<std::string, Grid<float> *> map;
	Grid2D<float> e(1.0f,1.0f,20u,20u);
	Grid2D<float> r(1.0f,1.0f,20u,20u);
	map["e"] = &e;
	map["r"] = &r;

	std::cout << e << std::endl;
	std::cout << r << std::endl;

	float r2 = 0.005;
	for (unsigned int j = 0; j < e.height(); j++) {
		for (unsigned int i = 0; i < e.width(); i++) {
			if(SQUARE(i*e.dh() - e.realWidth()/2.0f) + SQUARE(j*e.dh() - e.realHeight()/2.0f) < r2)
				e(i,j) = 1.0f;	
			else
				e(i,j) = 0.0f;	

			r(i,j) = 0.0f;	
		}
	}

	e.save("data/test.raw");
	
	float alpha1=0.2f, alpha2=0.1f;
	float mu1=0.07, mu2=0.3;
	float k=8.0f, epsilon=0.01, d=5e-5;
	SimpleSystem<float> system(&map, epsilon, k, d, mu1, mu2, alpha1, alpha2);
	Solver<float> solver;

	solver.solve(system, 1000.0f);

	exit(EXIT_SUCCESS);
}

#endif /* _N_MAIN == 0 */
