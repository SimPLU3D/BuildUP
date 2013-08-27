#ifndef RULE_HPP
#define RULE_HPP

#define HMIN 3
#define HMAX 50
#define CESMAX 0.5 //coefficient d'emprise au sol: aire bâti/aire parcelle
#define COSMAX 2.0 //coefficient d'occupation des sols: aire plancher/aire parcelle
#define DISTMIN 5.0

    class RuleLot{
        public:
            RuleLot(): _hMin(HMIN), _hMax(HMAX), _cesMax(CESMAX), _cosMax{COSMAX}{}

            RuleLot(double hMin, double hMax,double cesMax,double cosMax)
            : _hMin(hMin), _hMax(hMax), _cesMax(cesMax), _cosMax(cosMax){}

            RuleLot(const RuleLot& o)
            {_hMin=o._hMin; _hMax = o._hMax; _cesMax = o._cesMax; _cosMax = o._cosMax;}

            RuleLot & operator = (const RuleLot & o)
            {_hMin=o._hMin; _hMax = o._hMax; _cesMax = o._cesMax; _cosMax = o._cosMax; return *this;}

            ~RuleLot(){}

        public:
            double _hMin;
            double _hMax;
            double _cesMax;
            double _cosMax;

    };

    class RuleRoad{
        public:
            RuleRoad(): _distMin(DISTMIN){}

            RuleRoad(double distMin):_distMin(distMin){}

            ~RuleRoad(){}

        public:
            double _distMin;
    };


#endif // RULE_HPP
