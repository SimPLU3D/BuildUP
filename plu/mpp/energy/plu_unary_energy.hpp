#ifndef PLU_UNARY_ENERGY_HPP
#define PLU_UNARY_ENERGY_HPP

//#include "geometry/intersection/Rectangle_2_polygon_intersection.hpp"
#include "Lot.hpp"

template<typename Value = double>
class plu_unary_energy : public rjmcmc::energy<Value>
{
public:
    typedef Value result_type;

    template<typename T>
    inline result_type operator()(const T &t) const
    {
//        //TEST
//        std::cout<<"unary"<<std::endl;
//        for(int i=0;i<4;i++)
//        {
//            std::cout<<std::fixed<<t.point(i).x()<<" "<<t.point(i).y()<<std::endl;
//        }
//        std::cout<<std::endl;

        return t.area()/_lot->_area;
        //return geometry::intersection_area(t,_lot._polygon);
    }

//    // optimized version of the expression "operator()(t,u)!=0"
//    template<typename T>
//    inline bool interact(const T &t) const {
//        return geometry::do_intersect(t, _lot._polygon);
//    }

    plu_unary_energy(Lot* lot) : _lot(lot) {}

private:
    Lot* _lot;
};

#endif // PLU_UNARY_ENERGY_HPP
