#ifndef PROCEDURAL_HPP_INCLUDED
#define PROCEDURAL_HPP_INCLUDED

#include <gdal/ogrsf_frmts.h>
#include <vector>

enum class SurfaceType
{
    Envelope,
    Footprint,
    Roof,
    Wall,
    Floor,
    Unit,
    Window,
    Door,
    Unknown

};


class Surface
{
protected:
    SurfaceType _type;
    OGRPolygon* _geom;
    double _length,_width,_height;
    std::vector<Surface*> _children;
public:
    Surface(SurfaceType t,OGRPolygon* g,double l,double w,double h)
    :_type(t),_geom(0),_length(l),_width(w),_height(h){ if(g) _geom = (OGRPolygon*)(g->clone());}

    ~Surface(){
        if(_geom)
            _geom->empty();
        for(size_t i=0;i<_children.size();++i)
            if(_children[i]) delete _children[i];
    }

    inline void addChild(Surface* s) {_children.push_back(s);}
    inline int getNumChildren() const {return _children.size();}
    inline bool hasChild() const {return !_children.empty();}
    inline Surface* getChild(int i) {return _children.at(i);}
    inline SurfaceType getType() const {return _type;}
    inline OGRPolygon* getGeom() const {return _geom;}
    inline double getLength() const {return _length;}
    inline double getHeight() const {return _height;}

    void getLeaves(std::vector<Surface*>& );
    void getSurfaceByType(SurfaceType, std::vector<Surface*>&);
};

class Unit: public Surface
{
    OGRPoint* _lowerLeft;
    double _dirX,_dirY;
public:
    Unit(OGRPolygon* ply,double length,double height
    ,OGRPoint* lowerLeft,double dirX,double dirY)
    : Surface(SurfaceType::Unit,ply,length,0.,height)
    ,_lowerLeft((OGRPoint*)(lowerLeft->clone())),_dirX(dirX),_dirY(dirY){}

    ~Unit(){_lowerLeft->empty();}

    void insertWindow(double l,double h);
    void insertDoor(double l,double h);
    OGRPolygon* scale(double sL,double sH);
};




class Floor: public Surface
{
    OGRPoint* _lowerLeft, *_upperLeft;
    double _dirX,_dirY;

public:
    Floor(OGRPolygon* ply, double length,double height
    ,OGRPoint* lowerLeft,OGRPoint* upperLeft,double dirX,double dirY)
    :Surface(SurfaceType::Floor,ply,length,0.,height)
    ,_lowerLeft((OGRPoint*)(lowerLeft->clone()))
    ,_upperLeft((OGRPoint*)(upperLeft->clone()))
    ,_dirX(dirX),_dirY(dirY)
    {}

    ~Floor(){_lowerLeft->empty();_upperLeft->empty();}

    void splitUnit(double lenUnit);
};



class Wall:public Surface
{
    OGRPoint* _lowerLeft, *_lowerRight;

public:
    Wall(OGRPolygon* ply, double length,double height, OGRPoint* lowerLeft,OGRPoint* lowerRight)
    :Surface(SurfaceType::Wall,ply,length,0.,height)
    ,_lowerLeft((OGRPoint*)(lowerLeft->clone()))
    ,_lowerRight((OGRPoint*)(lowerRight->clone()))
    {}

    ~Wall(){_lowerLeft->empty();_lowerRight->empty();}

    void splitFloor(double dh);

};



#endif // PROCEDURAL_HPP_INCLUDED
