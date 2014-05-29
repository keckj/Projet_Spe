

#include "headers.hpp"
#include "grid1D.hpp"
#include "grid2D.hpp"
#include "grid3D.hpp"

#include <cassert>
#include <vector>

#if _N_MAIN == 2

using namespace log4cpp;
using namespace utils;

int main(int argc, char** argv) {
	
	std::cout <<   "========================" << std::endl;
	std::cout << "\n== GRID TEST PROGRAMM ==" << std::endl;
	std::cout << "\n========================" << std::endl;

	std::cout << "\n== 3D GRIDS FLOAT ==" << std::endl;
	{
		std::vector<Grid3D<float> *> grids;

		grids.push_back(new Grid3D<float>(1.0f,2.0f,3.0f,100u,200u,300u, false));
		grids.push_back(new Grid3D<float>(1.0f,2.0f,3.0f,0.01f, false));
		grids.push_back(new Grid3D<float>(100u,200u,300u,0.01f, false));
		
		grids.push_back(new Grid3D<float>(*grids[0]));
		grids.push_back(new	Grid3D<float>(*grids[1]));
		grids.push_back(new	Grid3D<float>(*grids[2]));

		grids.push_back(new	Grid3D<float>(1.0f,2.0f,3.0f,100u,200u,300u, true));
		grids.push_back(new	Grid3D<float>(1.0f,2.0f,3.0f,0.01f, true));
		grids.push_back(new	Grid3D<float>(100u,200u,300u,0.01f, true));

		grids.push_back(new	Grid3D<float>(*grids[3]));
		grids.push_back(new	Grid3D<float>(*grids[4]));
		grids.push_back(new	Grid3D<float>(*grids[5]));
		
		unsigned int width=100u, height=200u, length=300u;
		float realWidth=1.0f, realHeight=2.0f, realLength=3.0f;
		float dh = 0.01f;
		for (int i = 0; i < 12; i++) {
			std::cout << "Checking 3D Grid<float> " << i << std::endl;
			assert(grids[i]->width() == width);
			assert(grids[i]->height() == height);
			assert(grids[i]->length() == length);
			assert(areEqual<float>(grids[i]->dh(),dh));
			assert(areEqual<float>(grids[i]->realWidth(),realWidth));
			assert(areEqual<float>(grids[i]->realHeight(),realHeight));
			assert(areEqual<float>(grids[i]->realLength(),realLength));
			assert(grids[i]->dim() == 3u);

			if(i < 6 || i >= 9) {
				assert(!grids[i]->isAllocated());
				assert(grids[i]->data() == 0);

				grids[i]->allocateOnCpu();
				assert(grids[i]->isAllocated());
				assert(grids[i]->data() != 0);

				grids[i]->freeOnCpu();
				assert(!grids[i]->isAllocated());
				assert(grids[i]->data() == 0);
			}
			else {
				assert(grids[i]->isAllocated());
				assert(grids[i]->data() != 0);

				grids[i]->freeOnCpu();
				assert(!grids[i]->isAllocated());
				assert(grids[i]->data() == 0);
			}
		}
	}
	
	std::cout << "\n== 2D GRIDS FLOAT ==" << std::endl;
	{
		std::vector<Grid2D<float> *> grids;

		grids.push_back(new Grid2D<float>(1.0f,2.0f,100u,200u, false));
		grids.push_back(new Grid2D<float>(1.0f,2.0f,0.01f, false));
		grids.push_back(new Grid2D<float>(100u,200u,0.01f, false));
		
		grids.push_back(new Grid2D<float>(*grids[0]));
		grids.push_back(new	Grid2D<float>(*grids[1]));
		grids.push_back(new	Grid2D<float>(*grids[2]));

		grids.push_back(new	Grid2D<float>(1.0f,2.0f,100u,200u, true));
		grids.push_back(new	Grid2D<float>(1.0f,2.0f,0.01f, true));
		grids.push_back(new	Grid2D<float>(100u,200u,0.01f, true));

		grids.push_back(new	Grid2D<float>(*grids[3]));
		grids.push_back(new	Grid2D<float>(*grids[4]));
		grids.push_back(new	Grid2D<float>(*grids[5]));
		
		unsigned int width=100u, height=200u, length=1u;
		float realWidth=1.0f, realHeight=2.0f, realLength=0.01f;
		float dh = 0.01f;
		for (int i = 0; i < 12; i++) {
			std::cout << "Checking 3D Grid<float> " << i << std::endl;
			assert(grids[i]->width() == width);
			assert(grids[i]->height() == height);
			assert(grids[i]->length() == length);
			assert(areEqual<float>(grids[i]->dh(),dh));
			assert(areEqual<float>(grids[i]->realWidth(),realWidth));
			assert(areEqual<float>(grids[i]->realHeight(),realHeight));
			assert(areEqual<float>(grids[i]->realLength(),realLength));
			assert(grids[i]->dim() == 2u);

			if(i < 6 || i >= 9) {
				assert(!grids[i]->isAllocated());
				assert(grids[i]->data() == 0);

				grids[i]->allocateOnCpu();
				assert(grids[i]->isAllocated());
				assert(grids[i]->data() != 0);

				grids[i]->freeOnCpu();
				assert(!grids[i]->isAllocated());
				assert(grids[i]->data() == 0);
			}
			else {
				assert(grids[i]->isAllocated());
				assert(grids[i]->data() != 0);

				grids[i]->freeOnCpu();
				assert(!grids[i]->isAllocated());
				assert(grids[i]->data() == 0);
			}
		}
	}
	
	std::cout << "\n== 1D GRIDS FLOAT ==" << std::endl;
	{
		std::vector<Grid1D<float> *> grids;

		grids.push_back(new Grid1D<float>(1.0f,100u,false));
		grids.push_back(new Grid1D<float>(1.0f,0.01f,false));
		grids.push_back(new Grid1D<float>(100u,0.01f,false));
		
		grids.push_back(new Grid1D<float>(*grids[0]));
		grids.push_back(new	Grid1D<float>(*grids[1]));
		grids.push_back(new	Grid1D<float>(*grids[2]));

		grids.push_back(new	Grid1D<float>(1.0f,100u,true));
		grids.push_back(new	Grid1D<float>(1.0f,0.01f,true));
		grids.push_back(new	Grid1D<float>(100u,0.01f,true));

		grids.push_back(new	Grid1D<float>(*grids[3]));
		grids.push_back(new	Grid1D<float>(*grids[4]));
		grids.push_back(new	Grid1D<float>(*grids[5]));
		
		unsigned int width=100u, height=1u, length=1u;
		float realWidth=1.0f, realHeight=0.01f, realLength=0.01f;
		float dh = 0.01f;
		for (int i = 0; i < 12; i++) {
			std::cout << "Checking 1D Grid<float> " << i << std::endl;
			assert(grids[i]->width() == width);
			assert(grids[i]->height() == height);
			assert(grids[i]->length() == length);
			assert(areEqual<float>(grids[i]->dh(),dh));
			assert(areEqual<float>(grids[i]->realWidth(),realWidth));
			assert(areEqual<float>(grids[i]->realHeight(),realHeight));
			assert(areEqual<float>(grids[i]->realLength(),realLength));
			assert(grids[i]->dim() == 1u);

			if(i < 6 || i >= 9) {
				assert(!grids[i]->isAllocated());
				assert(grids[i]->data() == 0);

				grids[i]->allocateOnCpu();
				assert(grids[i]->isAllocated());
				assert(grids[i]->data() != 0);

				grids[i]->freeOnCpu();
				assert(!grids[i]->isAllocated());
				assert(grids[i]->data() == 0);
			}
			else {
				assert(grids[i]->isAllocated());
				assert(grids[i]->data() != 0);

				grids[i]->freeOnCpu();
				assert(!grids[i]->isAllocated());
				assert(grids[i]->data() == 0);
			}
		}
	}
	
	std::cout << "\n== 3D GRIDS DOUBLE ==" << std::endl;
	{
		std::vector<Grid3D<double> *> grids;

		grids.push_back(new Grid3D<double>(1.0,2.0,3.0,100u,200u,300u, false));
		grids.push_back(new Grid3D<double>(1.0,2.0,3.0,0.01, false));
		grids.push_back(new Grid3D<double>(100u,200u,300u,0.01, false));
		
		grids.push_back(new Grid3D<double>(*grids[0]));
		grids.push_back(new	Grid3D<double>(*grids[1]));
		grids.push_back(new	Grid3D<double>(*grids[2]));

		grids.push_back(new	Grid3D<double>(1.0,2.0,3.0,100u,200u,300u, true));
		grids.push_back(new	Grid3D<double>(1.0,2.0,3.0,0.01, true));
		grids.push_back(new	Grid3D<double>(100u,200u,300u,0.01, true));

		grids.push_back(new	Grid3D<double>(*grids[3]));
		grids.push_back(new	Grid3D<double>(*grids[4]));
		grids.push_back(new	Grid3D<double>(*grids[5]));
		
		unsigned int width=100u, height=200u, length=300u;
		double realWidth=1.0, realHeight=2.0, realLength=3.0;
		double dh = 0.01;
		for (int i = 0; i < 12; i++) {
			std::cout << "Checking 3D Grid<double> " << i << std::endl;
			assert(grids[i]->width() == width);
			assert(grids[i]->height() == height);
			assert(grids[i]->length() == length);
			assert(areEqual<double>(grids[i]->dh(),dh));
			assert(areEqual<double>(grids[i]->realWidth(),realWidth));
			assert(areEqual<double>(grids[i]->realHeight(),realHeight));
			assert(areEqual<double>(grids[i]->realLength(),realLength));
			assert(grids[i]->dim() == 3u);

			if(i < 6 || i >= 9) {
				assert(!grids[i]->isAllocated());
				assert(grids[i]->data() == 0);

				grids[i]->allocateOnCpu();
				assert(grids[i]->isAllocated());
				assert(grids[i]->data() != 0);

				grids[i]->freeOnCpu();
				assert(!grids[i]->isAllocated());
				assert(grids[i]->data() == 0);
			}
			else {
				assert(grids[i]->isAllocated());
				assert(grids[i]->data() != 0);

				grids[i]->freeOnCpu();
				assert(!grids[i]->isAllocated());
				assert(grids[i]->data() == 0);
			}
		}
	}
	
	std::cout << "\n== 2D GRIDS DOUBLE ==" << std::endl;
	{
		std::vector<Grid2D<double> *> grids;

		grids.push_back(new Grid2D<double>(1.0,2.0,100u,200u, false));
		grids.push_back(new Grid2D<double>(1.0,2.0,0.01, false));
		grids.push_back(new Grid2D<double>(100u,200u,0.01, false));
		
		grids.push_back(new Grid2D<double>(*grids[0]));
		grids.push_back(new	Grid2D<double>(*grids[1]));
		grids.push_back(new	Grid2D<double>(*grids[2]));

		grids.push_back(new	Grid2D<double>(1.0,2.0,100u,200u, true));
		grids.push_back(new	Grid2D<double>(1.0,2.0,0.01, true));
		grids.push_back(new	Grid2D<double>(100u,200u,0.01, true));

		grids.push_back(new	Grid2D<double>(*grids[3]));
		grids.push_back(new	Grid2D<double>(*grids[4]));
		grids.push_back(new	Grid2D<double>(*grids[5]));
		
		unsigned int width=100u, height=200u, length=1u;
		double realWidth=1.0, realHeight=2.0, realLength=0.01;
		double dh = 0.01;
		for (int i = 0; i < 12; i++) {
			std::cout << "Checking 3D Grid<double> " << i << std::endl;
			assert(grids[i]->width() == width);
			assert(grids[i]->height() == height);
			assert(grids[i]->length() == length);
			assert(areEqual<double>(grids[i]->dh(),dh));
			assert(areEqual<double>(grids[i]->realWidth(),realWidth));
			assert(areEqual<double>(grids[i]->realHeight(),realHeight));
			assert(areEqual<double>(grids[i]->realLength(),realLength));
			assert(grids[i]->dim() == 2u);

			if(i < 6 || i >= 9) {
				assert(!grids[i]->isAllocated());
				assert(grids[i]->data() == 0);

				grids[i]->allocateOnCpu();
				assert(grids[i]->isAllocated());
				assert(grids[i]->data() != 0);

				grids[i]->freeOnCpu();
				assert(!grids[i]->isAllocated());
				assert(grids[i]->data() == 0);
			}
			else {
				assert(grids[i]->isAllocated());
				assert(grids[i]->data() != 0);

				grids[i]->freeOnCpu();
				assert(!grids[i]->isAllocated());
				assert(grids[i]->data() == 0);
			}
		}
	}
	
	std::cout << "\n== 1D GRIDS DOUBLE ==" << std::endl;
	{
		std::vector<Grid1D<double> *> grids;

		grids.push_back(new Grid1D<double>(1.0,100u,false));
		grids.push_back(new Grid1D<double>(1.0,0.01,false));
		grids.push_back(new Grid1D<double>(100u,0.01,false));
		
		grids.push_back(new Grid1D<double>(*grids[0]));
		grids.push_back(new	Grid1D<double>(*grids[1]));
		grids.push_back(new	Grid1D<double>(*grids[2]));

		grids.push_back(new	Grid1D<double>(1.0,100u,true));
		grids.push_back(new	Grid1D<double>(1.0,0.01,true));
		grids.push_back(new	Grid1D<double>(100u,0.01,true));

		grids.push_back(new	Grid1D<double>(*grids[3]));
		grids.push_back(new	Grid1D<double>(*grids[4]));
		grids.push_back(new	Grid1D<double>(*grids[5]));
		
		unsigned int width=100u, height=1u, length=1u;
		double realWidth=1.0, realHeight=0.01, realLength=0.01;
		double dh = 0.01;
		for (int i = 0; i < 12; i++) {
			std::cout << "Checking 1D Grid<double> " << i << std::endl;
			assert(grids[i]->width() == width);
			assert(grids[i]->height() == height);
			assert(grids[i]->length() == length);
			assert(areEqual<double>(grids[i]->dh(),dh));
			assert(areEqual<double>(grids[i]->realWidth(),realWidth));
			assert(areEqual<double>(grids[i]->realHeight(),realHeight));
			assert(areEqual<double>(grids[i]->realLength(),realLength));
			assert(grids[i]->dim() == 1u);

			if(i < 6 || i >= 9) {
				assert(!grids[i]->isAllocated());
				assert(grids[i]->data() == 0);

				grids[i]->allocateOnCpu();
				assert(grids[i]->isAllocated());
				assert(grids[i]->data() != 0);

				grids[i]->freeOnCpu();
				assert(!grids[i]->isAllocated());
				assert(grids[i]->data() == 0);
			}
			else {
				assert(grids[i]->isAllocated());
				assert(grids[i]->data() != 0);

				grids[i]->freeOnCpu();
				assert(!grids[i]->isAllocated());
				assert(grids[i]->data() == 0);
			}
		}
	}

}

#endif
