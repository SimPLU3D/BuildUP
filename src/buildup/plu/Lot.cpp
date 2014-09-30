#include "buildup/plu/Lot.hpp"
#include <boost/lexical_cast.hpp>

#ifndef PI
#define PI 3.14159265
#endif

void Lot::translate(double x0,double y0)
{
    _translatedX += x0;
    _translatedY += y0;
    //1) lot polygon
    //2) lot envelope
    //3) border segments
    int n=_polygon->getExteriorRing()->getNumPoints();
    for(int i=0; i<n; ++i)
    {
        double x = _polygon->getExteriorRing()->getX(i)+x0;
        double y = _polygon->getExteriorRing()->getY(i)+y0;
        _polygon->getExteriorRing()->setPoint(i,x,y);
    }

    _polygon->getEnvelope(&_box);

    Vector_2 pt0(x0,y0);
    std::map< int,BorderSeg>::iterator it;
    for(it=_borderSegs.begin(); it!=_borderSegs.end(); ++it)
    {
        Segment_2 seg(it->second.getGeom().source()+pt0,it->second.getGeom().target()+pt0);
        it->second.setGeom(seg);
    }
}




void Lot::set_isRectLike(bool isRectLike)
{
    if(isRectLike)
    {
        std::map<int,BorderSeg>::iterator it;
        for(it=_borderSegs.begin(); it!=_borderSegs.end(); ++it)
            if(it->second.getType()==BorderType::Front)
            {
                //reference to the first front border segment
                _idRefSeg = it->second.getID();
                Segment_2 refSeg = _borderSegs.find(_idRefSeg)->second.getGeom();
                Vector_2 vec = refSeg.target()-refSeg.source();
                _thetaRefSeg = std::atan2(vec.y(),vec.x()); //(-pi,pi]
                if(_thetaRefSeg<0)
                    _thetaRefSeg += PI;
                return;
            }
    }
}

void Lot::set_name_borders()
{
    std::map<std::string, std::vector<Border*> > typeName_borders;

    std::map< int,Border>::iterator it;
    for(it=_borders.begin(); it!=_borders.end(); ++it)
        typeName_borders[it->second.getTypeName()].push_back(&(it->second));

    std::map<std::string, std::vector<Border*> >::iterator iter;
    for(iter=typeName_borders.begin(); iter!=typeName_borders.end(); ++iter)
    {
        int n = iter->second.size();
        if(n==1)
            _name_borders[iter->first] = (iter->second).at(0);

        else
        {
            for(int i=0; i<n; ++i)
            {
                std::string name = iter->first;
                name += boost::lexical_cast<std::string>(i+1);
                _name_borders[name] = (iter->second).at(i);
            }

        }
    }
}



double Lot::refTheta(double x,double y)
{
    if(_idRefSeg>-1)
        return _thetaRefSeg;

    Point_2 center(x,y);

    std::map< int,BorderSeg >::iterator iter = _borderSegs.begin();
    double d2min = iter->second.squared_dist(center);
    int idNearest = iter->first;
    iter++;
    for(; iter != _borderSegs.end(); ++iter)
    {
        double d2 = iter->second.squared_dist(center);
        if(d2<d2min)
        {
            d2min = d2;
            idNearest = iter->first;
        }
    }

    Segment_2& s = _borderSegs.find(idNearest)->second.getGeom();
    Vector_2 vec = s.target() - s.source();
    double theta = std::atan2(vec.y(),vec.x());
    if(theta<0)
        theta += PI;
    return theta;
}


double Lot::refTheta_front(double x,double y)
{
    if(_idRefSeg>-1)
        return _thetaRefSeg;


    std::map< int,BorderSeg >::iterator iter = _borderSegs.begin();
    while(iter!=_borderSegs.end() && iter->second.getType()!=BorderType::Front)
        iter++;

    Point_2 center(x,y);
    double d2min = iter->second.squared_dist(center);
    int idNearest = iter->first;
    iter++;

    for(; iter != _borderSegs.end(); ++iter)
    {
        if(iter->second.getType()!=BorderType::Front)
            continue;

        double d2 = iter->second.squared_dist(center);
        if(d2<=d2min)
        {
            d2min = d2;
            idNearest = iter->first;
        }
    }

    Segment_2& s = _borderSegs.find(idNearest)->second.getGeom();
    Vector_2 vec = s.target() - s.source();
    double theta = std::atan2(vec.y(),vec.x());
    if(theta<0)
        theta += PI;
    return theta;
}


void Lot::extractBorderSegs(std::map< int,BorderSeg >& borderSegs)
{
    int idSeg = -1;
    OGRLinearRing* ring = _polygon->getExteriorRing();
    for(int i=0; i<ring->getNumPoints()-1; ++i)
    {
        Point_2 pt1(ring->getX(i),ring->getY(i));
        Point_2 pt2(ring->getX(i+1),ring->getY(i+1));
        ++idSeg;
        Segment_2 s(pt1,pt2);
        borderSegs.insert(std::make_pair(idSeg,BorderSeg(_id,idSeg,s)));
    }

    for(int j=0; j<_polygon->getNumInteriorRings(); ++j)
    {
        ring = _polygon->getInteriorRing(j);
        for(int i=0; i<ring->getNumPoints()-1; ++i)
        {
            Point_2 pt1(ring->getX(i),ring->getY(i));
            Point_2 pt2(ring->getX(i+1),ring->getY(i+1));
            ++idSeg;
            Segment_2 s(pt1,pt2);
            borderSegs.insert(std::make_pair(idSeg,BorderSeg(_id,idSeg,s)));

        }
    }
}
