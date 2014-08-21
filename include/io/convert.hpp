#ifndef CONVERT_HPP_INCLUDED
#define CONVERT_HPP_INCLUDED
#include "sfcgal.hpp"
#include <gdal/ogrsf_frmts.h>

 namespace io
{

    void SFCGAL2OGR(Polygon& ply, OGRPolygon* ogrPly)
    {

        for(size_t j=0;j<ply.numRings();++j)
        {
            OGRLinearRing ogrRing;
            LineString & ring = ply.ringN(j);
            for(size_t i=0;i<ring.numPoints();++i)
            {
                double x = CGAL::to_double(ring.pointN(i).x());
                double y = CGAL::to_double(ring.pointN(i).y());
                ogrRing.addPoint(x,y,0);
            }
            ogrPly->addRing(&ogrRing);
        }
    }

    void OGR2SFCGAL(Polygon& ply, OGRPolygon* ogrPly)
    {
        ogrPly->closeRings();

        {
            OGRLinearRing* ogrRing = ogrPly->getExteriorRing();
            int n=ogrRing->getNumPoints();
            std::vector<Point> points;
            for(int i=0;i<n;++i)
                points.push_back(Point(ogrRing->getX(i),ogrRing->getY(i),ogrRing->getY(i)));
            ply.addRing(LineString(points));
        }


        for(int j=0;j<ogrPly->getNumInteriorRings();++j)
        {
            OGRLinearRing* ogrRing = ogrPly->getInteriorRing(j);
            int n=ogrRing->getNumPoints();
            std::vector<Point> points;
            for(int i=0;i<n;++i)
                points.push_back(Point(ogrRing->getX(i),ogrRing->getY(i),ogrRing->getY(i)));
            ply.addRing(LineString(points));
        }

    }

    Polygon OGR2SFCGAL(OGRPolygon* ogrPly)
    {
        std::vector< LineString > rings;

        {
            OGRLinearRing* ogrRing = ogrPly->getExteriorRing();
            std::vector< Point > points;
            for(int i=0;i < ogrRing->getNumPoints();++i)
                points.push_back(Point(ogrRing->getX(i),ogrRing->getY(i)));
            rings.push_back(LineString(points));
        }

        for(int j=0; j< ogrPly->getNumInteriorRings(); ++j)
        {
            OGRLinearRing* ogrRing = ogrPly->getInteriorRing(j);
            std::vector< Point > points;
            for(int i=0;i < ogrRing->getNumPoints();++i)
                points.push_back(Point(ogrRing->getX(i),ogrRing->getY(i)));
            rings.push_back(LineString(points));
        }

        return Polygon(rings);
    }

}//namespace io

#endif // CONVERT_HPP_INCLUDED
