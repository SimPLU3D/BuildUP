#ifndef DISTRIBUTION_HPP_INCLUDED
#define DISTRIBUTION_HPP_INCLUDED

#include <boost/random/poisson_distribution.hpp>
#include <boost/math/distributions/poisson.hpp>
class poisson_distribution_max {
public:
    typedef double real_type;
    typedef int    int_type;
    typedef boost::poisson_distribution<int_type,real_type> rand_distribution_type;
    typedef boost::math::poisson_distribution<real_type>    math_distribution_type;

    poisson_distribution_max(int_type nMax): m_nMax(nMax),m_rand(nMax), m_math(nMax){}


    real_type pdf_ratio(int_type n0, int_type n1) const
    {
        if(n1>m_nMax)
            return 0.;
        // new/old: (mean^(n1-n0) * n0! / n1!
        real_type res = 1.;
        for(;n1>n0;--n1) res *= m_math.mean()/n1;
        for(;n0>n1;--n0) res *= n0/m_math.mean();
        return res;
    }

    real_type pdf(int_type n) const
    {
        if(n>m_nMax)
            return 0.;
        return boost::math::pdf(m_math, n);
    }

    template<typename Engine>
    inline int_type operator()(Engine& e) const { return m_rand(e); }

private:
    int_type m_nMax;
    mutable rand_distribution_type m_rand;
    math_distribution_type m_math;
};

#endif // DISTRIBUTION_HPP_INCLUDED
