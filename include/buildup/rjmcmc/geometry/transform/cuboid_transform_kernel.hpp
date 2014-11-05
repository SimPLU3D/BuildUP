#ifndef CUBOID_TRANSFORM_KERNEL_HPP
#define CUBOID_TRANSFORM_KERNEL_HPP

#include "buildup/rjmcmc/geometry/Cuboid.hpp"
#include <cmath>
#ifndef PI
#define PI 3.14159265
#endif // #ifndef PI

//translate the edge parallel to the normal (short edge); width is fixed
//TODO: predicate (if inside parcel)
//template<unsigned int N=0>
//struct cuboid_edge_translation_transform
//{
//    enum { dimension = 7 };
//    typedef Cuboid value_type;
//
//    template<int I,typename Iterator>
//    inline double abs_jacobian(Iterator it) const
//    {
//        return 1.;
//    }
//
//    template<int I, typename IteratorIn,typename IteratorOut>
//    inline double apply  (IteratorIn in, IteratorOut out) const
//    {
//        double res = abs_jacobian<I> (in);
//        typedef typename K::FT FT;
//        FT x = *in++;
//        FT y = *in++;
//        FT rho = *in++;
//        FT theta = *in++;
//        FT r = *in++;
//        FT h = *in++;
//        FT s = *in++;
//
//        FT u = rho*std::cos(theta);
//        FT v = rho*std::sin(theta);
//        FT f = std::max(1/r,exp(4.0*(s-0.5))); //f >= 1/r  so that new ratio f*r>=1
//        //FT f = s+0.5;
//
//
////        if(f*r<1) //new ratio <1
////        {
////            *out++ = x-r*v*(f-1);
////            *out++ = y+r*u*(f-1);
////            *out++ = rho*f*r;
////            *out++ = theta+Pi/2;
////            *out++ = 1/f*r;
////            *out++ = h;
////            *out++ = 1.0-s;
////        }
////
////        else{
//        *out++ = x-r*v*(f-1);
//        *out++ = y+r*u*(f-1);
//        *out++ = rho;
//        *out++ = theta;
//        *out++ = f*r;
//        *out++ = h;
//        *out++ = 1.0-s;
////            }
//        return res;
//
//    }
//
//};
//
//
////controlled wrt width
template<unsigned int N=0>
struct cuboid_height_scaling_transform
{
    double _hMin;
    double _hMax;
    double _hFloor;

    enum { dimension = 7 };
    typedef Cuboid value_type;

    template<int I, typename Iterator>
    inline double abs_jacobian(Iterator it) const
    {
        return 1.;
    }

    template<int I, typename IteratorIn,typename IteratorOut>
    inline double apply  (IteratorIn in, IteratorOut out) const
    {
        double res = abs_jacobian<I>(in);
        typedef typename K::FT FT;
        *out++ = *in++;
        *out++ = *in++;
        FT w = *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
        FT h = *in++;
        FT s = *in++;

        double hMax = _hMax;
        if(w<5) // 1 floor
            hMax = _hFloor;

        if(w>=5 && w<7)// <=2 floors
            hMax = _hFloor*2;

        if(w>=7 && w<8) //<=3 floors
            hMax = _hFloor*3;

        if(w>=8 && w<15)// <=6 floors
            hMax = _hFloor*6;

        double nDelta = std::ceil((hMax-_hMin)/_hFloor);

        // FT f = exp(2.0*(s-0.5));
        // *out++ = (f*h>=_hMin && f*h<= _hMax)? f*h: (_hMin + std::floor(s*s*_nDelta)*_hFloor);
        *out++ = _hMin + std::round(s*s*nDelta)*_hFloor;
        *out++ = 1.0-s;

        return res;
    }

    cuboid_height_scaling_transform(double hMin, double hMax,double hFloor)
        :_hMin(hMin),_hMax(hMax),_hFloor(hFloor) {}
};

#endif // CUBOID_TRANSFORM_KERNEL_HPP
