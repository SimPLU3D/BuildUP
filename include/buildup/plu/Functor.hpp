#ifndef PLU_FUNCTOR_HPP_INCLUDED
#define PLU_FUNCTOR_HPP_INCLUDED

//sample space functors
#include <array>
#include <vector>
#include "Lot.hpp"
#include <cmath>
#ifndef PI
#define PI 3.14159265
#endif // #ifndef PI
#include <boost/math/distributions/normal.hpp>

template<unsigned int N>
using SampleType = std::array<double,N>;

template<unsigned int N>
using SampleContainer = std::vector< SampleType<N> >;

//static discrete sample sapce
struct Center_Functor
{
    enum {dimension=2};
    typedef SampleType<dimension> sample_type;
    typedef SampleContainer<dimension> sample_container;
    Lot* m_lot;
    double m_rate;

    Center_Functor(Lot* lot,double rate):m_lot(lot),m_rate(rate){}

    inline void operator()(sample_container& c) const {
        double x0=m_lot->xMin();
        double y0=m_lot->yMin();
        int nx= (int)(std::ceil(m_lot->xMax()-x0)/m_rate);
        int ny= (int)(std::ceil(m_lot->yMax()-y0)/m_rate);

        double x,y;
        for(int i=0;i<nx;++i)
        {
            x = x0+ m_rate*(i+0.5);
            for(int j=0;j<ny;++j)
            {
                y= y0+m_rate*(j+0.5);
                OGRPoint pt(x,y);
                if(pt.Within(m_lot->polygon()))
                    c.push_back(sample_type {x,y});
            }
        }
    }
};


//struct Rho_Functor
//{
//    enum {dimension=1};
//    typedef SampleType<dimension> sample_type;
//    typedef SampleContainer<dimension> sample_container;
//    Lot* m_lot;
//
//    Rho_Functor(Lot* lot):m_lot(lot){}
//
//    inline void operator()(sample_container & c) const {c=m_lot->ruleGeom()->rho();}
//};


//static discrete sample space with weights
struct Width_Functor
{
    enum {dimension=1};
    typedef SampleType<dimension> sample_type;
    typedef SampleContainer<dimension> sample_container;
    Lot* m_lot;
    double m_rate;

    Width_Functor(Lot* lot,double rate):m_lot(lot),m_rate(rate){}
    typedef boost::math::normal_distribution<> Gaussian;

    inline void operator()(sample_container & c,std::vector<double>& weights) const
    {
        std::vector<double>& wPeaks = m_lot->ruleGeom()->wPeaks();
        std::vector<Gaussian> gaussians;
        for(size_t i=0;i<wPeaks.size();++i)
            gaussians.push_back(Gaussian(wPeaks[i],0.5));


        double wMin = m_lot->ruleGeom()->wMin();
        double wMax = m_lot->ruleGeom()->wMax();
        int n= 1+(int)(std::floor(wMax-wMin)/m_rate);
        for(int i=0;i<n;++i)
        {
            double width = wMin+m_rate*i;
            c.push_back(sample_type {width});
            weights.push_back(getWeight(gaussians,width));
        }

    }
    inline double getWeight(std::vector<Gaussian>& gaussians, double x) const
    {
        if(!gaussians.size())
            return 0.;

        if(gaussians.size()==1)
            return boost::math::pdf(gaussians[0],x);

        double pdf = boost::math::pdf(gaussians[0],x);
        double pdf_i;
        for(size_t i=1;i<gaussians.size();++i)
        {
            pdf_i = boost::math::pdf(gaussians[i],x);
            if(pdf_i>pdf)
                pdf = pdf_i;
        }
        return pdf;
    }

};

//dynamic discrete sample space depending on center location
struct Theta_Functor
{
    enum {dimension=1};
    typedef SampleType<dimension> sample_type;
    typedef SampleContainer<dimension> sample_container;
    Lot* m_lot;

    Theta_Functor(Lot* lot):m_lot(lot){}

    inline void operator()(sample_container& c) const {return ;}

    template<typename Iterator>
    inline void operator()(Iterator it,sample_container& c) const
    {
        double x = (*it), y=*(it+1);
        double theta = m_lot->refTheta(x,y);
        c.push_back( sample_type {theta} );

        if(theta<PI*0.5)
            c.push_back(sample_type {theta+PI*0.5});
        else
            c.push_back(sample_type {theta-PI*0.5});
    }
};

//dynamic discrete sample space depending on width
struct Length_Functor
{
    enum {dimension=1};
    typedef SampleType<dimension> sample_type;
    typedef SampleContainer<dimension> sample_container;
    double m_min,m_max,m_rate;

    Length_Functor(Lot* lot, double rate):m_min(lot->ruleGeom()->lMin()),m_max(lot->ruleGeom()->lMax()),m_rate(rate){}

    inline void operator()(sample_container& c) const {return ;}

    template<typename Iterator>
    inline void operator()(Iterator it, sample_container& c) const
    {
        double width = *it;
        if(width>m_max)
        {
            std::cout<<"error: width>maxLen \n";
            exit(1);
        }
        double start = std::max(m_min,width);
        int n= 1+(int)(std::floor(m_max-start)/m_rate);
        for(int i=0;i<n;++i)
            c.push_back(sample_type {start+m_rate*i});

    }

};
////dynamic discrete sample space depending on width
//struct Ratio_Functor
//{
//    enum {dimension=1};
//    typedef SampleType<dimension> sample_type;
//    typedef SampleContainer<dimension> sample_container;
//    double m_minR,m_maxR,m_rate;
//
//    Ratio_Functor(double minR, double maxR, double rate):m_minR(minR),m_maxR(maxR),m_rate(rate){}
//
//    inline void operator()(sample_container& c) const {return ;}
//
//    template<typename Iterator>
//    inline void operator()(Iterator it, sample_container& c) const
//    {
//        double rho = (*it);
//        double dr = m_rate/(rho*2);
//        int n = (int)((m_maxR-m_minR)/dr);
//        for(int i=0;i<n;++i)
//            c.push_back( sample_type {m_minR+i*dr} );
//    }
//
//};

//dynamic discrete sample space depending on width
//current implementation: only one maximum value
struct Height_Functor
{
    enum {dimension=1};
    typedef SampleType<dimension> sample_type;
    typedef SampleContainer<dimension> sample_container;
    double m_hMax,m_hFloor;

    Height_Functor(Lot* lot):m_hMax(lot->ruleGeom()->hMax()),m_hFloor(lot->ruleGeom()->hFloor()){}

    inline void operator()(sample_container& c) const {return ;}

    template<typename Iterator>
    inline void operator()(Iterator it,sample_container& c) const
    {
        double w = *it;
        double h;
        if(w<5) h = m_hFloor;
        else if(w>=5 && w<7) h = std::min(m_hFloor*2,m_hMax);
        else if(w>=7 && w<8) h = std::min(m_hFloor*3,m_hMax);
        else if(w>=8 && w<15) h = std::min(m_hFloor*6,m_hMax);
        else h= m_hMax;
        c.push_back( sample_type {h});
    }
};



#endif // PLU_FUNCTOR_HPP_INCLUDED
