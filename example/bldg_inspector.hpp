#ifndef BLDG_INSPECTOR_HPP_INCLUDED
#define BLDG_INSPECTOR_HPP_INCLUDED

/************ geometry ******************/
#include "buildup/rjmcmc/geometry/Cuboid.hpp"
#include "buildup/rjmcmc/geometry/coordinates/Cuboid_coordinates.hpp"
#include "buildup/rjmcmc/geometry/intersection/Cuboid_intersection.hpp"
typedef geometry::Simple_cartesian<double> K;
typedef geometry::Cuboid<K> Cuboid;
typedef K::Point_2 Point_2;

#include "buildup/plu/Building.hpp"
#include "buildup/plu/Lot.hpp"
#include <vector>
#include <gdal/ogrsf_frmts.h>

#include "rjmcmc/param/parameter.hpp"
typedef parameters< parameter > Param;
#include "bldg_generator_para.hpp"


void inspector(std::vector<Building>& bldgs,Lot* lot,std::ofstream& fs)
{
    std::vector<Cuboid> boxes;

    for(size_t i=0;i<bldgs.size();++i)
    {
        OGRPoint pt;
        bldgs[i].footprint()->Centroid(&pt);
        Point_2 c(pt.getX(),pt.getY());
        boxes.push_back(Cuboid(c,bldgs[i].width(),bldgs[i].length(),bldgs[i].theta(),bldgs[i].height()));
    }

    double area=0,areaF=0,hF = lot->ruleGeom()->hFloor();
    Param p;
    initialize_parameters(&p);

    for(size_t i=0;i<boxes.size();++i )
    {
        std::cout<<"\nbldg "<<i<<"\n";
        fs<<"\nbldg "<<i<<"\n";

        area+=boxes[i].area();
        areaF+=boxes[i].area()*(int)(boxes[i].h()/hF);

        //width
        std::cout<<"  ruleWidth: "<<lot->ruleGeom()->strW()<<"\n";
        std::cout<<"    --width = "<<boxes[i].width()<<"\n";
        fs<<"  ruleWidth: "<<lot->ruleGeom()->strW()<<"\n";
        fs<<"    --width = "<<boxes[i].width()<<"\n";

        //length
        std::cout<<"  ruleLength: "<<lot->ruleGeom()->strL()<<"\n";
        std::cout<<"    --length = "<<boxes[i].length()<<"\n";
        fs<<"  ruleLength: "<<lot->ruleGeom()->strL()<<"\n";
        fs<<"    --length = "<<boxes[i].length()<<"\n";

        //height
        std::cout<<"  ruleHeight: "<<lot->ruleGeom()->strH()<<"\n";
        std::cout<<"    --h = "<<boxes[i].h()<<"\n";
        fs<<"  ruleHeight: "<<lot->ruleGeom()->strH()<<"\n";
        fs<<"    --h = "<<boxes[i].h()<<"\n";

        //distance to borders
        std::map<std::string,double> dist,hasWindow;
        std::map< std::string,double >::iterator itD;
        std::map<Var,double> var_value;

        lot->dist2borders(boxes[i],dist,hasWindow);
        for(itD=dist.begin();itD!=dist.end();++itD)
            var_value.insert(std::make_pair(Var("d"+itD->first),itD->second));

        for(itD=hasWindow.begin();itD!=hasWindow.end();++itD)
            var_value.insert(std::make_pair(Var("hasWindow"+itD->first),itD->second));

        var_value.insert(std::make_pair(Var("h"),boxes[i].h()));

        double eFront=0,eSide=0,eBack=0;
        if(lot->hasRule(RuleType::DistFront))
        {
            std::cout<<"  ruleFront: "<<lot->ruleEnergy(RuleType::DistFront)->ruleString()<<"\n";
            fs<<"  ruleFront: "<<lot->ruleEnergy(RuleType::DistFront)->ruleString()<<"\n";

            for(itD=dist.begin();itD!=dist.end();++itD)
            {
                if(itD->first.find("Front")==std::string::npos)
                    continue;
                std::cout<<"    --"<<"d"+itD->first<<" = "<<itD->second<<" hasWindow:"<<hasWindow[itD->first]<<"\n";
                fs<<"    --"<<"d"+itD->first<<" = "<<itD->second<<" hasWindow:"<<hasWindow[itD->first]<<"\n";
            }

            eFront = p.template get<double>("wdborder")*p.template get<double>("erej")*(lot->ruleEnergy(RuleType::DistFront))->energy(var_value);
            std::cout<<"  eFront = "<<eFront<< "\n";
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

                std::cout<<"    --"<<"d"+itD->first<<" = "<<itD->second<<" hasWindow:"<<hasWindow[itD->first]<<"\n";
                fs<<"    --"<<"d"+itD->first<<" = "<<itD->second<<" hasWindow:"<<hasWindow[itD->first]<<"\n";
            }


            eSide = p.template get<double>("wdborder")*p.template get<double>("erej")*(lot->ruleEnergy(RuleType::DistSide))->energy(var_value);
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
                std::cout<<"    --"<<"d"+itD->first<<" = "<<itD->second<<" hasWindow:"<<hasWindow[itD->first]<<"\n";
                fs<<"    --"<<"d"+itD->first<<" = "<<itD->second<<" hasWindow:"<<hasWindow[itD->first]<<"\n";
            }


            eBack = p.template get<double>("wdborder")*p.template get<double>("erej")*(lot->ruleEnergy(RuleType::DistBack))->energy(var_value);
            std::cout<<"  eBack = "<<eBack<<"\n";
            fs<<"  eBack = "<<eBack<<"\n";

        }

    }

    if(lot->hasRule(RuleType::DistPair))
    {
        std::cout<<"\n ruleDPair: "<<lot->ruleEnergy(RuleType::DistPair)->ruleString()<<"\n";
        fs<<"\n ruleDPair: "<<lot->ruleEnergy(RuleType::DistPair)->ruleString()<<"\n";

        int isValidDistBin = 1;
        if(!lot->ruleEnergy(RuleType::DistPair)->isConditional())
        {

            double dBin,hHigh;
            for(size_t i=0;i<boxes.size()-1;++i)
            {
                for(size_t j=i+1;j<boxes.size();++j)
                {
                    dBin = boxes[i].distance2cuboid(boxes[j]);
                    hHigh = std::max(boxes[i].h(),boxes[j].h());

                    if(geometry::do_intersect(boxes[i],boxes[j]))
                        dBin = -dBin;

                    std::map<Var,double> varValue;
                    varValue.insert(std::make_pair(Var("dPair"),dBin));
                    varValue.insert(std::make_pair(Var("hHigh"),hHigh));

                    if(lot->ruleEnergy(RuleType::DistPair)->isValid(varValue))
                    {
                        std::cout<<"  --dPair"<<i<<"_"<<j<<"="<<dBin<<"\n";
                        fs<<"  --dPair"<<i<<"_"<<j<<"="<<dBin<<"\n";
                        continue;
                    }


                    isValidDistBin = 0;
                    std::cout<<"  --violation: dPair"<<i<<"_"<<j<<"="<<dBin<<"\n";
                    fs<<"  --violation: dPair"<<i<<"_"<<j<<"="<<dBin<<"\n";

                }
            }

        }


        else
        {
            double dBin,hHigh,hLow;
            int hasWindowPair,hasWindowHigh,hasWindowLow;
            for(size_t i=0;i<boxes.size()-1;++i)
            {
                for(size_t j=i+1;j<boxes.size();++j)
                {
                    dBin = boxes[i].distance2cuboid(boxes[j],lot->lengthHasWindow(),hasWindowPair,hasWindowHigh,hasWindowLow);
                    hHigh = std::max((double)boxes[i].h(),(double)boxes[j].h());
                    hLow = std::min((double)boxes[i].h(),(double)boxes[j].h());

                    if(geometry::do_intersect(boxes[i],boxes[j]))
                        dBin = -dBin;

                    std::map<Var,double> varValue;
                    varValue.insert(std::make_pair(Var("hasWindowPair"),hasWindowPair));
                    varValue.insert(std::make_pair(Var("hasWindowHigh"),hasWindowHigh));
                    varValue.insert(std::make_pair(Var("hasWindowLow"),hasWindowLow));
                    varValue.insert(std::make_pair(Var("dPair"),dBin));
                    varValue.insert(std::make_pair(Var("hHigh"),hHigh));
                    varValue.insert(std::make_pair(Var("hLow"),hLow));

                    if(lot->ruleEnergy(RuleType::DistPair)->isValid(varValue))
                    {
                        std::cout<<"  --dPair"<<i<<"_"<<j<<"="<<dBin<<" hasWindowHigh:"<<hasWindowHigh<<" hasWindowLow:"<<hasWindowLow<<"\n";
                        fs<<"  --dPair"<<i<<"_"<<j<<"="<<dBin<<" hasWindowHigh:"<<hasWindowHigh<<" hasWindowLow:"<<hasWindowLow<<"\n";
                        continue;
                    }


                    isValidDistBin = 0;
                    std::cout<<"  --violation: dPair"<<i<<"_"<<j<<"="<<dBin<<" hasWindowHigh:"<<hasWindowHigh<<" hasWindowLow:"<<hasWindowLow<<"\n";
                    fs<<"  --violation: dPair"<<i<<"_"<<j<<"="<<dBin<<" hasWindowHigh:"<<hasWindowHigh<<" hasWindowLow:"<<hasWindowLow<<"\n";


                }
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

        double dBin,hDiff,hHigh;
        for(size_t i=0;i<boxes.size()-1;++i)
        {
            for(size_t j=i+1;j<boxes.size();++j)
            {
                dBin = boxes[i].distance2cuboid(boxes[j]);
                hHigh = std::max((double)boxes[i].h(),(double)boxes[j].h());

                if(geometry::do_intersect(boxes[i],boxes[j]))
                    dBin = -dBin;

                if(dBin>=hHigh)
                    continue;

                hDiff = std::abs(boxes[i].h()-boxes[j].h());
                std::map<Var,double> varValue;
                varValue.insert(std::make_pair(Var("hDiff"),hDiff));


                if(lot->ruleEnergy(RuleType::HeightDiff)->isValid(varValue))
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

    double lcr = area/lot->area(), far = areaF/lot->area();
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
}

#endif // BLDG_INSPECTOR_HPP_INCLUDED
