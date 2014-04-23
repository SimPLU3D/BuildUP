#ifndef DB_HPP_INCLUDED
#define DB_HPP_INCLUDED

#include <pqxx/pqxx>
#include <geos/geom.h>
#include <geos/io/WKBReader.h>
#include <iostream>
#include <map>
#include <vector>

#include "plu/Lot.hpp"
#include "plu/Road.hpp"


namespace io{

    void pq_import_lots(std::map<int,Lot>& lots)
    {
        pqxx::connection c("host=localhost dbname=test22 user=postgres");
        pqxx::work w(c);
        pqxx::result r=w.exec("SELECT geom FROM parcelle;");
        w.commit();

        std::cout <<r.size()<<std::endl;
        int n=r.size();
        for(int i=0;i<n;++i)
        {
            std::istringstream istr(r[i][0].c_str());
            geos::geom::GeometryFactory factory;
            geos::io::WKBReader wkbr(factory);
            geos::geom::Polygon* ply = dynamic_cast<geos::geom::Polygon*>(wkbr.readHEX(istr));

            OGRPolygon* ogrPly = new OGRPolygon;

            {
                OGRLinearRing ogrRing;
                const geos::geom::LineString* ring = ply->getExteriorRing();
                for(int j=0;j<ring->getNumPoints();++j)
                    ogrRing.addPoint(ring->getPointN(j)->getX(),ring->getPointN(j)->getY());
                ogrPly->addRing(&ogrRing);
            }



            if(ply->getNumInteriorRing())
            {
                for(int k=0;k<ply->getNumInteriorRing();++k)
                {
                    OGRLinearRing ogrRing;
                    const geos::geom::LineString* ring = ply->getExteriorRing();
                    for(int j=0;j<ring->getNumPoints();++j)
                        ogrRing.addPoint(ring->getPointN(j)->getX(),ring->getPointN(j)->getY());
                    ogrPly->addRing(&ogrRing);
                }
            }

            ogrPly->closeRings();
            lots.insert(std::pair<int,Lot>(i,Lot(i,ogrPly)));

        }
    }

    void pq_import_roads()
    {
        pqxx::connection c("host=localhost dbname=test22 user=postgres");
        pqxx::work w(c);
        pqxx::result r=w.exec("SELECT geom FROM route;");
        w.commit();
    }
}

#endif // DB_HPP_INCLUDED
