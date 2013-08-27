#ifndef CUBOID_TRANSFORM_KERNEL_HPP
#define CUBOID_TRANSFORM_KERNEL_HPP

#include "geometry/Cuboid_bldg.hpp"

struct cuboid_edge_translation_transform
{
    enum { dimension = 7 };
    typedef Cuboid_bldg value_type;

    template<typename Iterator>
    inline double abs_jacobian(Iterator it) const { return 1.; }

    template<typename IteratorIn,typename IteratorOut>
    inline double apply  (IteratorIn in, IteratorOut out) const {
        double res = abs_jacobian(in);
        typedef typename K::FT FT;
        FT x = *in++;
        FT y = *in++;
        FT u = *in++;
        FT v = *in++;
        FT r = *in++;
        FT h = *in++;
        FT s = *in++;
        FT f = exp(4.0*(s-0.5));
        FT g = 1-f;
        //   res = Rectangle_2(c+m*(1-f), n,f*r);
        *out++ = x-g*r*v;
        *out++ = y+g*r*u;
        *out++ = u;
        *out++ = v;
        *out++ = f*r;
        *out++ = h;
        *out++ = 1.0-s;
        return res;
    }

    template<typename IteratorIn,typename IteratorOut>
    inline double inverse(IteratorIn in, IteratorOut out) const { return apply(in,out); }

};

struct cuboid_corner_translation_transform
{
    enum { dimension = 8 };
    typedef Cuboid_bldg value_type;

    template<typename Iterator>
    inline double abs_jacobian(Iterator it) const { return 1.; }

    template<typename IteratorIn,typename IteratorOut>
    inline double apply  (IteratorIn in, IteratorOut out) const {
        double res = abs_jacobian(in);
        typedef typename K::FT FT;
        FT x = *in++;
        FT y = *in++;
        FT u = *in++;
        FT v = *in++;
        FT r = *in++;
        FT h = *in++;
        FT s = *in++;
        FT t = *in;
        //   res = Rectangle_2(c+v+u, n+v,r);
        *out++ = x+s-r*t;
        *out++ = y+t+r*s;
        *out++ = u+s;
        *out++ = v+t;
        *out++ = r;
        *out++ = h;
        *out++ =-s;
        *out++ =-t;
        return res;
    }

    template<typename IteratorIn,typename IteratorOut>
    inline double inverse(IteratorIn in, IteratorOut out) const { return apply(in,out); }
};

struct cuboid_height_scaling_transform
{
     enum { dimension = 8 };
    typedef Cuboid_bldg value_type;

    template<typename Iterator>
    inline double abs_jacobian(Iterator it) const { return 1.; }

    template<typename IteratorIn,typename IteratorOut>
    inline double apply  (IteratorIn in, IteratorOut out) const {
        double res = abs_jacobian(in);
        typedef typename K::FT FT;
        FT x = *in++;
        FT y = *in++;
        FT u = *in++;
        FT v = *in++;
        FT r = *in++;
        FT h = *in++;
        FT s = 0.5+*in;

        *out++ = x;
        *out++ = y;
        *out++ = u;
        *out++ = v;
        *out++ = r;
        *out++ = h*s;
        *out++ = 1./s;

        return res;
    }

    template<typename IteratorIn,typename IteratorOut>
    inline double inverse(IteratorIn in, IteratorOut out) const { return apply(in,out); }
};


#endif // CUBOID_TRANSFORM_KERNEL_HPP
