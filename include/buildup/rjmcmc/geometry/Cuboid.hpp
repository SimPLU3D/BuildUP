#ifndef GEOMETRY_CUBOID_HPP
#define GEOMETRY_CUBOID_HPP

#include "rjmcmc/geometry/Rectangle_2.hpp"
#include <cmath>
#include <vector>
#include <map>
#include <iostream>
//#include "buildup/viewer/osg.hpp"
#ifndef PI
#define PI 3.14159265
#endif

enum class AdjacentType
{
    edge2edge,
    edge2face,
    face2edge,
    face2face
};

namespace geometry
{

template<class T>
class Cuboid
{
private:
    typedef typename T::Point_2               Point_2;
    typedef typename T::Vector_2              Vector_2;
    typedef typename T::Segment_2             Segment_2;
    typedef typename T::FT                    FT;

    Rectangle_2<T> _rect;
    FT _width;
    FT _length;
    FT _theta;
    FT _h;

public:
    Cuboid():_rect(Rectangle_2<T>()),_h(0.) {}

    Cuboid(const Point_2 &p,const FT &w, const FT &l, const FT&t, const FT &h)
        :_rect(Rectangle_2<T>(p,Vector_2(w*0.5*std::cos(t),w*0.5*std::sin(t)),l/w))
        ,_width(w),_length(l),_theta(t),_h(h) {}

    FT distance2cuboid(const Cuboid<T> & o) const
    {

        std::vector<FT> sqd;

        for(int i=0; i<4; i++)
            for(int j=0; j<4; j++)
            {
                //squared distance from point to edge (this to other)
                sqd.push_back( squared_distance_point2seg(_rect.point(i),o._rect.segment(j)) );

                //squared distance from point to edge (other to this)
                sqd.push_back( squared_distance_point2seg(o._rect.point(i),_rect.segment(j)) );
            }

        FT sqd_min = sqd.front();
        for(typename std::vector<FT>::iterator it = sqd.begin(); it!=sqd.end(); ++it)
            if(*it<sqd_min)
                sqd_min = *it;

        return std::sqrt(sqd_min);

    }

    FT distance2cuboid(const Cuboid<T> & o, double lengthHasWindow, double& hasWindow) const
    {
        std::vector<FT> sqd1,sqd2;
        std::map<int,int> pos1,pos2;
        int n = 4;

        for(int i=0; i<n; i++)
            for(int j=0; j<n; j++)
            {
                //squared distance from point to edge (this to other)
                sqd1.push_back( squared_distance_point2seg(_rect.point(i),o._rect.segment(j),pos1[i*n+j]) );

                //squared distance from point to edge (other to this)
                sqd2.push_back( squared_distance_point2seg(o._rect.point(i),_rect.segment(j),pos2[i*n+j]) );
            }


        FT min1 = sqd1.front(), min2 = sqd2.front();
        int idMin1 = 0, idMin2 = 0;
        for( int k = 1;k<n*n; ++k)
        {
            if(sqd1[k]<min1)
            {
                min1 = sqd1[k];
                idMin1 = k;
            }
            if(sqd2[k]<min2)
            {
                min2 = sqd2[k];
                idMin2 = k;
            }
        }

        FT sqd_min;
        int id1,id2,pos;

        if(min1<=min2)
        {
            sqd_min = min1;
            id1 = idMin1/n;
            id2 = idMin1%n;
            pos = pos1[idMin1];
        }
        else
        {
            sqd_min = min2;
            id2 = idMin2/n;
            id1 = idMin2%n;
            pos = pos2[idMin2];
        }

        AdjacentType type;
        if(pos==0) //perpendicular foot is not on the surface of the opposite building
            type = AdjacentType::edge2edge;

        else
        {   //perpendicular foot is on the surface of the opposite building
            //need to check angle between the two buildings
            double angle = std::abs ( 90 -  (180/PI) * std::abs(_theta - o.theta())  );
            if(angle<=20 || angle>=70)
                type = AdjacentType::face2face;
            else if (min1<=min2)
                type = AdjacentType::edge2face;
            else
                type = AdjacentType::face2edge;
        }

//        if(type!=AdjacentType::face2face)
//        {
//            std::cout<<"not face2face: "<<(int)type<<"\n";
//            io::display((*this),o);
//        }



        hasWindow = isFacingWithWindow(o,type,id1,id2,lengthHasWindow);
//        std::cout<<id1<<", "<<id2<<", "<<"type:"<<(int)type<<",hasWindow:"<<hasWindow<<"\n";

        return std::sqrt(sqd_min);
    }

    FT distance2line(const Segment_2& s) const
    {
        FT dMin = distance_point2line(_rect.point(0),s.source(),s.target());

        for(int i=1; i<4; i++)
        {
            double d = distance_point2line(_rect.point(i),s.source(),s.target());
            if(d<dMin)
                dMin=d;
        }

        return dMin;
    }

    //idEdge is the edge facing to the line segment
    FT distance2line(const Segment_2& s,int& idEdge) const
    {
        FT dMin = distance_point2line(_rect.point(0),s.source(),s.target());
        int idPoint = 0;

        for(int i=1; i<4; i++)
        {
            double d = distance_point2line(_rect.point(i),s.source(),s.target());
            if(d<dMin)
            {
                dMin=d;
                idPoint = i;
            }
        }

        Segment_2 edge = _rect.segment(idPoint);

        if(angle_radian(edge,s)<= PI*0.25)
            idEdge = idPoint;
        else
            idEdge = idPoint-1;


        return dMin;
    }

