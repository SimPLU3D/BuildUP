#ifndef ENERGY_HPP_INCLUDED
#define ENERGY_HPP_INCLUDED

#include "Expression.hpp"
#include <boost/math/special_functions/erf.hpp>


    class Energy
    {
    public:
        virtual inline double operator()(double){}
    };


    class EnergyZero : public Energy
    {
    public:
        inline double operator() (double x){return 0.;}
    };


    class EnergySmooth : public Energy
    {
    public:
        EnergySmooth(double o):_offset(o){}
        //inline double operator() (double x){return std::abs(boost::math::erf(x-_offset));}
        inline double operator() (double x){return std::abs(boost::math::erf((x-_offset)*0.4));}

    private:
        double _offset;
    };

    class EnergyPLU
    {
    public:
        EnergyPLU(EnergyPLU* lc,EnergyPLU* rc):_leftChild(lc),_rightChild(rc){}
        virtual ~EnergyPLU(){
        if(_leftChild!=NULL) delete _leftChild;if(_rightChild!=NULL) delete _rightChild;}
        typedef std::map<Var,double> VarMap;
        virtual double operator()(VarMap){}

    protected:
        EnergyPLU* _leftChild;
        EnergyPLU* _rightChild;
    };

    class EnergyComposite: public EnergyPLU
    {
    public:
        EnergyComposite(EnergyPLU* lc,EnergyPLU* rc,Relation r):EnergyPLU(lc,rc),_r(r){}
        double operator()(VarMap vMap)
        {
            switch (_r)
            {
            case RL_And:
                return std::max((*_leftChild)(vMap),(*_rightChild)(vMap));
            case RL_Or:
                return std::min((*_leftChild)(vMap),(*_rightChild)(vMap));
            }
        }
    private:
        Relation _r;
    };

    class EnergyPiecewise : public EnergyPLU
    {
    public:
        typedef boost::icl::continuous_interval<double> Interval;
        typedef std::vector<Interval> Intervals;
        typedef boost::icl::interval_map<double,int> IntervalMap;
        typedef std::map<int,Energy*> EnergyMap;

        EnergyPiecewise(Var v,Intervals intervals):EnergyPLU(NULL,NULL),_var(v){init(intervals);}
        ~EnergyPiecewise(){
        if(!_energyMap.empty()) for(EnergyMap::iterator it=_energyMap.begin(); it!= _energyMap.end();++it) delete it->second;}


        double operator() (VarMap vMap)
        {
            VarMap::iterator it = vMap.find(_var);
            if(it == vMap.end())
                std::cerr<<"error no matched variable";
            double x = it->second;
            IntervalMap::const_iterator it1 = _intervalMap.find(x);
            if(it1 ==_intervalMap.end())
                std::cerr<<"error no matched interval for "<<x<<"\n";

            EnergyMap::iterator it2 = _energyMap.find(it1->second);
            if(it2 == _energyMap.end())
                std::cerr<<"error undefined energy function";

            return (*(it2->second))(x);
        }

    private:
        //zero energy mapped value ==1; smooth energy mapped value >=2
        void init(Intervals& intervals)
        {
            for(Intervals::iterator it=intervals.begin();it!=intervals.end();++it)
                _intervalMap += std::make_pair(*it,-1);

            typedef std::numeric_limits<double> Limit;
            Interval whole = boost::icl::interval<double>::open(-Limit::infinity(),Limit::infinity());
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
                    _energyMap[it->second] = new EnergySmooth(it->first.upper());

                else if(it->first.upper() == Limit::infinity())
                    _energyMap[it->second] = new EnergySmooth(it->first.lower());

                else{
                    //split interval
                    double midpoint = (it->first.lower()+it->first.upper())*0.5;

                    Interval iv = boost::icl::construct<Interval>(midpoint,it->first.upper(),it->first.bounds());

                    _intervalMap += std::make_pair(iv,1);
                    --it;
                    _energyMap[it->second] = new EnergySmooth(it->first.lower());
                    ++it;
                    _energyMap[it->second] = new EnergySmooth(it->first.upper());
                    nSmooth++;}
            }
        }


    private:
        Var _var;
        IntervalMap _intervalMap; //interval - index (key of _energyMap)
        EnergyMap _energyMap; //index - Energy*
    };



#endif // ENERGY_HPP_INCLUDED
