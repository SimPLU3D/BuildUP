#include "buildup/plu/Building.hpp"


OGRMultiPolygon* Building::extrude() const
{
    OGRMultiPolygon* block = new OGRMultiPolygon;

    //extrude roof
    OGRPolygon roof;
    {
        roof.addRing(_footprint->getExteriorRing());
        OGRLinearRing* ring = roof.getExteriorRing();
        for(int i=0; i<ring->getNumPoints(); i++)
            ring->setPoint(i,ring->getX(i),ring->getY(i),_h);
    }

    if(int n = _footprint->getNumInteriorRings())
    {
        for (int j=0; j<n; j++)
        {
            roof.addRing(_footprint->getInteriorRing(j));
            OGRLinearRing* ring = roof.getInteriorRing(j);
            for(int i=0; i<ring->getNumPoints(); i++)
                ring->setPoint(i,ring->getX(i),ring->getY(i),_h);
        }
    }

    block->addGeometry(&roof);

    //extrude exter walls
    OGRLinearRing* ringEx = _footprint->getExteriorRing();
    for(int i=0; i<ringEx->getNumPoints()-1; i++)
    {
        OGRPolygon wall;
        OGRLinearRing ring;
        ring.addPoint(ringEx->getX(i),ringEx->getY(i),0);
        ring.addPoint(ringEx->getX(i+1),ringEx->getY(i+1),0);
        ring.addPoint(ringEx->getX(i+1),ringEx->getY(i+1),_h);
        ring.addPoint(ringEx->getX(i),ringEx->getY(i),_h);
        ring.addPoint(ringEx->getX(i),ringEx->getY(i),0);
        wall.addRing(&ring);
        block->addGeometry(&wall);
    }

    //extrude inner walls if exist
    if(int n = _footprint->getNumInteriorRings())
    {
        for (int i=0; i<n; i++)
        {
            OGRLinearRing* ringIn = _footprint->getInteriorRing(i);
            for(int j=0; j<ringIn->getNumPoints()-1; j++)
            {
                OGRPolygon wall;
                OGRLinearRing ring;
                ring.addPoint(ringIn->getX(j),ringIn->getY(j),0);
                ring.addPoint(ringIn->getX(j+1),ringIn->getY(j+1),0);
                ring.addPoint(ringIn->getX(j+1),ringIn->getY(j+1),_h);
                ring.addPoint(ringIn->getX(j),ringIn->getY(j),_h);
                ring.addPoint(ringIn->getX(j),ringIn->getY(j),0);
                wall.addRing(&ring);
                block->addGeometry(&wall);
            }
        }
    }


    return block;

}
