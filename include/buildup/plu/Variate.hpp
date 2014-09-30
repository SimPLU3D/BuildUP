#ifndef PLU_VARIATE_HPP_INCLUDED
#define PLU_VARIATE_HPP_INCLUDED

#include <boost/random/uniform_int.hpp>
#include "rjmcmc/util/random.hpp"


//N-dim discrete random variable whose sample space is generated by SampleFunctor
//Function call () performs uniform sampling or conditional sampling
template <typename SampleFunctor>
class Discrete_Variate
{
public:
    enum {dimension = SampleFunctor::dimension};
    typedef typename SampleFunctor::sample_type sample_type;
    typedef typename SampleFunctor::sample_container sample_container;
    typedef boost::uniform_int<> rand_type;

    Discrete_Variate(SampleFunctor& f):m_func(f),m_rand(0,0)
    {
        m_func(m_samples);
        if(m_samples.size()<=1)
            return;
        rand_type::param_type param(0,m_samples.size()-1);
        m_rand.param(param);
    }

    template<typename Engine,typename OutputIterator>
    inline double operator()(Engine& e, OutputIterator it) const
    {
        int i=0;
        if(m_samples.size()>1)
            i = m_rand(e);
        for(int j=0;j<dimension;++j)
            *it++ = m_samples[i][j];

        return 1./m_samples.size();
    }

    template<typename InputIterator>
    inline double pmf(InputIterator it) const
    {
        return 1./m_samples.size();
    }

    //conditional sampling
    template<typename Engine,typename OutputIterator,typename DependeeIterator>
    inline double operator()(Engine& e, OutputIterator it, DependeeIterator itd) const
    {
        sample_container samples;
        m_func(itd,samples);
        int i=0;
        if(samples.size()>1)
        {
            rand_type::param_type param(0,samples.size()-1);
            m_rand.param(param);
            i = m_rand(e);
        }
        for(int j=0;j<dimension;++j)
            *it++ = samples[i][j];
        return 1./samples.size();
    }

    template<typename InputIterator,typename DependeeIterator>
    inline double pmf(InputIterator it,DependeeIterator itd) const
    {
        sample_container samples;
        m_func(itd,samples);
        return 1./samples.size();
    }

private:
    SampleFunctor m_func;
    sample_container  m_samples;
    mutable rand_type m_rand;
};


template<typename VariateCenter,typename VariateRho
, typename VariateTheta, typename VariateRatio, typename VariateHeight
, typename Predicate>
class VariatePLU
{
    enum {dimension = 6};

    VariateCenter m_variate_center;
    VariateRho m_variate_rho;
    VariateTheta m_variate_theta;
    VariateRatio m_variate_ratio;
    VariateHeight m_variate_height;
    Predicate m_predicate;
    double m_renormalization;

public:
    VariatePLU(const VariateCenter& vc, const VariateRho& vrho, const VariateTheta& vt, const VariateRatio& vr, const VariateHeight& vh, const Predicate& p)
    :m_variate_center(vc),m_variate_rho(vrho),m_variate_theta(vt),m_variate_ratio(vr),m_variate_height(vh),m_predicate(p)
    {
            rjmcmc::mt19937_generator& e = rjmcmc::random();
            int iter=100000,count = 0;
            for(int i=0; i<iter; ++i)
            {
                double var[dimension];
                sampler(e,var);
                if(m_predicate(var))
                    ++count;
            }
            m_renormalization = iter/count;
            //std::cout<<"m_renormalization"<< m_renormalization<<"\n";
    }

    template<typename Engine,typename OutputIterator>
    inline double operator()(Engine& e,OutputIterator it) const
    {
        double var[dimension];
        double prob = sampler(e,var);
        if(m_predicate(var))
        {
            for(int i=0;i<dimension;++i)
                *it++ = var[i];
            return prob*m_renormalization;
        }
        return 0;
    }

    template<typename InputIterator>
    inline double pmf(InputIterator it) const
    {
        double c[2],rho=0,theta=0,ratio=0,height=0;
        c[0]=*it++;
        c[1]=*it++;
        rho = *it++;
        theta = *it++;
        ratio = *it++;
        height = *it++;

        double pmf = 1.;
        pmf *= m_variate_center.pmf(c);
        pmf *= m_variate_rho.pmf(&rho);
        pmf *= m_variate_theta.pmf(&theta,c);
        pmf *= m_variate_ratio.pmf(&ratio,&rho);
        pmf *= m_variate_height.pmf(&height,&rho);

        return pmf*m_renormalization;
    }

    template<typename InputIterator>
    inline double pdf(InputIterator it) const
    {
        //return this->pmf(it); //problem with new librjmcmc
        return 1.;
    }

private:
    template<typename Engine,typename OutputIterator>
    inline double sampler(Engine& e, OutputIterator it) const
    {
        double prob = 1.;
        double c[2],rho=0,theta=0,ratio=0,height=0;

        prob *= m_variate_center(e,c);
        prob *= m_variate_rho(e,&rho);
        prob *= m_variate_theta(e,&theta,c);
        prob *= m_variate_ratio(e,&ratio,&rho);
        prob *= m_variate_height(e,&height,&rho);

        *it++ = c[0];
        *it++ = c[1];
        *it++ = rho;
        *it++ = theta;
        *it++ = ratio;
        *it++ = height;

        return prob;
    }
};



#endif // PLU_VARIATE_HPP_INCLUDED
