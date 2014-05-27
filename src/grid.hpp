
#ifndef GRID_HPP
#define GRID_HPP

class Grid {

    public:
        Grid(double sizeX, unsigned int nPointsX, 
             double sizeY, unsigned int nPointsY, 
             double sizeZ = 0, unsigned int nPointsZ = 0);
        ~Grid();

        bool is3D();

    private:
        double m_sizeX;
        unsigned int m_nPointsX;
        double m_sizeY;
        unsigned int m_nPointsY;
        double m_sizeZ;
        unsigned int m_nPointsZ;
        // TODO buffers
};

#endif
