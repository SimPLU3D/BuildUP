#ifndef PLU_GLOBAL_ENERGY_HPP
#define PLU_GLOBAL_ENERGY_HPP

//#include "geometry/intersection/Rectangle_2_polygon_intersection.hpp"
#include "Lot.hpp"

template<typename Value = double>
class plu_global_energy : public rjmcmc::energy<Value>
{
public:
    typedef Value result_type;

    template<typename Config>
    inline result_type operator()(Config conf) const
    {
        result_type e = 0;

        for(typename Config::iterator it=conf.begin(); it!=conf.end(); ++it)
        {
            e+=conf[it].area();
        }
//        if(e/_lot->_area > _lot->_rule._cesMax)
//            return 1000;

        return abs(_lot->_rule._cesMax - e/_lot->_area)*10;
    }


    //                                Death            Birth
    template<typename Config, typename IterD, typename IterB>
    inline result_type operator()(Config conf,
                                  //IterC cbegin, IterC cend,
                                  IterD dbegin, IterD dend,
                                  IterB bbegin, IterB bend) const
    {
        result_type e = 0;

        for(typename Config::iterator it=conf.begin(); it!=conf.end(); ++it)
            if(std::find(dbegin,dend,it)==dend)
                //vec.push_back(conf.value(it));
                e+=conf[it].area();

//        for(IterB itB=bbegin; itB!=bend; ++itB)
//            //vec.push_back(*it);
//            e+=itB->area();

        return abs(_lot->_rule._cesMax - e/_lot->_area)*10;
    }

    plu_global_energy(Lot* lot) : _lot(lot) {}

private:
    Lot* _lot;
};

#endif // PLU_GLOBAL_ENERGY_HPP
