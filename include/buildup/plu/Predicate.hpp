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
        double w = *it++;
        double l = *it++;
        double theta = *it++;
        double mx = w*0.5*std::cos(theta);
        double my = w*0.5*std::sin(theta);
        double r = l/w;
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