    inline bool is_degenerate() const{return (_h && _rect.is_degenerate());}
    inline const Rectangle_2<T>& bottom()   const{return _rect;}
    inline const Vector_2& normal()         const{return _rect.normal();}
 //   inline const FT   ratio ()  const{return _rect.ratio();}
    inline const FT   width()   const{return _width;}
    inline const FT   length()  const{return _length;}
    inline const FT   theta()   const{return _theta;}
    inline const FT   h()       const{return _h;}
    inline const FT   area()    const{return _width*_length;}
    inline const FT   volume()  const{return _width*_length*_h;}


private:
    inline FT squared_distance_point2seg(const Point_2& p, const Segment_2& seg) const
    {

        //A,B: endpoints of segment, C: perpendicular foot from point to lineAB
        //r=product(AP,AB)/ |AB||AB|, vec_AC = r*vec_AB
        //if r<=0 dist=|AP|
        //if r>=1 dist=|BP|
        //else dist=|CP|
        FT x = p.x(),y = p.y();
        FT x1 = seg.source().x(),y1 = seg.source().y();
        FT x2 = seg.target().x(),y2 = seg.target().y();

        FT dot = (x-x1)*(x2-x1) + (y-y1)*(y2-y1);
        if(dot<=0)
            return (x-x1)*(x-x1) + (y-y1)*(y-y1);

        FT sqLen =  (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1);
        if(dot>=sqLen)
            return (x-x2)*(x-x2) + (y-y2)*(y-y2);

        FT cx = x1+(x2-x1)*dot/sqLen;
        FT cy = y1+(y2-y1)*dot/sqLen;
        return (x-cx)*(x-cx)+(y-cy)*(y-cy);
    }

    inline FT squared_distance_point2seg(const Point_2& p, const Segment_2& seg, int& pos) const
    {

        //A,B: endpoints of segment, C: perpendicular foot from point to lineAB
        //r=product(AP,AB)/ |AB||AB|, vec_AC = r*vec_AB
        //if r<=0 dist=|AP| pos = 0
        //if r>=1 dist=|BP| pos = 0
        //else dist=|CP| pos = 1
        FT x = p.x(),y = p.y();
        FT x1 = seg.source().x(),y1 = seg.source().y();
        FT x2 = seg.target().x(),y2 = seg.target().y();

        FT dot = (x-x1)*(x2-x1) + (y-y1)*(y2-y1);
        if(dot<=0)
        {
            pos = 0;
            return (x-x1)*(x-x1) + (y-y1)*(y-y1);
        }

        FT sqLen =  (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1);
        if(dot>=sqLen)
        {
            pos = 0;
            return (x-x2)*(x-x2) + (y-y2)*(y-y2);
        }

        FT cx = x1+(x2-x1)*dot/sqLen;
        FT cy = y1+(y2-y1)*dot/sqLen;
        pos = 1;
        return (x-cx)*(x-cx)+(y-cy)*(y-cy);
    }


    inline FT distance_point2line(const Point_2& p0,const Point_2& p1, const Point_2& p2) const
    {
        //distance from p0 to line(p1,p2)
        //two point form -> general form: (y-y1)/(y2-y1) = (x-x1)/(x2-x1) -> ax+by+c=0
        // a = y2-y1; b=x1-x2; c=-by1-ax1; d=|ax0+by0+c|/sqrt(a²+b²)
        FT a,b,c;
        a=p2.y()-p1.y();
        b=p1.x()-p2.x();
        c=-b*p1.y()-a*p1.x();

        return std::abs(a*p0.x()+b*p0.y()+c)/sqrt(a*a+b*b);
    }

    //result [0,pi/2]
    inline FT angle_radian(const Segment_2& seg1, const Segment_2& seg2) const
    {
        Vector_2 v1 = seg1.target()-seg1.source();
        Vector_2 v2 = seg2.target()-seg2.source();
        return std::acos(std::abs(v1*v2) / std::sqrt(v1*v1*v2*v2));
    }

    inline int geq(double a,double b,double epsilon = 0.001) const
    {
        return (a>b || std::abs(a-b)<epsilon);
    }

    inline FT isFacingWithWindow(const Cuboid<T> & o,AdjacentType type, int id1, int id2,double lengthHasWindow) const
    {
        //determine if haswindow on the opposing facades

        Point_2 p1 = _rect.point(id1);
        Point_2 p1a = _rect.point(id1-1);
        Point_2 p1b = _rect.point(id1+1);

        Point_2 p2 = _rect.point(id2);
        Point_2 p2a = o._rect.point(id2-1);
        Point_2 p2b = o._rect.point(id2+1);

        double sqd11 = (p1-p1a).squared_length();
        double sqd12 = (p1-p1b).squared_length();
        double sqd21 = (p2-p2a).squared_length();
        double sqd22 = (p2-p2b).squared_length();

        double a = lengthHasWindow * lengthHasWindow;
        FT hasWindow = 0.;
        switch (type){
            case AdjacentType::edge2edge:
                if( (geq(sqd11,a)||geq(sqd12,a)) && (geq(sqd21,a)||geq(sqd22,a)) )
                    hasWindow = 1.;
                break;
            case  AdjacentType::edge2face:
                if( (geq(sqd11,a)||geq(sqd12,a)) && geq(sqd22,a))
                    hasWindow = 1.;
                break;
            case AdjacentType::face2edge:
                if( geq(sqd12,a) && (geq(sqd21,a)||geq(sqd22,a)) )
                    hasWindow = 1.;
                break;
            case AdjacentType::face2face:
                if( geq(sqd12,a) && geq(sqd22,a) )
                    hasWindow = 1.;
                break;
            default:
                break;
        }
        return hasWindow;

    }
}; // class template Cuboid

} //namespace geometry


#endif
