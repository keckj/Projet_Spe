
#include "log.hpp"
#include <cassert>

template <typename T>
System<T>::System(const std::map<std::string, Grid<T> *> &initialCond) :
	grids(0), grids_old(0), _dh(0.0)
{
	assert(!initialCond.empty());
	
	Grid<T> *firstGrid = initialCond.begin()->second;
	unsigned int width = firstGrid->width();
	unsigned int height = firstGrid->height();
	unsigned int length = firstGrid->length();

	_dh = firstGrid->dh();

	for(auto it = initialCond.begin(); it != initialCond.end(); it++) {
		Grid<T> *grid = it->second;

		if(width != grid->width() || height != grid->height() || length != grid->length()) {
			log_console->errorStream() << "Incoherent grid sizes in system !";
			exit(EXIT_FAILURE);
		}

		if(!grid->isAllocated()) {
			log_console->errorStream() << "Initial condition grid is NULL !";
			exit(EXIT_FAILURE);
		}

		width = grid->width();
		height = grid->height();
		length = grid->length();
	}

	grids_old = new std::map<std::string, Grid<T> *>;
	for(auto it = initialCond.begin(); it != initialCond.end(); it++) {
		(*grids_old)[it->first] = it->second->cloneData();			
	}

	grids = new std::map<std::string, Grid<T> *>;
	for(auto it = initialCond.begin(); it != initialCond.end(); it++) {
		(*grids)[it->first] = it->second->cloneAttribs();			
		(*grids)[it->first]->allocate();
	}
}

template <typename T>
System<T>::~System() {
	
	for(auto it = grids->begin(); it != grids->end(); it++) {
		(*grids)[it->first]->free();
		delete (*grids)[it->first];			
	}
	delete grids;
	grids = 0;
	
	for(auto it = grids_old->begin(); it != grids_old->end(); it++) {
		(*grids_old)[it->first]->free();
		delete (*grids_old)[it->first];			
	}
	delete grids_old;
	grids_old = 0;
}
