
#include "utils/headers.hpp"
#include "grid.hpp"

Grid::Grid(double sizeX, unsigned int nPointsX, 
           double sizeY, unsigned int nPointsY, 
           double sizeZ, unsigned int nPointsZ) :
           m_sizeX(sizeX), m_nPointsX(nPointsX), m_sizeY(sizeY), m_nPointsY(nPointsY), m_sizeZ(sizeZ), m_nPointsZ(nPointsZ) {


    //TODO
}

Grid::~Grid() {
    //TODO free data
}


bool Grid::is3D() {
    return (m_nPointsZ != 0);
}

