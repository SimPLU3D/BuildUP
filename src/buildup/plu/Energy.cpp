#include "buildup/plu/Energy.hpp"


/********************composite energy**********************************/
EnergyComposite::EnergyComposite(EnergyPLU* lc,EnergyPLU* rc,Relation r)
:_leftChild(lc),_rightChild(rc),_r(r) {}

EnergyComposite::~EnergyComposite()
{
    if(_leftChild!=NULL) delete _leftChild;
    if(_rightChild!=NULL) delete _rightChild;
}

double EnergyComposite::operator()(VarValue& vMap)
{
    switch (_r){
    case Relation::And:
        return std::max((*_leftChild)(vMap),(*_rightChild)(vMap));
    case Relation::Or:
        return std::min((*_leftChild)(vMap),(*_rightChild)(vMap));
    default:
        std::cerr<<"invalid relation\n";
        exit(1);
    }
}

bool EnergyComposite::isValid(VarValue& vMap)
{
    switch (_r){
    case Relation::And:
        if(_leftChild->isValid(vMap) && _rightChild->isValid(vMap))
            return true;
        else
            return false;
    case Relation::Or:
        if(_leftChild->isValid(vMap) || _rightChild->isValid(vMap))
            return true;
        else
            return false;
    default:
        std::cerr<<"invalid relation\n";
        exit(1);
    }
}


/********************piecewise energy**********************************/
EnergyPiecewise::EnergyPiecewise(Var v,Intervals acceptIntervals
,double acceptScale, EnergyFuncType acceptType
,double penaltyScale, EnergyFuncType penaltyType)
:_var(v){generateEnergyMap(acceptIntervals,acceptScale,acceptType,penaltyScale,penaltyType);}

EnergyPiecewise::~EnergyPiecewise()
{
    if(!_energyMap.empty())
        for(EnergyMap::iterator it=_energyMap.begin(); it!= _energyMap.end(); ++it)
            delete it->second;
}

double EnergyPiecewise::operator()(VarValue& vMap)
{
    VarValue::iterator it = vMap.find(_var);
    if(it == vMap.end()){
        //std::cerr<<"error no matched variable "<<_var._name<<"\n";
        return 0.;
        //exit(1);
    }

    double x = it->second;
    IntervalMap::const_iterator it1 = _intervalMap.find(x);
    if(it1 ==_intervalMap.end())
        std::cerr<<"error no matched interval for "<<x<<"\n";

    EnergyMap::iterator it2 = _energyMap.find(it1->second);
    if(it2 == _energyMap.end())
        std::cerr<<"error undefined energy function";

    return (*(it2->second))(x);
}

bool EnergyPiecewise::isValid(VarValue& vMap)
{
    VarValue::iterator it = vMap.find(_var);
    if(it == vMap.end()){
        std::cerr<<"error no matched variable "<<_var._name<<"\n";
        exit(1);
    }

    double x = it->second;
    IntervalMap::const_iterator it1 = _intervalMap.find(x);
    if(it1 ==_intervalMap.end())
        std::cerr<<"error no matched interval for "<<x<<"\n";

    if(it1->second<0) //acceptable intervals are mapped to negative values
        return true;
    return false;
}

void EnergyPiecewise::generateEnergyMap(Intervals& acceptIntervals,double acceptScale,EnergyFuncType acceptType,double penaltyScale, EnergyFuncType penaltyType)
{
    //std::cout<<"generate energy map\n";
    //map acceptable intervals to -1
    for(Intervals::iterator it=acceptIntervals.begin(); it!=acceptIntervals.end(); ++it)
        _intervalMap += std::make_pair(*it,-1);

    typedef std::numeric_limits<double> Limit;
    Interval whole = boost::icl::interval<double>::open(-Limit::infinity(),Limit::infinity());
    //map acceptable intervals to 1; create unacceptable intervals and map them to 2
    _intervalMap += std::make_pair(whole,2);


    int iAccept = -2, iPenalty = 0;
    typename IntervalMap::iterator it;
    for(it=_intervalMap.begin(); it!=_intervalMap.end(); ++it)
    {
        if(it->second==1) //accept interval
        {
            it->second += iAccept--;

            if(it->first.upper() == Limit::infinity())
                _energyMap[it->second] = new EnergyFunc(it->first.lower(),acceptScale,acceptType);

            else
                _energyMap[it->second] = new EnergyFunc(it->first.upper(),acceptScale,acceptType);

        }

        else
        {

            //add penalty energy
            it->second += iPenalty++;

            if(it->first.lower() == -Limit::infinity())
                _energyMap[it->second] = new EnergyFunc(it->first.upper(),penaltyScale,penaltyType);

            else if(it->first.upper() == Limit::infinity())
                _energyMap[it->second] = new EnergyFunc(it->first.lower(),penaltyScale,penaltyType);

            else
            {
                //split interval
                double midpoint = (it->first.lower()+it->first.upper())*0.5;

                Interval iv = boost::icl::construct<Interval>(midpoint,it->first.upper(),it->first.bounds());

                _intervalMap += std::make_pair(iv,1);
                --it;
                _energyMap[it->second] = new EnergyFunc(it->first.lower(),penaltyScale,penaltyType);
                ++it;
                _energyMap[it->second] = new EnergyFunc(it->first.upper(),penaltyScale,penaltyType);
                iPenalty++;
            }
        }

        //std::cout<<it->first<<" index "<<it->second<<"\n";
    }
    // std::cout<<"\n";
}
