#ifndef PLU_ENERGY_HPP_INCLUDED
#define PLU_ENERGY_HPP_INCLUDED

#include <boost/math/special_functions/erf.hpp>
#include <boost/icl/interval_map.hpp>
#include <boost/icl/interval.hpp>


template<typename Value = double>
class plu_energy_zero : public rjmcmc::energy<Value>
{
public:
    typedef Value result_type;

    result_type operator() (Value x)
    {
        return (result_type)0;
    }
};



template<typename Value = double>
class plu_energy_smooth : public rjmcmc::energy<Value>
{
public:
    typedef Value result_type;

    result_type operator() (Value x)
    {
        return std::abs((result_type)(_scale*boost::math::erf(x-_offset)));
    }
    plu_energy_smooth(Value o, Value s):_offset(o),_scale(s){}
private:
    Value _offset;
    Value _scale;
};


template<typename Value = double>
class plu_energy_piecewise : public rjmcmc::energy<Value>
{
public:
    typedef Value result_type;
    typedef boost::icl::continuous_interval<Value> Interval;
    typedef boost::icl::interval_map<Value,int> IntervalMap;
    typedef rjmcmc::energy<Value> Energy;
    typedef std::map<int,Energy*> EnergyMap;

    typedef plu_energy_smooth<Value> EnergySmooth;
    typedef plu_energy_zero<Value> EnergyZero;

    result_type operator() (Value x)
    {
        typename IntervalMap::const_iterator it1 = _intervalMap.find(x);
        if(it1 ==_intervalMap.end())
        {
            std::cerr<<"error no matched interval for "<<x<<"\n";
        }

        typename EnergyMap::iterator it2 = _energyMap.find(it1->second);
        if(it2 == _energyMap.end())
            std::cerr<<"error undefined energy function";

        return (result_type)(*(it2->second))(x);
    }

   // inline void addInterval(Interval& interval){_intervalMap += std::make_pair(interval,-1);}


    //zero energy mapped value ==1; smooth energy mapped value >=2
    void energyPolicy(Value penalty)
    {
        typedef std::numeric_limits<Value> Limit;
        Interval whole = boost::icl::interval<Value>::open(-Limit::infinity(),Limit::infinity());
        _intervalMap += std::make_pair(whole,2);

        Energy* pZero = new EnergyZero;
        _energyMap[1] = pZero;

        int nSmooth = 0;
        typename IntervalMap::iterator it;
        for(it=_intervalMap.begin();it!=_intervalMap.end();++it)
        {
            if(it->second==1) //zero energy
                continue;

            //add smooth energy
            it->second += nSmooth++;

            if(it->first.lower() == -Limit::infinity())
                _energyMap[it->second] = new EnergySmooth(it->first.upper(),penalty);

            else if(it->first.upper() == Limit::infinity())
                _energyMap[it->second] = new EnergySmooth(it->first.lower(),penalty);

            else{
                //split interval
                Value midpoint = (it->first.lower()+it->first.upper())*0.5;

                Interval iv = boost::icl::construct<Interval>(midpoint,it->first.upper(),it->first.bounds());

                _intervalMap += std::make_pair(iv,1);
                --it;
                _energyMap[it->second] = new EnergySmooth(it->first.lower(),penalty);
                ++it;
                _energyMap[it->second] = new EnergySmooth(it->first.upper(),penalty);
                nSmooth++;}
        }
    }

    plu_energy_piecewise(){}
    ~plu_energy_piecewise(){if(!_energyMap.empty()) for(typename EnergyMap::iterator it=_energyMap.begin(); it!= _energyMap.end();++it) delete it->second;}

private:
    IntervalMap _intervalMap; //interval - index (key of _energyMap)
    EnergyMap _energyMap; //index - Energy*
};



#endif // PLU_ENERGY_HPP_INCLUDED
