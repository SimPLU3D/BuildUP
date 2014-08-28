#ifndef GEOMETRY_CUBOID_HPP
#define GEOMETRY_CUBOID_HPP

#include <rjmcmc/geometry/Rectangle_2.hpp>
#include <cmath>

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
    FT _rho;
    FT _theta;
    FT _h;

public:
    Cuboid():_rect(Rectangle_2<T>()),_h(0.) {}

    Cuboid(const Point_2 &p,const FT &r, const FT &t, const FT&f, const FT &g)
        :_rect(Rectangle_2<T>(p,Vector_2(r*std::cos(t),r*std::sin(t)),f)),_rho(r),_theta(t),_h(g) {}

    FT distance2cuboid(const Cuboid<T> & o) const
    {
        std::vector<FT> sqd;

        for(int i=0; i<4; i++)
        {
            Segment_2 seg(_rect.point(i),_rect.point(i+1));
            for(int j=0; j<4; j++)
                sqd.push_back( squared_distance_point2seg(o._rect.point(j),seg) );
        }

        FT result = sqd.front();
        for(typename std::vector<FT>::iterator it = sqd.begin(); it!=sqd.end(); ++it)
            if(*it<result)
                result = *it;

        return std::sqrt(result);
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

    inline void translate(double x,double y)
    {
        Vector_2 v(x,y);
        _rect.translate(v);
    }

    inline bool is_degenerate() const
    {
        return _rect.is_degenerate();
    }

    inline const Rectangle_2<T>& bottom() const
    {
        return _rect;
    }
    inline const Vector_2& normal() const
    {
        return _rect.normal();
    }
    inline const FT   ratio () const
    {
        return _rect.ratio();
    }
    inline const FT   rho() const
    {
        return _rho;
    }
    inline const FT   theta() const
    {
        return _theta;
    }
    inline const FT   h() const
    {
        return _h;
    }
    inline const FT   area() const
    {
        FT l1 = 2*sqrt(this->normal().squared_length());
        FT l2 = l1*this->ratio();
        return l1*l2;
    }


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

}; // class template Cuboid

} //namespace geometry


#endif
