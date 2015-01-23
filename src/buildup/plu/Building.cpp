#include "buildup/plu/Building.hpp"
#include <iostream>
#include <cmath>

OGRMultiPolygon* Building::extrude_box() const
{
    OGRMultiPolygon* block = new OGRMultiPolygon;

    //extrude roof
    OGRPolygon roof;
    {
        roof.addRing(_footprint->getExteriorRing());
        OGRLinearRing* ring = roof.getExteriorRing();
        for(int i=0; i<ring->getNumPoints(); i++)
            ring->setPoint(i,ring->getX(i),ring->getY(i),_height);
    }

    if(int n = _footprint->getNumInteriorRings())
    {
        for (int j=0; j<n; j++)
        {
            roof.addRing(_footprint->getInteriorRing(j));
            OGRLinearRing* ring = roof.getInteriorRing(j);
            for(int i=0; i<ring->getNumPoints(); i++)
                ring->setPoint(i,ring->getX(i),ring->getY(i),_height);
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
        ring.addPoint(ringEx->getX(i+1),ringEx->getY(i+1),_height);
        ring.addPoint(ringEx->getX(i),ringEx->getY(i),_height);
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
                ring.addPoint(ringIn->getX(j+1),ringIn->getY(j+1),_height);
                ring.addPoint(ringIn->getX(j),ringIn->getY(j),_height);
                ring.addPoint(ringIn->getX(j),ringIn->getY(j),0);
                wall.addRing(&ring);
                block->addGeometry(&wall);
            }
        }
    }


    return block;

}


void adjust_winding_ccw(OGRLinearRing* ring)
{
    OGRLinearRing* ring1 = (OGRLinearRing*)(ring->clone());
    int n = ring->getNumPoints();
    for(int i=0,j=n-1;i<n;++i,--j)
        ring->setPoint(i,ring1->getX(j),ring1->getY(j),ring1->getZ(j));
    ring1->empty();
}

Surface* Building::extrude_envelope() const
{
    Surface* envelope = new Surface(SurfaceType::Envelope,0,_length,_width,_height);

    OGRLinearRing* ringFt = _footprint->getExteriorRing();
    if(ringFt->isClockwise())
         adjust_winding_ccw(ringFt);

    envelope->addChild(new Surface(SurfaceType::Footprint,_footprint,_length,_width,0.));
    //extrude roof
    OGRLinearRing* ringRoof = new OGRLinearRing;
    for(int i=0; i<ringFt->getNumPoints(); i++)
        ringRoof->addPoint(ringFt->getX(i),ringFt->getY(i),_height);

    OGRPolygon plyRoof;
    plyRoof.addRingDirectly(ringRoof);

    envelope->addChild(new Surface(SurfaceType::Roof,&plyRoof,_length,_width,0.));

    //extrude walls
    for(int i=0; i<ringFt->getNumPoints()-1; i++)
    {
        OGRLinearRing* ringWall = new OGRLinearRing;
        ringWall->addPoint(ringFt->getX(i),ringFt->getY(i),0);
        ringWall->addPoint(ringFt->getX(i+1),ringFt->getY(i+1),0);
        ringWall->addPoint(ringFt->getX(i+1),ringFt->getY(i+1),_height);
        ringWall->addPoint(ringFt->getX(i),ringFt->getY(i),_height);
        ringWall->addPoint(ringFt->getX(i),ringFt->getY(i),0);

        OGRPolygon plyWall;
        plyWall.addRingDirectly(ringWall);

        OGRPoint pt1(ringFt->getX(i),ringFt->getY(i),0);
        OGRPoint pt2(ringFt->getX(i+1),ringFt->getY(i+1),0);

        envelope->addChild(new Wall(&plyWall,pt1.Distance(&pt2),_height,&pt1,&pt2));

    }

    return envelope;
}


Surface* Building::extrude_lod3(double hFloor, double lenHasWin, double lenUnit) const
{
    Surface* bldg = extrude_envelope();
    for(int i=0;i<bldg->getNumChildren();++i)
    {
        if(bldg->getChild(i)->getType()==SurfaceType::Wall)
        {
            Wall* wall = (Wall*)(bldg->getChild(i));
            wall->splitFloor(hFloor);
            double lWall = wall->getLength();
            if(lWall>lenHasWin || std::abs(lWall-lenHasWin)<0.001)
            {
                for(int j=0;j<wall->getNumChildren();++j)
                {
                    Floor* floor = (Floor*)(wall->getChild(j));
                    floor->splitUnit(lenUnit);
                    for(int k=0;k<floor->getNumChildren();++k)
                    {
                        Unit* unit = (Unit*)(floor->getChild(k));
                        if(j==0 && k%5 == 0 && unit->getLength()>3)
                        {
                            unit->insertDoor(unit->getLength()/3,unit->getHeight()*2/3);
                            continue;
                        }

                        if(unit->getLength()>1)
                            unit->insertWindow(unit->getLength()/3,unit->getHeight()/3);
                    }
                }
            }

        }
    }
    return bldg;
}
