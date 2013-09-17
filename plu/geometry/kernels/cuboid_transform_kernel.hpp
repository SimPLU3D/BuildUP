#ifndef CUBOID_TRANSFORM_KERNEL_HPP
#define CUBOID_TRANSFORM_KERNEL_HPP

#include "geometry/Cuboid_bldg.hpp"
//#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/variate_generator.hpp>
#include "rjmcmc/random.hpp"

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

class cuboid_height_scaling_transform
{
public:

    typedef boost::random::uniform_int_distribution<> rand_distribution_type;
    typedef boost::random::variate_generator<rjmcmc::mt19937_generator&,rand_distribution_type> variate_generator_type;

    cuboid_height_scaling_transform(double hMin, double hMax): _hMin(hMin),_hMax(hMax),_n((_hMax-_hMin)/_hMin)
    ,_variate_coef(rjmcmc::random(),rand_distribution_type(-_n,_n))
    ,_variate_case(rjmcmc::random(),rand_distribution_type(1,20))
    {}

    enum { dimension = 6};
    typedef Cuboid_bldg value_type;
    typedef typename K::FT FT;

    template<typename Iterator>
    inline double abs_jacobian(Iterator it) const { return 1.; }
    FT modifH(FT height) const
    {
        FT h=height;
        int coef= _variate_coef();
        h = height + coef*_hMin;
        //std::cout<<"coef "<<coef;

        if(h>_hMax || h<_hMin)
        {
            int cas = _variate_case();
            //std::cout<<" cas "<<cas;
            switch (cas){
            case 1:
                return _hMin;
            case 2:
                return _hMax;
            default:
                return height;
            }
        }
        return h;
    }

    template<typename IteratorIn,typename IteratorOut>
    inline double apply  (IteratorIn in, IteratorOut out) const {
        double res = abs_jacobian(in);

        FT x = *in++;
        FT y = *in++;
        FT u = *in++;
        FT v = *in++;
        FT r = *in++;
        FT h = *in;

        *out++ = x;
        *out++ = y;
        *out++ = u;
        *out++ = v;
        *out++ = r;
        *out = modifH(h);
        //std::cout<<" h "<<*out<<std::endl;

        return res;
    }

    template<typename IteratorIn,typename IteratorOut>
    inline double inverse(IteratorIn in, IteratorOut out) const { return apply(in,out); }

private:
    double _hMin;
    double _hMax;
    int _n;
    mutable variate_generator_type _variate_coef;
    mutable variate_generator_type _variate_case;
};


#endif // CUBOID_TRANSFORM_KERNEL_HPP
