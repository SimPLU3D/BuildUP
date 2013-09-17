#ifndef RULE_HPP
#define RULE_HPP

#define HMIN 3      //min height of each building (also min height of each floor)
#define HMAX 30     //max height of each building
#define CESMAX 0.5  //coefficient d'emprise au sol: aire bâti/aire parcelle
#define COSMAX 2    //coefficient d'occupation des sols: aire plancher/aire parcelle
#define THETAMAX 8  //max angle (in degree) between building and lot normal
#define SIZEMIN 50  //min area of each building
#define RATIOMAX 2  //max ratio of length and width
#define DISTMIN_UR 5 //min distance to the border
#define DISTMIN_BI 10 //min distance between buildings


class RuleLot{
    public:
        RuleLot(): _hMin(HMIN), _hMax(HMAX), _cesMax(CESMAX), _cosMax{COSMAX}
        ,_thetaMax(THETAMAX),_sizeMin(SIZEMIN),_ratioMax(RATIOMAX)
        ,_distMinUr(DISTMIN_UR), _distMinBi(DISTMIN_BI)
        {}

        RuleLot(double hMin, double hMax,double cesMax,double cosMax
        ,double thetaMax, double sizeMin, double ratioMax,double distMinUr, double distMinBi)
        : _hMin(hMin), _hMax(hMax), _cesMax(cesMax), _cosMax(cosMax)
        ,_thetaMax(thetaMax),_sizeMin(sizeMin),_ratioMax(ratioMax)
        ,_distMinUr(distMinUr),_distMinBi(distMinBi)
        {}

        RuleLot(const RuleLot& o)
        {
            _hMin=o._hMin; _hMax = o._hMax; _cesMax = o._cesMax; _cosMax = o._cosMax;
            _thetaMax=o._thetaMax; _sizeMin=o._sizeMin;_ratioMax=o._ratioMax;
            _distMinUr=o._distMinUr; _distMinBi=o._distMinBi;
        }

        RuleLot & operator = (const RuleLot & o)
        {
            _hMin=o._hMin; _hMax = o._hMax; _cesMax = o._cesMax; _cosMax = o._cosMax;
            _thetaMax=o._thetaMax; _sizeMin=o._sizeMin;_ratioMax=o._ratioMax;
            _distMinUr=o._distMinUr; _distMinBi=o._distMinBi;
            return *this;
        }

        ~RuleLot(){}

    public:
        double _hMin;
        double _hMax;
        double _cesMax;
        double _cosMax;
        double _thetaMax;
        double _sizeMin;
        double _ratioMax;
        double _distMinUr;
        double _distMinBi;

};

#define DIST 6
#define HMAIN 10
#define HSLOPE 3
#define SLOPE 0.5
class RuleRoad
{
public:
    RuleRoad():_distance(DIST),_hMain(HMAIN),_hSlope(HSLOPE),_slope(SLOPE){}

    RuleRoad(double d, double H,double h,double s)
    :_distance(d),_hMain(H),_hSlope(h),_slope(s){}

    RuleRoad(const RuleRoad& o)
    {_distance = o._distance; _hMain = o._hMain; _hSlope = o._hSlope; _slope = o._slope;}

    RuleRoad& operator = (const RuleRoad& o)
    {_distance = o._distance; _hMain = o._hMain; _hSlope = o._hSlope; _slope = o._slope;return *this;}

    ~RuleRoad(){}

public:
    double _distance;
    double _hMain;
    double _hSlope;
    double _slope;
};

#endif // RULE_HPP
