#ifndef RULE_HPP
#define RULE_HPP

    class RuleLot{
        public:
            RuleLot(){}
            ~RuleLot(){}

        public:
            double _hMin;
            double _hMax;
            double _hFloor;
            double _cesMax;
            double _cosMax;

    };

    class Envelope
    {
    public:
        Envelope(){}
        Envelope(double widthRd):_prospect(widthRd)
        {
            if(_prospect<8)
            {
                _h_main = _prospect+4;
                _slope1 = 1;
                _h_slope1 = 3;
                _slope2 = 0;
                _h_slope2 = 0;
                _radius = 0;
            }
            else if(_prospect>=8 && _prospect<12)
            {
                _h_main = _prospect+4;
                _slope1 = 2;
                _h_slope1 = 4.5;
                _slope2 = 0;
                _h_slope2 = 0;
                _radius = 0;
            }
            else if(_prospect>=12 && _prospect<20)
            {
                _h_main = _prospect+3;
                _slope1 = 2;
                _h_slope1 = 3;
                _slope2 = 1;
                _h_slope2 = 6;
                _radius = 0;
            }
            else if(_prospect>=20)
            {
                _h_main = (_prospect+3) > 25 ? 25:(_prospect+3);
                _slope1 = 0;
                _h_slope1 = 0;
                _slope2 = 0;
                _h_slope2 = 0;
                _radius = 6;
            }
            else
                std::cerr<<"no definition"<<std::endl;

        }
        ~Envelope(){}

        inline double getHMain(){return _h_main;}
        inline double getHTotal(){return _h_main+_h_slope1+_h_slope2;}

    public:
        double _prospect;
        double _h_main;
        double _slope1;
        double _h_slope1;
        double _slope2;
        double _h_slope2;
        double _radius;
    };

    class RuleRoad
    {
    public:
        RuleRoad(){}
        RuleRoad(double widthRd):_envelope(Envelope(widthRd)){}
        ~RuleRoad(){}

        inline double getHMain(){return _envelope.getHMain();}

    public:
        Envelope _envelope;
    };

//end of old implementation

#include "Constraint.hpp"
#include "Energy.hpp"



    typedef enum {
        RT_DistanceFront    = 1<<0,
        RT_DistanceSide     = 1<<1,
        RT_DistanceBack     = 1<<2,
        RT_DistancePairwise = 1<<3,
        RT_CES              = 1<<4,
        RT_COS              = 1<<5,
        RT_Height           = 1<<6,
    } RuleType;


    class Rule{
    public:
        typedef std::vector<Constraint*> Constraints;
        typedef std::map<Var,double> VarValue;

        Rule(RuleType t,Constraints& cs):_type(t),_constraints(cs){}
        ~Rule(){for(Constraints::iterator it=_constraints.begin();it!=_constraints.end();++it)
                    if((*it)!=NULL) delete *it;}

        virtual double energy(VarValue,double xExpr=0.,int i=0){}

    protected:
        RuleType _type;
        Constraints _constraints;

    };


    class RuleDynamic : public Rule//one variable in constraint expression
    {
    public:
        RuleDynamic(RuleType t,Constraints& cs):Rule(t,cs){}


        double energy(VarValue varValue,double xExpr=0.,int i=0)
        {
            EnergyPLU* p = _constraints[i]->toEnergy(xExpr);
            double out = (*p)(varValue);
            delete p;
            return out;
        }
    };

    //no dependencies for calculating rightvalue of the constraint
    //energy function can be created in advance
    class RuleStatic :public Rule
    {

    public:
        typedef std::vector<EnergyPLU*> Energies;
         RuleStatic(RuleType t,Constraints& cs):Rule(t,cs){createEnergy();}
        ~RuleStatic(){for(Energies::iterator it=_energies.begin();it!=_energies.end();++it)
                        if((*it)!=NULL) delete *it;}

        double energy(VarValue varValue,double x=0.,int i=0){return (*_energies[i])(varValue);}

    private:
        void createEnergy(void){
             for(Constraints::iterator it=_constraints.begin();it!=_constraints.end();++it)
                _energies.push_back((*it)->toEnergy(0));
        }

    private:
        Energies _energies;
    };



#endif // RULE_HPP
