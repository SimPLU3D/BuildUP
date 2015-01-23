#include "buildup/plu/Procedural.hpp"
#include <cmath>

void visit_leaves(Surface* node,std::vector<Surface*>& leaves)
{
    if(node->hasChild())
        for(int i=0;i<node->getNumChildren();++i)
            visit_leaves(node->getChild(i),leaves);
    else
        leaves.push_back(node);
}

void Surface::getLeaves(std::vector<Surface*>& leaves)
{
    visit_leaves(this,leaves);
}

void find_surface(Surface* node,SurfaceType type, std::vector<Surface*>& results )
{
    if(node->getType()==type)
        results.push_back(node);
    if(node->hasChild())
        for(int i=0;i<node->getNumChildren();++i)
            find_surface(node->getChild(i),type,results);
}

void Surface::getSurfaceByType(SurfaceType type, std::vector<Surface*>& results)
{
    find_surface(this,type,results);
}

void Unit::insertWindow(double l,double h)
{
    //center point of the unit
    OGRPoint c;
    _geom->Centroid(&c);
    c.setZ(c.getZ()+_lowerLeft->getZ()+_height/2);
    double cx = c.getX();
    double cy = c.getY();
    double cz = c.getZ();

    double dirL = std::sqrt(_dirX*_dirX + _dirY*_dirY);
    double dx = (l/2)*_dirX/dirL;
    double dy = (l/2)*_dirY/dirL;

    OGRLinearRing* ring = new OGRLinearRing;
    ring->addPoint(cx-dx,cy-dy,cz-h/2);
    ring->addPoint(cx+dx,cy+dy,cz-h/2);
    ring->addPoint(cx+dx,cy+dy,cz+h/2);
    ring->addPoint(cx-dx,cy-dy,cz+h/2);
    ring->addPoint(cx-dx,cy-dy,cz-h/2);


    OGRPolygon winPly;
    winPly.addRingDirectly(ring);
    this->addChild(new Surface(SurfaceType::Window,&winPly,l,0.,h));

    OGRPolygon* restPly = (OGRPolygon*) (_geom->clone());
    restPly->addRing(winPly.getExteriorRing());
    this->addChild(new Surface(SurfaceType::Unknown,restPly,_length,0.,_height));
}


void Unit::insertDoor(double l,double h)
{
    double dirL = std::sqrt(_dirX*_dirX + _dirY*_dirY);
    double vx = _dirX/dirL;
    double vy = _dirY/dirL;

    double d1 = (_length-l)*0.5;
    double d2 = (_length+l)*0.5;

    double ox = _lowerLeft->getX();
    double oy = _lowerLeft->getY();
    double oz = _lowerLeft->getZ();

    OGRLinearRing* ring = new OGRLinearRing;
    ring->addPoint(ox+d1*vx,oy+d1*vy,oz);
    ring->addPoint(ox+d1*vx,oy+d1*vy,oz+h);
    ring->addPoint(ox+d2*vx,oy+d2*vy,oz+h);
    ring->addPoint(ox+d2*vx,oy+d2*vy,oz);
    ring->addPoint(ox+d1*vx,oy+d1*vy,oz);

    OGRPolygon doorPly;
    doorPly.addRingDirectly(ring);
    this->addChild(new Surface(SurfaceType::Door,&doorPly,l,0.,h));

    OGRPolygon* restPly = (OGRPolygon*) (_geom->clone());
    restPly->addRing(doorPly.getExteriorRing());
    this->addChild(new Surface(SurfaceType::Unknown,restPly,_length,0.,_height));

}

