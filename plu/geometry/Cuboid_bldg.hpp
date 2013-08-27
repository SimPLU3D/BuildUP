#ifndef GEOMETRY_CUBOID_BLDG_HPP
#define GEOMETRY_CUBOID_BLDG_HPP

#include "geometry/Rectangle_2.hpp"
#include <math.h>
namespace geometry {

template<class T>
class Cuboid_bldg
{
public:
    typedef typename T::Point_2               Point_2;
    typedef typename T::Vector_2              Vector_2;
    typedef typename T::FT                    FT;

//private:
    typedef typename T::RT                    RT;
    typedef typename T::Line_2                Line_2;
    typedef typename T::Segment_2             Segment_2;
    typedef typename T::Iso_rectangle_2       Iso_rectangle_2;


    Rectangle_2<T> rect_2;
    FT h;

public:
    Cuboid_bldg():rect_2(Rectangle_2<T>()),h(0.){}

    Cuboid_bldg(const Point_2 &p, const Vector_2 &v, const FT &f, const FT &g): rect_2(Rectangle_2<T>(p,v,f)),h(g){}

    Cuboid_bldg(const Point_2 &p0, const Point_2 &p1, const Point_2 &p2, const FT &g): rect_2(Rectangle_2<T>(p0,p1,p2)),h(g){}

    Cuboid_bldg(const Segment_2 &s, const FT &g): rect_2(Rectangle_2<T>(s)),h(g){}

    Cuboid_bldg(const Point_2 &pmin, const Point_2 &pmax, const FT &g): rect_2(Rectangle_2<T>(pmin,pmax)),h(g){}

    Cuboid_bldg(const Rectangle_2<T> &rec, const FT & g): rect_2(rec),h(g) {}

    inline bool operator==(const Cuboid_bldg &i) const {return (rect_2==i.rect_2) && (h==i.h);}

    inline bool operator!=(const Cuboid_bldg &i) const {return ! (*this == i);}


/* Modifiers */
    inline void center(const Point_2& p ) { rect_2.c=p; }
    inline void normal(const Vector_2& v) { rect_2.n=v; }
    inline void ratio (FT f             ) { rect_2.r=f; }
    inline void height(FT g             ) { h=g;        }



///* Convenience Modifiers */
//    inline void translate       (const Vector_2& v ) { rect_2.translate(v); }
//    inline void scale_and_rotate(const Vector_2& v, const FT &g ) { rect_2.scale_and_rotate(v); h=g*h; }
//    inline void scale           (const FT &f, const FT &g ) { rect_2.scale(f); h=g*h; }
//    inline void reverse_orientation () { rect_2.reverse_orientation(); }
//
//    inline Cuboid_bldg rotate(int i) {return Cuboid_bldg(rect_2.rotate(i),h);}
//    inline Cuboid_bldg translated(const Vector_2& v ) const { return Cuboid_bldg(rect_2.translated(v),h);}
//    inline Cuboid_bldg scaled_and_rotated(const Vector_2& v, const FT &g ) const { return Cuboid_bldg(rect_2.scaled_and_rotated(v),g*h);}
//    inline Cuboid_bldg opposite() const { return Cuboid_bldg(rect_2.opposite(),h); }
//    inline Cuboid_bldg reversed_orientation () const { return opposite(); }
//    inline Cuboid_bldg scaled(const FT &f, const FT &g) const { return Cuboid(rect_2.scaled(f),g*h); }
//
//    Cuboid_bldg scaled_center(int i, const FT &f) const {return Cuboid_bldg(rect_2.scaled_center(i,f),h);}
//    Cuboid_bldg scaled_edge(int i, const FT &f) const {return Cuboid_bldg(rect_2.scaled_edge(i,f),h);}
//    Cuboid_bldg rotation_scaled_corner(int i, const Vector_2& v) const {return Cuboid_bldg(rect_2.rotation_scaled_corner(i,v),h);}


    inline bool is_degenerate() const {return rect_2.is_degenerate(); }

    inline const Point_2&  center() const { return rect_2.center(); }
    inline const Vector_2& normal() const { return rect_2.normal(); }
    inline const FT&       ratio () const { return rect_2.ratio(); }

    inline const FT area() const
    {
        FT l1 = 2*sqrt(this->normal().squared_length());
        FT l2 = l1*this->ratio();
        return l1*l2;
    }

    Point_2 point(int i) const
    {
        return rect_2.point(i);
    }
   // point(i) aliases
    inline Point_2 operator[](int i) const { return point(i); }

    Segment_2 segment(int i) const
    {
        return rect_2.segment(i);
    }

     Line_2 line(int i) const
     {
        return rect_2.line(i);
     }

}; // class template Cuboid_bldg

} //namespace geometry


#endif
