#ifndef PLU_FUNCTOR_HPP_INCLUDED
#define PLU_FUNCTOR_HPP_INCLUDED

//sample functors
#include <array>
#include <vector>
#include "Lot.hpp"

#ifndef M_PI
const double M_PI = 4.0 * atan(1.0);
#endif // #ifndef M_PI

//namespace plu
//{
    template<unsigned int N>
    using SampleType = std::array<double,N>;

    template<unsigned int N>
    using SampleContainer = std::vector< SampleType<N> >;


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
            std::cout<<"center sample space "<<c.size()<<"\n";
        }
    };

    struct Rho_Functor
    {
        enum {dimension=1};
        typedef SampleType<dimension> sample_type;
        typedef SampleContainer<dimension> sample_container;
        Lot* m_lot;

        Rho_Functor(Lot* lot):m_lot(lot){}

        inline void operator()(sample_container & c) const {c=m_lot->ruleGeom()->rho();}
    };

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

            if(theta<M_PI*0.5)
                c.push_back(sample_type {theta+M_PI*0.5});
            else
                c.push_back(sample_type {theta-M_PI*0.5});
        }
    };

    struct Ratio_Functor
    {
        enum {dimension=1};
        typedef SampleType<dimension> sample_type;
        typedef SampleContainer<dimension> sample_container;
        double m_minR,m_maxR,m_rate;

        Ratio_Functor(double minR, double maxR, double rate):m_minR(minR),m_maxR(maxR),m_rate(rate){}

        inline void operator()(sample_container& c) const {return ;}

        template<typename Iterator>
        inline void operator()(Iterator it, sample_container& c) const
        {
            double rho = (*it);
            double dr = m_rate/(rho*2);
            int n = (int)((m_maxR-m_minR)/dr);
            for(int i=0;i<n;++i)
                c.push_back( sample_type {m_minR+i*dr} );
        }

    };

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
            double w = (*it)*2;
            double h;
            if(w<5) h = m_hFloor;
            else if(w>=5 && w<7) h = std::min(m_hFloor*2,m_hMax);
            else if(w>=7 && w<8) h = std::min(m_hFloor*3,m_hMax);
            else if(w>=8 && w<15) h = std::min(m_hFloor*6,m_hMax);
            else h= m_hMax;
            c.push_back( sample_type {h});
        }
    };

//} //namespace plu


#endif // PLU_FUNCTOR_HPP_INCLUDED