void Floor::splitUnit(double lenUnit)
{
    int n = std::ceil(_length/lenUnit);

    double dirL = std::sqrt(_dirX*_dirX + _dirY*_dirY);
    double dx = lenUnit*_dirX/dirL;
    double dy = lenUnit*_dirY/dirL;

    OGRPoint* pt1 = (OGRPoint*)(_upperLeft->clone());
    OGRPoint* pt2 = (OGRPoint*)(_lowerLeft->clone());
    for(int i=0;i<n-1;++i)
    {
        OGRPoint* pt3 = new OGRPoint(pt2->getX()+dx,pt2->getY()+dy,pt2->getZ()); //lowerRight
        OGRPoint* pt4 = new OGRPoint(pt1->getX()+dx,pt1->getY()+dy,pt1->getZ()); //upperRight

        OGRLinearRing ring;
        ring.addPoint(pt1);
        ring.addPoint(pt2);
        ring.addPoint(pt3);
        ring.addPoint(pt4);
        ring.addPoint(pt1);

        OGRPolygon unitPly;
        unitPly.addRing(&ring);

        this->addChild(new Unit(&unitPly,lenUnit,_height,pt2,_dirX,_dirY));

        pt1->empty();
        pt2->empty();
        pt1 = pt4;
        pt2 = pt3;
    }

    double DX = _length *_dirX/dirL;
    double DY = _length *_dirY/dirL;
    OGRPoint* pt3 = new OGRPoint(_lowerLeft->getX()+DX,_lowerLeft->getY()+DY,_lowerLeft->getZ()); //lowerRight
    OGRPoint* pt4 = new OGRPoint(_upperLeft->getX()+DX,_upperLeft->getY()+DY,_upperLeft->getZ()); //upperRight

    OGRLinearRing ring;
    ring.addPoint(pt1);
    ring.addPoint(pt2);
    ring.addPoint(pt3);
    ring.addPoint(pt4);
    ring.addPoint(pt1);

    OGRPolygon unitPly;
    unitPly.addRing(&ring);

    this->addChild(new Unit(&unitPly,(_length-lenUnit*(n-1)),_height,pt2,_dirX,_dirY));

    pt1->empty();
    pt2->empty();

}

void Wall::splitFloor(double dh)
{
    //number of split
    int n = (int)(std::round(_height/dh))-1;
    double dirX = _lowerRight->getX()-_lowerLeft->getX();
    double dirY = _lowerRight->getY()-_lowerLeft->getY();

    OGRPoint* pt1 = (OGRPoint*)(_lowerLeft->clone());
    OGRPoint* pt2 = (OGRPoint*)(_lowerRight->clone());
    for(int i=0;i<n;++i)
    {
        OGRPoint* pt3 = new OGRPoint(pt2->getX(),pt2->getY(),pt2->getZ()+dh);
        OGRPoint* pt4 = new OGRPoint(pt1->getX(),pt1->getY(),pt1->getZ()+dh);

        OGRLinearRing ring;
        ring.addPoint(pt1);
        ring.addPoint(pt2);
        ring.addPoint(pt3);
        ring.addPoint(pt4);
        ring.addPoint(pt1);

        OGRPolygon floorPly;
        floorPly.addRing(&ring);

        this->addChild(new Floor(&floorPly,_length,dh,pt1,pt4,dirX,dirY));

        pt1->empty();
        pt2->empty();
        pt1 = pt4;
        pt2 = pt3;
    }

    OGRPoint* pt3 = new OGRPoint(_lowerRight->getX(),_lowerRight->getY(),_lowerRight->getZ()+_height);
    OGRPoint* pt4 = new OGRPoint(_lowerLeft->getX(),_lowerLeft->getY(),_lowerLeft->getZ()+_height);

    OGRLinearRing ring;
    ring.addPoint(pt1);
    ring.addPoint(pt2);
    ring.addPoint(pt3);
    ring.addPoint(pt4);
    ring.addPoint(pt1);

    OGRPolygon floorPly;
    floorPly.addRing(&ring);

    this->addChild(new Floor(&floorPly,_length,(_height-dh*n),pt1,pt4,dirX,dirY));

    pt1->empty();
    pt2->empty();
    pt3->empty();
    pt4->empty();
}

