#ifndef PLU_GLOBAL_ENERGY_HPP_INCLUDED
#define PLU_GLOBAL_ENERGY_HPP_INCLUDED

#include "buildup/plu/Lot.hpp"

template<typename Value = double>
class plu_global_lcr : public rjmcmc::energy<Value>
{
public:
    typedef Value result_type;

    template<typename Configuration, typename Modification>
    result_type operator()(const Configuration &c, const Modification &modif) const
    {
        typedef typename Modification::birth_const_iterator bci; //iterator of value_type (object)
        typedef typename Modification::death_const_iterator dci; //iterator of vertex_iterator
        bci bbeg = modif.birth_begin();
        bci bend = modif.birth_end();
        dci dbeg = modif.death_begin();
        dci dend = modif.death_end();

        double area=0;
        typename Configuration::const_iterator it;
        for(it=c.begin(); it!=c.end(); ++it)
        {
            if (std::find(dbeg,dend,it)!=dend) //dead
                continue;
            area+=c[it].area();
        }

        for(bci it2=bbeg; it2!=bend; ++it2)
            area+=(*it2).area();


        double lcr = area/_lot->area();

        if(lcr!=lcr || std::isinf(lcr) || lcr<0)
            return _eRej;

        std::map<Var,double> varValue;
        varValue.insert(std::make_pair(Var("lcr"),lcr));
        return _eRej*(_lot->ruleEnergy(RuleType::LCR))->energy(varValue);

    }
    plu_global_lcr(Lot* lot,Value eRej) : _lot(lot),_eRej(eRej) {}

private:
    Lot* _lot;
    Value _eRej;
};


template<typename Value = double>
class plu_global_far: public rjmcmc::energy<Value>
{
public:
    typedef Value result_type;

    template<typename Configuration, typename Modification>
    result_type operator()(const Configuration &c, const Modification &modif) const
    {
        typedef typename Modification::birth_const_iterator bci; //iterator of value_type (object)
        typedef typename Modification::death_const_iterator dci; //iterator of vertex_iterator
        bci bbeg = modif.birth_begin();
        bci bend = modif.birth_end();
        dci dbeg = modif.death_begin();
        dci dend = modif.death_end();

        double areaF=0,hF = _lot->ruleGeom()->hFloor();
        typename Configuration::const_iterator it;
        for(it=c.begin(); it!=c.end(); ++it)
        {
            if (std::find(dbeg,dend,it)!=dend) //dead
                continue;
            areaF+=c[it].area()*(int)(c[it].h()/hF);
        }

        for(bci it2=bbeg; it2!=bend; ++it2)
            areaF+=(*it2).area()*(int)((*it2).h()/hF);

        double far = areaF/_lot->area();

        if(far!=far || std::isinf(far) || far<0)
            return _eRej;

        std::map<Var,double> varValue;
        varValue.insert(std::make_pair(Var("far"),far));
        return _eRej*(_lot->ruleEnergy(RuleType::FAR))->energy(varValue);

    }
    plu_global_far(Lot* lot,Value eRej) : _lot(lot),_eRej(eRej) {}

private:
    Lot* _lot;
    Value _eRej;

};

#endif // PLU_GLOBAL_ENERGY_HPP_INCLUDED
