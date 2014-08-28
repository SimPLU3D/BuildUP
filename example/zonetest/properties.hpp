#ifndef PROPERTIES_HPP
#define PROPERTIES_HPP

#include <fstream>
#include "buildup/plu/Lot.hpp"



template<typename Configuration, typename Param>
void properties(Param& p,const Configuration& c, Lot* lot,std::ofstream& fs,double& lcr,double& far,double& e)
{

    if(c.empty())
    {
        std::cout<<"empty config"<<"\n";
        fs<<"empty config"<<"\n";
        return ;
    }

    double area=0,areaF=0,hF = lot->ruleGeom()->hFloor();
    int i = -1;
    typename Configuration::const_iterator it;
    for(it=c.begin();it!=c.end();++it)
    {
        i++;
        std::cout<<"\nbldg "<<i<<"\n";
        fs<<"\nbldg "<<i<<"\n";

        area+=c[it].area();
        areaF+=c[it].area()*(int)(c[it].h()/hF);

        //height
        std::cout<<"  ruleHeight: "<<lot->ruleGeom()->strH()<<"\n";
        std::cout<<"    --h = "<<c[it].h()<<"\n";
        fs<<"  ruleHeight: "<<lot->ruleGeom()->strH()<<"\n";
        fs<<"    --h = "<<c[it].h()<<"\n";

        //distance to borders
        std::map<std::string,double> dist;
        std::map< std::string,double >::iterator itD;
        std::map<Var,double> var_value;

        lot->dist2borders(c[it],dist);
        for(itD=dist.begin();itD!=dist.end();++itD)
            var_value.insert(std::make_pair(Var("d"+itD->first),itD->second));

        double eFront=0,eSide=0,eBack=0;
        if(lot->hasRule(RuleType::DistFront))
        {
            std::cout<<"  ruleFront: "<<lot->ruleEnergy(RuleType::DistFront)->ruleString()<<"\n";
            fs<<"  ruleFront: "<<lot->ruleEnergy(RuleType::DistFront)->ruleString()<<"\n";

            for(itD=dist.begin();itD!=dist.end();++itD)
            {
                if(itD->first.find("Front")==std::string::npos)
                    continue;
                std::cout<<"    --"<<"d"+itD->first<<" = "<<itD->second<<"\n";
                fs<<"    --"<<"d"+itD->first<<" = "<<itD->second<<"\n";
            }

            eFront = p.get<double>("wdborder")*p.get<double>("erej")*(lot->ruleEnergy(RuleType::DistFront))->energy(var_value,c[it].h());
            std::cout<<"  eFront = "<<eFront<<"\n";
            fs<<"  eFront = "<<eFront<<"\n";
        }

        if(lot->hasRule(RuleType::DistSide))
        {
            std::cout<<"  ruleSide: "<<lot->ruleEnergy(RuleType::DistSide)->ruleString()<<"\n";
            fs<<"  ruleSide: "<<lot->ruleEnergy(RuleType::DistSide)->ruleString()<<"\n";

            for(itD=dist.begin();itD!=dist.end();++itD)
            {
                if(itD->first.find("Side")==std::string::npos)
                    continue;

                std::cout<<"    --"<<"d"+itD->first<<" = "<<itD->second<<"\n";
                fs<<"    --"<<"d"+itD->first<<" = "<<itD->second<<"\n";
            }


            eSide = p.get<double>("wdborder")*p.get<double>("erej")*(lot->ruleEnergy(RuleType::DistSide))->energy(var_value,c[it].h());
            std::cout<<"  eSide = "<<eSide<<"\n";
            fs<<"  eSide = "<<eSide<<"\n";

        }

        if(lot->hasRule(RuleType::DistBack))
        {
            std::cout<<"  ruleBack: "<<lot->ruleEnergy(RuleType::DistBack)->ruleString()<<"\n";
            fs<<"  ruleBack: "<<lot->ruleEnergy(RuleType::DistBack)->ruleString()<<"\n";

            for(itD=dist.begin();itD!=dist.end();++itD)
            {
                if(itD->first.find("Back")==std::string::npos)
                    continue;
                std::cout<<"    --"<<"d"+itD->first<<" = "<<itD->second<<"\n";
                fs<<"    --"<<"d"+itD->first<<" = "<<itD->second<<"\n";
            }


            eBack = p.get<double>("wdborder")*p.get<double>("erej")*(lot->ruleEnergy(RuleType::DistBack))->energy(var_value,c[it].h());
            std::cout<<"  eBack = "<<eBack<<"\n";
            fs<<"  eBack = "<<eBack<<"\n";

        }

    }

    if(lot->hasRule(RuleType::DistPair))
    {
        std::cout<<"\n ruleDPair: "<<lot->ruleEnergy(RuleType::DistPair)->ruleString()<<"\n";
        fs<<"\n ruleDPair: "<<lot->ruleEnergy(RuleType::DistPair)->ruleString()<<"\n";

        int isValidDistBin = 1;
        typename Configuration::const_iterator it2;
        double dBin,hMax;
        for(it=c.begin();it!=c.end();++it)
        {
            it2=it;
            ++it2;
            for(;it2!=c.end();++it2)
            {
                dBin = c[it].distance2cuboid(c[it2]);
                hMax = std::max((double)c[it].h(),(double)c[it2].h());

                if(geometry::do_intersect(c[it],c[it2]))
                    dBin = -dBin;

                std::map<Var,double> varValue;
                varValue.insert(std::make_pair(Var("dPair"),dBin));

                if(lot->ruleEnergy(RuleType::DistPair)->isValid(varValue,hMax))
                    continue;

                isValidDistBin = 0;
                std::cout<<"  --violation: dPair="<<dBin<<"\n";
                fs<<"  --violation: dPair="<<dBin<<"\n";

            }
        }
        if(isValidDistBin)
        {
            std::cout<<"  no violation\n";
            fs<<"  no Violation\n";
        }

    }

    if(lot->hasRule(RuleType::HeightDiff))
    {
        std::cout<<"\n ruleHDiff: "<<lot->ruleEnergy(RuleType::HeightDiff)->ruleString()<<"\n";
        fs<<"\n ruleHDiff: "<<lot->ruleEnergy(RuleType::HeightDiff)->ruleString()<<"\n";

        int isValidHDiff = 1;
        typename Configuration::const_iterator it2;
        double dBin,hMax,hDiff;
        for(it=c.begin();it!=c.end();++it)
        {
            it2=it;
            ++it2;
            for(;it2!=c.end();++it2)
            {
                dBin = c[it].distance2cuboid(c[it2]);
                hMax = std::max((double)c[it].h(),(double)c[it2].h());

                if(geometry::do_intersect(c[it],c[it2]))
                    dBin = -dBin;

                if(dBin>=hMax)
                    continue;

                hDiff = std::abs(c[it].h()-c[it2].h());
                std::map<Var,double> varValue;
                varValue.insert(std::make_pair(Var("hDiff"),hDiff));

                if(lot->ruleEnergy(RuleType::HeightDiff)->isValid(varValue,hMax))
                    continue;

                isValidHDiff = 0;
                std::cout<<"  --violation: hDiff="<<hDiff<<"\n";
                fs<<"  --violation: hDiff="<<hDiff<<"\n";
            }
        }

        if(isValidHDiff)
        {
            std::cout<<"  no violation\n";
            fs<<"  no Violation\n";
        }
    }


    lcr = area/lot->area(), far = areaF/lot->area(), e = c.energy();

    std::cout<<"\n ------Global rules-----\n";
    fs<<"\n ------Global rules-----\n";

    std::cout<<"ruleLCR: "<<lot->ruleEnergy(RuleType::LCR)->ruleString()<<"\n";
    std::cout<<"  --LCR = "<<lcr<<"\n";
    fs<<"ruleLCR: "<<lot->ruleEnergy(RuleType::LCR)->ruleString()<<"\n";
    fs<<"  --LCR = "<<lcr<<"\n";

    std::cout<<"ruleFAR: "<<lot->ruleEnergy(RuleType::FAR)->ruleString()<<"\n";
    std::cout<<"  --FAR = "<<far<<"\n";
    fs<<"ruleFAR: "<<lot->ruleEnergy(RuleType::FAR)->ruleString()<<"\n";
    fs<<"  --FAR = "<<far<<"\n";

    std::cout<<"\n ------Energies-----\n";
    fs<<"\n ------Energies-----\n";

   // std::cout<<"energy "<<c.energy()<<" eBorder "<<c.energy_dBorder()<<" ePair "<<c.energy_dPair()<<" eHDiff "<<c.energy_hDiff()<< " eLcr "<<c.energy_lcr()<<" eFar "<<c.energy_far()<<"\n";
    std::cout<<"eTotal:"<<c.audit_energy()<<" eBorder:"<<c.audit_dBorder()<<" ePair:"<<c.audit_dPair()<<" eHDiff:"<<c.audit_hDiff()<<  " eLCR:"<<c.audit_lcr()<<" eFAR: "<<c.audit_far()<<"\n";

   // fs<<"energy "<<c.energy()<<" eBorder "<<c.energy_dBorder()<<" ePair "<<c.energy_dPair()<< " eHDiff "<<c.energy_hDiff()<<  " eLcr "<<c.energy_lcr()<<" eFar "<<c.energy_far()<< "\n";
    fs<<"eTotal:"<<c.audit_energy()<<" eBorder:"<<c.audit_dBorder()<<" ePair:"<<c.audit_dPair()<<" eHDiff:"<<c.audit_hDiff()<< " eLCR: "<<c.audit_lcr()<<" eFAR: "<<c.audit_far()<<"\n";
}


#endif // PROPERTIES_HPP
