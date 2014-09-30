#ifndef PREDICATE_HPP_INCLUDED
#define PREDICATE_HPP_INCLUDED

#include <gdal/ogrsf_frmts.h>
#include <cmath>
#include <vector>

class Predicate
{
    OGRPolygon* m_polygon;
public:
    inline Predicate(OGRPolygon* ply):m_polygon(ply) {}

    template<typename InputIterator>
    int operator()(InputIterator it) const
    {
        double cx = *it++;
        double cy = *it++;
        double rho = *it++;
        double theta = *it++;
        double r = *it++;
        double mx = rho*std::cos(theta);
        double my = rho*std::sin(theta);
        double nx = -r*my;
        double ny = r*mx;

        std::vector<OGRPoint> pts;
        pts.push_back(OGRPoint (cx-nx-mx,cy-ny-my));
        pts.push_back(OGRPoint (cx+nx-mx,cy+ny-my));
        pts.push_back(OGRPoint (cx+nx+mx,cy+ny+my));
        pts.push_back(OGRPoint (cx-nx+mx,cy-ny+my));
        for(int i=0;i<pts.size();++i)
            if(!pts[i].Within(m_polygon))
                return 0;
        return 1;
    }
};

#endif // PREDICATE_HPP_INCLUDED
