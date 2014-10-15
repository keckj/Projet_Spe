

#include "headers.hpp"
#include "grid.hpp"
#include "grid1D.hpp"
#include "grid2D.hpp"
#include "grid3D.hpp"
#include "log.hpp"

#include <cassert>
#include <vector>

#if _N_MAIN == 2

using namespace log4cpp;
using namespace utils;

int main(int argc, char** argv) {

	log4cpp::initLogs();
	
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

				grids[i]->allocate();
				assert(grids[i]->isAllocated());
				assert(grids[i]->data() != 0);

				grids[i]->free();
				assert(!grids[i]->isAllocated());
				assert(grids[i]->data() == 0);
			}
			else {
				assert(grids[i]->isAllocated());
				assert(grids[i]->data() != 0);

				grids[i]->free();
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

				grids[i]->allocate();
				assert(grids[i]->isAllocated());
				assert(grids[i]->data() != 0);

				grids[i]->free();
				assert(!grids[i]->isAllocated());
				assert(grids[i]->data() == 0);
			}
			else {
				assert(grids[i]->isAllocated());
				assert(grids[i]->data() != 0);

				grids[i]->free();
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

				grids[i]->allocate();
				assert(grids[i]->isAllocated());
				assert(grids[i]->data() != 0);

				grids[i]->free();
				assert(!grids[i]->isAllocated());
				assert(grids[i]->data() == 0);
			}
			else {
				assert(grids[i]->isAllocated());
				assert(grids[i]->data() != 0);

				grids[i]->free();
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

				grids[i]->allocate();
				assert(grids[i]->isAllocated());
				assert(grids[i]->data() != 0);

				grids[i]->free();
				assert(!grids[i]->isAllocated());
				assert(grids[i]->data() == 0);
			}
			else {
				assert(grids[i]->isAllocated());
				assert(grids[i]->data() != 0);

				grids[i]->free();
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

				grids[i]->allocate();
				assert(grids[i]->isAllocated());
				assert(grids[i]->data() != 0);

				grids[i]->free();
				assert(!grids[i]->isAllocated());
				assert(grids[i]->data() == 0);
			}
			else {
				assert(grids[i]->isAllocated());
				assert(grids[i]->data() != 0);

				grids[i]->free();
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

				grids[i]->allocate();
				assert(grids[i]->isAllocated());
				assert(grids[i]->data() != 0);

				grids[i]->free();
				assert(!grids[i]->isAllocated());
				assert(grids[i]->data() == 0);
			}
			else {
				assert(grids[i]->isAllocated());
				assert(grids[i]->data() != 0);

				grids[i]->free();
				assert(!grids[i]->isAllocated());
				assert(grids[i]->data() == 0);
			}
		}
	}

	std::cout << "\n== 1D FLOAT GRID SERIALIZATION ==" << std::endl;
	{
		Grid1D<float> fg1(1.0f,10u, false);
		fg1.exportGrid("data/exportGrid.raw");
		Grid1D<float> fg2("data/exportGrid.raw");
		assert(fg1.width() == fg2.width());
		assert(fg1.height() == fg2.height());
		assert(fg1.length() == fg2.length());
		assert(areEqual<float>(fg1.realWidth(), fg2.realWidth()));
		assert(areEqual<float>(fg1.realHeight(), fg2.realHeight()));
		assert(areEqual<float>(fg1.realLength(), fg2.realLength()));
		assert(areEqual<float>(fg1.dh(), fg2.dh()));
		assert(fg1.dim() == fg2.dim());
		assert(fg1.isAllocated() == fg2.isAllocated());
		std::cout << "No data : ok" << std::endl;

		Grid1D<float> fg3(1.0f,10u, true);
		for(unsigned int i = 0; i < fg3.size(); i++) fg3[i] = i;
		fg3.exportGrid("data/exportGrid.raw");
		Grid1D<float> fg4("data/exportGrid.raw");
		assert(fg3.width() == fg4.width());
		assert(fg3.height() == fg4.height());
		assert(fg3.length() == fg4.length());
		assert(areEqual<float>(fg3.realWidth(), fg4.realWidth()));
		assert(areEqual<float>(fg3.realHeight(), fg4.realHeight()));
		assert(areEqual<float>(fg3.realLength(), fg4.realLength()));
		assert(areEqual<float>(fg3.dh(), fg4.dh()));
		assert(fg3.dim() == fg4.dim());
		assert(fg3.isAllocated() == fg4.isAllocated());
		for (unsigned int i = 0; i < fg3.size(); i++) assert(fg3[i]==fg4[i]);
		std::cout << "With data : ok" << std::endl;
	}
	
	std::cout << "\n== 2D FLOAT GRID SERIALIZATION ==" << std::endl;
	{
		Grid2D<float> fg1(1.0f,2.0f,10u,20u,false);
		fg1.exportGrid("data/exportGrid.raw");
		Grid2D<float> fg2("data/exportGrid.raw");
		assert(fg1.width() == fg2.width());
		assert(fg1.height() == fg2.height());
		assert(fg1.length() == fg2.length());
		assert(areEqual<float>(fg1.realWidth(), fg2.realWidth()));
		assert(areEqual<float>(fg1.realHeight(), fg2.realHeight()));
		assert(areEqual<float>(fg1.realLength(), fg2.realLength()));
		assert(areEqual<float>(fg1.dh(), fg2.dh()));
		assert(fg1.dim() == fg2.dim());
		assert(fg1.isAllocated() == fg2.isAllocated());
		std::cout << "No data : ok" << std::endl;

		Grid2D<float> fg3(1.0f,2.0f,10u,20u,true);
		for(unsigned int i = 0; i < fg3.size(); i++) fg3[i] = i;
		fg3.exportGrid("data/exportGrid.raw");
		Grid2D<float> fg4("data/exportGrid.raw");
		assert(fg3.width() == fg4.width());
		assert(fg3.height() == fg4.height());
		assert(fg3.length() == fg4.length());
		assert(areEqual<float>(fg3.realWidth(), fg4.realWidth()));
		assert(areEqual<float>(fg3.realHeight(), fg4.realHeight()));
		assert(areEqual<float>(fg3.realLength(), fg4.realLength()));
		assert(areEqual<float>(fg3.dh(), fg4.dh()));
		assert(fg3.dim() == fg4.dim());
		assert(fg3.isAllocated() == fg4.isAllocated());
		for (unsigned int i = 0; i < fg3.size(); i++) assert(fg3[i]==fg4[i]);
		std::cout << "With data : ok" << std::endl;
	}
	
	std::cout << "\n== 3D FLOAT GRID SERIALIZATION ==" << std::endl;
	{
		Grid3D<float> fg1(1.0f,2.0f,3.0f,10u,20u,30u,false);
		fg1.exportGrid("data/exportGrid.raw");
		Grid3D<float> fg2("data/exportGrid.raw");
		assert(fg1.width() == fg2.width());
		assert(fg1.height() == fg2.height());
		assert(fg1.length() == fg2.length());
		assert(areEqual<float>(fg1.realWidth(), fg2.realWidth()));
		assert(areEqual<float>(fg1.realHeight(), fg2.realHeight()));
		assert(areEqual<float>(fg1.realLength(), fg2.realLength()));
		assert(areEqual<float>(fg1.dh(), fg2.dh()));
		assert(fg1.dim() == fg2.dim());
		assert(fg1.isAllocated() == fg2.isAllocated());
		std::cout << "No data : ok" << std::endl;

		Grid3D<float> fg3(1.0f,2.0f,3.0f,10u,20u,30u,true);
		for(unsigned int i = 0; i < fg3.size(); i++) fg3[i] = i;
		fg3.exportGrid("data/exportGrid.raw");
		Grid3D<float> fg4("data/exportGrid.raw");
		assert(fg3.width() == fg4.width());
		assert(fg3.height() == fg4.height());
		assert(fg3.length() == fg4.length());
		assert(areEqual<float>(fg3.realWidth(), fg4.realWidth()));
		assert(areEqual<float>(fg3.realHeight(), fg4.realHeight()));
		assert(areEqual<float>(fg3.realLength(), fg4.realLength()));
		assert(areEqual<float>(fg3.dh(), fg4.dh()));
		assert(fg3.dim() == fg4.dim());
		assert(fg3.isAllocated() == fg4.isAllocated());
		for (unsigned int i = 0; i < fg3.size(); i++) assert(fg3[i]==fg4[i]);
		std::cout << "With data : ok" << std::endl;
	}

	std::cout << "\n== 1D DOUBLE GRID SERIALIZATION ==" << std::endl;
	{
		Grid1D<double> fg1(1.0,10u, false);
		fg1.exportGrid("data/exportGrid.raw");
		Grid1D<double> fg2("data/exportGrid.raw");
		assert(fg1.width() == fg2.width());
		assert(fg1.height() == fg2.height());
		assert(fg1.length() == fg2.length());
		assert(areEqual<double>(fg1.realWidth(), fg2.realWidth()));
		assert(areEqual<double>(fg1.realHeight(), fg2.realHeight()));
		assert(areEqual<double>(fg1.realLength(), fg2.realLength()));
		assert(areEqual<double>(fg1.dh(), fg2.dh()));
		assert(fg1.dim() == fg2.dim());
		assert(fg1.isAllocated() == fg2.isAllocated());
		std::cout << "No data : ok" << std::endl;

		Grid1D<double> fg3(1.0,10u, true);
		for(unsigned int i = 0; i < fg3.size(); i++) fg3[i] = i;
		fg3.exportGrid("data/exportGrid.raw");
		Grid1D<double> fg4("data/exportGrid.raw");
		assert(fg3.width() == fg4.width());
		assert(fg3.height() == fg4.height());
		assert(fg3.length() == fg4.length());
		assert(areEqual<double>(fg3.realWidth(), fg4.realWidth()));
		assert(areEqual<double>(fg3.realHeight(), fg4.realHeight()));
		assert(areEqual<double>(fg3.realLength(), fg4.realLength()));
		assert(areEqual<double>(fg3.dh(), fg4.dh()));
		assert(fg3.dim() == fg4.dim());
		assert(fg3.isAllocated() == fg4.isAllocated());
		for (unsigned int i = 0; i < fg3.size(); i++) assert(fg3[i]==fg4[i]);
		std::cout << "With data : ok" << std::endl;
	}
	
	std::cout << "\n== 2D DOUBLE GRID SERIALIZATION ==" << std::endl;
	{
		Grid2D<double> fg1(1.0,2.0,10u,20u,false);
		fg1.exportGrid("data/exportGrid.raw");
		Grid2D<double> fg2("data/exportGrid.raw");
		assert(fg1.width() == fg2.width());
		assert(fg1.height() == fg2.height());
		assert(fg1.length() == fg2.length());
		assert(areEqual<double>(fg1.realWidth(), fg2.realWidth()));
		assert(areEqual<double>(fg1.realHeight(), fg2.realHeight()));
		assert(areEqual<double>(fg1.realLength(), fg2.realLength()));
		assert(areEqual<double>(fg1.dh(), fg2.dh()));
		assert(fg1.dim() == fg2.dim());
		assert(fg1.isAllocated() == fg2.isAllocated());
		std::cout << "No data : ok" << std::endl;

		Grid2D<double> fg3(1.0,2.0,10u,20u,true);
		for(unsigned int i = 0; i < fg3.size(); i++) fg3[i] = i;
		fg3.exportGrid("data/exportGrid.raw");
		Grid2D<double> fg4("data/exportGrid.raw");
		assert(fg3.width() == fg4.width());
		assert(fg3.height() == fg4.height());
		assert(fg3.length() == fg4.length());
		assert(areEqual<double>(fg3.realWidth(), fg4.realWidth()));
		assert(areEqual<double>(fg3.realHeight(), fg4.realHeight()));
		assert(areEqual<double>(fg3.realLength(), fg4.realLength()));
		assert(areEqual<double>(fg3.dh(), fg4.dh()));
		assert(fg3.dim() == fg4.dim());
		assert(fg3.isAllocated() == fg4.isAllocated());
		for (unsigned int i = 0; i < fg3.size(); i++) assert(fg3[i]==fg4[i]);
		std::cout << "With data : ok" << std::endl;
	}
	
	std::cout << "\n== 3D DOUBLE GRID SERIALIZATION ==" << std::endl;
	{
		Grid3D<double> fg1(1.0,2.0,3.0,10u,20u,30u,false);
		fg1.exportGrid("data/exportGrid.raw");
		Grid3D<double> fg2("data/exportGrid.raw");
		assert(fg1.width() == fg2.width());
		assert(fg1.height() == fg2.height());
		assert(fg1.length() == fg2.length());
		assert(areEqual<double>(fg1.realWidth(), fg2.realWidth()));
		assert(areEqual<double>(fg1.realHeight(), fg2.realHeight()));
		assert(areEqual<double>(fg1.realLength(), fg2.realLength()));
		assert(areEqual<double>(fg1.dh(), fg2.dh()));
		assert(fg1.dim() == fg2.dim());
		assert(fg1.isAllocated() == fg2.isAllocated());
		std::cout << "No data : ok" << std::endl;

		Grid3D<double> fg3(1.0,2.0,3.0,10u,20u,30u,true);
		for(unsigned int i = 0; i < fg3.size(); i++) fg3[i] = i;
		fg3.exportGrid("data/exportGrid.raw");
		Grid3D<double> fg4("data/exportGrid.raw");
		assert(fg3.width() == fg4.width());
		assert(fg3.height() == fg4.height());
		assert(fg3.length() == fg4.length());
		assert(areEqual<double>(fg3.realWidth(), fg4.realWidth()));
		assert(areEqual<double>(fg3.realHeight(), fg4.realHeight()));
		assert(areEqual<double>(fg3.realLength(), fg4.realLength()));
		assert(areEqual<double>(fg3.dh(), fg4.dh()));
		assert(fg3.dim() == fg4.dim());
		assert(fg3.isAllocated() == fg4.isAllocated());
		for (unsigned int i = 0; i < fg3.size(); i++) assert(fg3[i]==fg4[i]);
		std::cout << "With data : ok" << std::endl;
	}
	
	std::cout << "\n== RANDOM FLOAT GRID SERIALIZATION ==" << std::endl;
	{
		std::vector<Grid<float> * > fgrids;
		fgrids.push_back(new Grid1D<float>(1.0,10u,true));
		fgrids.push_back(new Grid2D<float>(1.0,2.0,10u,20u,true));
		fgrids.push_back(new Grid3D<float>(1.0,2.0,3.0,10u,20u,30u,true));
		fgrids.push_back(new Grid1D<float>(1.0,10u,false));
		fgrids.push_back(new Grid2D<float>(1.0,2.0,10u,20u,false));
		fgrids.push_back(new Grid3D<float>(1.0,2.0,3.0,10u,20u,30u,false));
		
		for (Grid<float> *g : fgrids) {
			g->exportGrid("data/exportGrid.raw");
			Grid<float> g2("data/exportGrid.raw");
			assert(g->width() == g2.width());
			assert(g->height() == g2.height());
			assert(g->length() == g2.length());
			assert(areEqual<float>(g->realWidth(), g2.realWidth()));
			assert(areEqual<float>(g->realHeight(), g2.realHeight()));
			assert(areEqual<float>(g->realLength(), g2.realLength()));
			assert(areEqual<float>(g->dh(), g2.dh()));
			assert(g->dim() == g2.dim());
			assert(g->isAllocated() == g2.isAllocated());
		}
		std::cout << "ok" << std::endl;
	}
	
	std::cout << "\n== RANDOM DOUBLE GRID SERIALIZATION ==" << std::endl;
	{
		std::vector<Grid<double> * > fgrids;
		fgrids.push_back(new Grid1D<double>(1.0,10u,true));
		fgrids.push_back(new Grid2D<double>(1.0,2.0,10u,20u,true));
		fgrids.push_back(new Grid3D<double>(1.0,2.0,3.0,10u,20u,30u,true));
		fgrids.push_back(new Grid1D<double>(1.0,10u,false));
		fgrids.push_back(new Grid2D<double>(1.0,2.0,10u,20u,false));
		fgrids.push_back(new Grid3D<double>(1.0,2.0,3.0,10u,20u,30u,false));
		
		for (Grid<double> *g : fgrids) {
			g->exportGrid("data/exportGrid.raw");
			Grid<double> g2("data/exportGrid.raw");
			assert(g->width() == g2.width());
			assert(g->height() == g2.height());
			assert(g->length() == g2.length());
			assert(areEqual<double>(g->realWidth(), g2.realWidth()));
			assert(areEqual<double>(g->realHeight(), g2.realHeight()));
			assert(areEqual<double>(g->realLength(), g2.realLength()));
			assert(areEqual<double>(g->dh(), g2.dh()));
			assert(g->dim() == g2.dim());
			assert(g->isAllocated() == g2.isAllocated());
		}
		std::cout << "ok" << std::endl;
	}
	
	std::cout << "\n== GRID DATA OWNERSHIP ==" << std::endl;
	{
		Grid3D<float> g(1.0,2.0,3.0,10u,20u,30u,false);
		assert(!g.isOwner());
		assert(!g.isAllocated());
		g.allocate();
		assert(g.isOwner());
		assert(g.isAllocated());
		g.free();
		assert(!g.isOwner());
		assert(!g.isAllocated());
		g.setData(0,false);
		assert(!g.isOwner());
		assert(!g.isAllocated());
		g.setData(0,true);
		assert(!g.isOwner());
		assert(!g.isAllocated());
		float *data = new float[g.size()];
		g.setData(data,false);
		assert(!g.isOwner());
		assert(g.isAllocated());
		assert(g.data() == data);
		g.free();
		assert(!g.isOwner());
		assert(!g.isAllocated());
		assert(g.data() == 0);
		data[0] = 10;
		g.setData(data,true);
		assert(g.isOwner());
		assert(g.isAllocated());
		assert(g.data() == data);
		g.free();
		assert(!g.isOwner());
		assert(!g.isAllocated());
		assert(g.data() == 0);
		std::cout << "allocate setData free : ok" << std::endl;

		{
			Grid3D<float> g1(1.0,2.0,3.0,10u,20u,30u,true);
			Grid3D<float> g2(g1, true);
			assert(g2.data() == g1.data());
			assert(g2.isAllocated());
			assert(!g2.isOwner());
			Grid3D<float> g3(g1, false);
			assert(g3.data() == 0);
			assert(!g3.isAllocated());
			assert(!g3.isOwner());
		}	
		{
			Grid2D<float> g1(1.0,2.0,10u,20u,true);
			Grid2D<float> g2(g1, true);
			assert(g2.data() == g1.data());
			assert(g2.isAllocated());
			assert(!g2.isOwner());
			Grid2D<float> g3(g1, false);
			assert(g3.data() == 0);
			assert(!g3.isAllocated());
			assert(!g3.isOwner());
		}
		{
			Grid1D<float> g1(1.0,10u,true);
			Grid1D<float> g2(g1, true);
			assert(g2.data() == g1.data());
			assert(g2.isAllocated());
			assert(!g2.isOwner());
			Grid1D<float> g3(g1, false);
			assert(g3.data() == 0);
			assert(!g3.isAllocated());
			assert(!g3.isOwner());
		}
		std::cout << "copy constructors : ok" << std::endl;

		{
			Grid3D<float> g1(1.0,2.0,3.0,10u,20u,30u,true);
			Grid<float> *g2 = g1.clone();
			assert(g2->data() == g1.data());
			assert(g2->isAllocated());
			assert(!g2->isOwner());
			Grid<float> *g3 = g1.cloneAttribs();
			assert(g3->data() == 0);
			assert(!g3->isAllocated());
			assert(!g3->isOwner());
			Grid<float> *g4 = g1.cloneData();
			assert(g4->data() != 0);
			assert(g4->data() != g1.data());
			assert(g4->isAllocated());
			assert(g4->isOwner());
		}	
		{
			Grid2D<float> g1(1.0,2.0,10u,20u,true);
			Grid<float> *g2 = g1.clone();
			assert(g2->data() == g1.data());
			assert(g2->isAllocated());
			assert(!g2->isOwner());
			Grid<float> *g3 = g1.cloneAttribs();
			assert(g3->data() == 0);
			assert(!g3->isAllocated());
			assert(!g3->isOwner());
			Grid<float> *g4 = g1.cloneData();
			assert(g4->data() != 0);
			assert(g4->data() != g1.data());
			assert(g4->isAllocated());
			assert(g4->isOwner());
		}	
		{
			Grid1D<float> g1(1.0,10u,true);
			Grid<float> *g2 = g1.clone();
			assert(g2->data() == g1.data());
			assert(g2->isAllocated());
			assert(!g2->isOwner());
			Grid<float> *g3 = g1.cloneAttribs();
			assert(g3->data() == 0);
			assert(!g3->isAllocated());
			assert(!g3->isOwner());
			Grid<float> *g4 = g1.cloneData();
			assert(g4->data() != 0);
			assert(g4->data() != g1.data());
			assert(g4->isAllocated());
			assert(g4->isOwner());
		}	
		std::cout << "clone cloneAttribs cloneData : ok" << std::endl;
	}

	exit(EXIT_SUCCESS);
}

#endif
