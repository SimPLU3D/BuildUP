#ifndef CONTROL_HPP
#define CONTROL_HPP

#include "Lot.hpp"
#include "geometry/Rectangle_2.hpp"
#include "rjmcmc/random.hpp"
#include <gdal/ogrsf_frmts.h>
#include <boost/random/variate_generator.hpp>

#include <cmath>
#ifndef M_PI
const double M_PI = 4.0 * atan(1.0);
#endif // #ifndef M_PI

class Control_inPolygon{

public:
    Control_inPolygon(OGRPolygon* ply):m_polygon(ply){}
   // Control_inPolygon(const Control_inPolygon& o){m_polygon=o.m_polygon;}
    ~Control_inPolygon(){}

    template <typename T>
    int operator () (T & obj) const
    {

        geometry::Rectangle_2<geometry::Simple_cartesian<double>> a = obj.bottom();

        OGRPolygon plyA;
        OGRLinearRing ringA;
        for(int i=0;i<5;i++)
        {
            if(a.point(i).x()!= a.point(i).x() || a.point(i).y()!= a.point(i).y())
                return 0;
            for(int j=i+1;j<4;j++)
                if(a.point(i)[0] == a.point(j)[0] && a.point(i)[1] == a.point(j)[1])
                    return 0;

            double x = geometry::to_double(a.point(i).x());
            double y = geometry::to_double(a.point(i).y());
            if(isinf(x) || isinf(y))
                return 0;
            ringA.addPoint(x,y);
        }
        plyA.addRing(&ringA);

        if(!plyA.Within(m_polygon))
            return 0;

        return 1;

    }

public:
    OGRPolygon* m_polygon;
};

class Variate_rho_discrete
{
public:
    Variate_rho_discrete(std::vector<double>& value): m_dice(rjmcmc::random(), boost::uniform_int<>(0,value.size()-1)), m_value(value) {}
    inline double operator()() const {return m_value[m_dice()];}

private:
    typedef boost::variate_generator<rjmcmc::mt19937_generator&, boost::uniform_int<> > dice_type;
    mutable dice_type m_dice;
    std::vector<double> m_value;

};

class Variate_theta //parallel or perpendicular to reference border segment
{
public:
    Variate_theta(Lot* lot):m_lot(lot),m_dice(rjmcmc::random(), boost::uniform_int<>(0,1)){}

    template<typename Iterator>
    inline double operator()(Iterator it) const
    {
        double theta =m_lot->getRefTheta_front(it); // (0,Pi]
        if(theta<M_PI*0.5)
            return theta + m_dice()*M_PI*0.5;
        else
            return theta - m_dice()*M_PI*0.5;
    }

private:
    Lot* m_lot;
    typedef boost::variate_generator<rjmcmc::mt19937_generator&, boost::uniform_int<> > dice_type;
    mutable dice_type m_dice;
};

class Variate_height
{
public:
    Variate_height(Lot* lot)
    :m_hMax(lot->ruleGeom()->hMax()),m_hMin(lot->ruleGeom()->hMin()),m_hFloor(lot->ruleGeom()->hFloor()){}

    template<typename Iterator>
    double operator()(Iterator it) const
    {
        double w = (*(it+2))*2;
 //       double h = *(it+5);

//        if(w<5) // 1 floor
//            return m_hMin;
//
//        if(w>=5 && w<7)// <=2 floors
//            return std::min(h,m_hFloor*2);
//
//        if(w>=7 && w<8) //<=3 floors
//            return std::min(h,m_hFloor*3);
//
//        if(w>=8 && w<15)// <=6 floors
//            return std::min(h,m_hFloor*6);
//
//        if(w>=15)
//            return h;
//
        if(w<5) // 1 floor
            return m_hFloor;

        if(w>=5 && w<7)// <=2 floors
            return m_hFloor*2;

        if(w>=7 && w<8) //<=3 floors
            return m_hFloor*3;

        if(w>=8 && w<15)// <=6 floors
            return m_hFloor*6;

        if(w>=15)
            return m_hMax;

        else
            exit(1);


    }

private:
    double m_hMax;
    double m_hMin;
    double m_hFloor;
};

template<typename VariateTheta, typename VariateH>
class Control_Theta_H
{
public:

    Control_Theta_H(VariateTheta& vTheta,VariateH& vH)
    :m_variate_theta(vTheta),m_variate_h(vH){}

    template<typename Iterator>
    void apply(Iterator it) const
    {
        *(it+3) = m_variate_theta(it);
        *(it+5) = m_variate_h(it);
    }

 private:
    VariateTheta m_variate_theta;
    VariateH m_variate_h;
};


template<typename VariateRho, typename VariateTheta, typename VariateH>
class Control_Rho_Theta_H
{
public:
    Control_Rho_Theta_H(VariateRho& vRho, VariateTheta& vTheta,VariateH& vH)
    :m_variate_rho(vRho),m_variate_theta(vTheta),m_variate_h(vH){}

    template<typename Iterator>
    void apply(Iterator it) const
    {
        *(it+2) = m_variate_rho();
        *(it+3) = m_variate_theta(it);
        *(it+5) = m_variate_h(it);
    }

 private:
    VariateRho m_variate_rho;
    VariateTheta m_variate_theta;
    VariateH m_variate_h;

};


#endif // CONTROL_HPP
