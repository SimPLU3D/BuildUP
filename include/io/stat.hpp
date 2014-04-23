#ifndef IO_STAT_HPP
#define IO_STAT_HPP

#include <fstream>
#include "plu/Lot.hpp"
#include "mpp/energy/plu_unary_energy.hpp"
#include "mpp/energy/plu_binary_energy.hpp"
#include <gdal/ogrsf_frmts.h>

namespace io
{
    bool check_polygon_validity(const OGRGeometry* p)
    {
        if(p->getGeometryType()!= wkbPolygon)
            return false;
        OGRPolygon* ply = (OGRPolygon*) p;

        if(!ply->IsValid())
            return false;
        if(!ply->IsSimple())
            return false;
        if(ply->getExteriorRing()->isClockwise())
        {
//            for(int i=0;i<ply->getExteriorRing()->getNumPoints();++i)
//                std::cout<<ply->getExteriorRing()->getX(i)<<","<<ply->getExteriorRing()->getY(i)<<std::endl;
            return false;
        }
        for(int i=0;i<ply->getNumInteriorRings();++i)
        {
            if(!ply->getInteriorRing(i)->isClockwise())
                return false;
                //std::cout<<"wrong winding: interior ring"<<std::endl;
        }
        return true;
    }


    template<typename Configuration>
    void statistics_test1(const Configuration& c, Lot* lot,std::ofstream& fs,double& ces,double& cos)
    {

        if(c.empty())
        {
            std::cout<<"empty config"<<"\n";
            fs<<"empty config"<<"\n";
            return ;
        }


        double area=0,areaF=0,hF = lot->_rule._hFloor;
        double dFront,dBack,dSide1,dSide2,dFront_obj;
        double wBand = 20;
        int i = -1;

        typename Configuration::const_iterator it;
        for(it=c.begin();it!=c.end();++it)
        {
            i++;
            std::cout<<"bldg "<<i<<"\n";
            fs<<"bldg "<<i<<"\n";

            //height
            std::cout<<"  ruleHeight: [6,18]\n";
            std::cout<<"    --h = "<<c[it].h<<"\n";
            fs<<"  ruleHeight: [6,18]\n";
            fs<<"   --h = "<<c[it].h<<"\n";

            area+=c[it].area();
            areaF+=c[it].area()*(int)(c[it].h/hF);

            std::map<BorderType,std::map<int,double> > dist_border;
            lot->dist2borders(c[it],dist_border);

            dFront = dist_border[FrontBorder][0];
            dBack = dist_border[BackBorder][0];
            dSide1 = dist_border[SideBorder][0];
            dSide2 = dist_border[SideBorder][1];


            //front distance
            std::cout<<"  ruleFront: d>3\n";
            std::cout<<"   --distFront = "<<dFront<<"\n";

            fs<<"  ruleFront: d>3\n";
            fs<<"   --distFront = "<<dFront<<"\n";

            //side distance
            double d_h = 6>=c[it].h?6:c[it].h;

//            dFront_obj = c[it].distance2line_obj(lot->getRefSeg());
//            if(dFront_obj<=wBand || dFront<wBand/2)
//            {
//                std::cout<<"  ruleSide: band 1: d1=0&&d2=0 || d1=0&&d2>="<<d_h<<" || d1>="<<d_h<<"&&d2=0\n";
//                fs<<"  ruleSide: band 1: d1=0&&d2=0 || d1=0&&d2>="<<d_h<<" || d1>="<<d_h<<"&&d2=0\n";
//            }
//
//            else
//            {
//                std::cout<<"  ruleSide: band 2: d1=0&&d2=0 || d1>="<<d_h<<"&&d2>="<<d_h<<"\n";
//                fs<<"  ruleSide: band 2: d1=0&&d2=0 || d1>="<<d_h<<"&&d2>="<<d_h<<"\n";
//            }

            std::cout<<"  ruleSide: d1=0&&d2=0 || d1=0&&d2>="<<d_h<<" || d1>="<<d_h<<"&&d2=0\n";
            fs<<"  ruleSide: d1=0&&d2=0 || d1=0&&d2>="<<d_h<<" || d1>="<<d_h<<"&&d2=0\n";

            std::cout<<"   --dSide1 = "<<dSide1<<"\n";
            std::cout<<"   --dSide2 = "<<dSide2<<"\n";

            fs<<"   --dSide1 = "<<dSide1<<"\n";
            fs<<"   --dSide2 = "<<dSide2<<"\n";

            //back distance
            std::cout<<"  ruleBack: d>=4\n";
            std::cout<<"   --dBack = "<<dBack<<"\n";

            fs<<"  ruleBack: d>=4\n";
            fs<<"   --dBack = "<<dBack<<"\n";

        }

        //pairwise distance
        std::cout<<"ruleDistPair: d>4\n";
        fs<<"ruleDistPair: d>4\n";

        typename Configuration::const_iterator it2;
        double dBin = -1;
        int isValidDistBin = 1;
        for(it=c.begin();it!=c.end();++it)
        {
            it2=it;
            ++it2;
            for(;it2!=c.end();++it2)
            {
                dBin = std::sqrt(c[it].squared_distance(c[it2]));

                if(geometry::do_intersect(c[it],c[it2]))
                    dBin = -dBin;

                std::cout<<"  --"<<dBin<<"\n";
                fs<<"  --"<<dBin<<"\n";

                if(!(dBin>4))
                {
                    isValidDistBin = 0;

                    std::cout<<"  --violation: d="<<dBin<<"\n";
                    fs<<"  --violation: d="<<dBin<<"\n";

                }

            }

        }
        if(isValidDistBin)
        {
            std::cout<<"  --no violation\n";
            fs<<"  --noViolation\n";
        }

        //ces
        ces = area/lot->_area, cos = areaF/lot->_area;

        std::cout<<"ruleCes: <=0.6\n";
        std::cout<<"  --ces = "<<ces<<"\n";
        std::cout<<"ruleCos: <=3\n";
        std::cout<<"  --cos = "<<cos<<"\n";

        fs<<"ruleCes: <=0.6\n";
        fs<<"  --ces = "<<ces<<"\n";
        fs<<"ruleCos: <=3\n ";
        fs<<"  --cos = "<<cos<<"\n";

        std::cout<<"energy "<<c.energy()<<" eBorder "<<c.energy_dBorder()<<" ePair "<<c.energy_dPair()<<" eCes "<<c.energy_ces()<<" eCos "<<c.energy_cos()<<"\n";
        std::cout<<"audit  "<<c.audit_energy()<<" eBorder "<<c.audit_dBorder()<<" ePair "<<c.audit_dPair()<<" eCes "<<c.audit_ces()<<" eCos "<<c.audit_cos()<<"\n";

        fs<<"energy "<<c.energy()<<" eBorder "<<c.energy_dBorder()<<" ePair "<<c.energy_dPair()<<" eCes "<<c.energy_ces()<<" eCos "<<c.energy_cos()<<"\n";
        fs<<"audit  "<<c.audit_energy()<<" eBorder "<<c.audit_dBorder()<<" ePair "<<c.audit_dPair()<<" eCes "<<c.audit_ces()<<" eCos "<<c.audit_cos()<<"\n";
 }

    template<typename Configuration>
    void statistics_test2(const Configuration& c, Lot* lot,std::ofstream& fs,double& ces,double& cos)
    {

        if(c.empty())
        {
            std::cout<<"empty config"<<"\n";
            fs<<"empty config"<<"\n";
            return ;
        }

        double area=0,areaF=0,hF = lot->_rule._hMin;
        double dFront1,dFront2,dSide1,dSide2;

        int i = -1;

        typename Configuration::const_iterator it;
        for(it=c.begin();it!=c.end();++it)
        {
            i++;
            std::cout<<"bldg "<<i<<"\n";
            fs<<"bldg "<<i<<"\n";

            //height
            std::cout<<"  ruleHeight: [3.2,24]\n";
            std::cout<<"    --h = "<<c[it].h<<"\n";
            fs<<"  ruleHeight: [3.2,24]\n";
            fs<<"   --h = "<<c[it].h<<"\n";

            area+=c[it].area();
            areaF+=c[it].area()*(int)(c[it].h/hF);

            std::map<BorderType,std::map<int,double> > dist_border;
            lot->dist2borders(c[it],dist_border);

            dFront1 = dist_border[FrontBorder][0];
            dFront2 = dist_border[FrontBorder][2];
            dSide1 = dist_border[SideBorder][0];
            dSide2 = dist_border[SideBorder][1];

            //front distance
            std::cout<<"  ruleFront: d1=0 || d2=0 \n";
            std::cout<<"   --dFront1 = "<<dFront1<<"\n";
            std::cout<<"   --dFront2 = "<<dFront2<<"\n";

            fs<<"  ruleFront: d1=0 || d2=0 \n";
            fs<<"   --dFront1 = "<<dFront1<<"\n";
            fs<<"   --dFront2 = "<<dFront2<<"\n";

            //side distance

            double d_h = std::max((c[it].h-3.0)*0.5,3.0);

            std::cout<<"  ruleSide: d1>="<<d_h<<" && d2>="<<d_h<<"\n";
            std::cout<<"   --dSide1 = "<<dSide1<<"\n";
            std::cout<<"   --dSide2 = "<<dSide2<<"\n";

            fs<<"  ruleSide: d1>="<<d_h<<" && d2>="<<d_h<<"\n";
            fs<<"   --dSide1 = "<<dSide1<<"\n";
            fs<<"   --dSide2 = "<<dSide2<<"\n";


        }

        std::cout<<"\n";
        fs<<"\n";
        //pairwise distance

        typename Configuration::const_iterator it2;
        double hMin, hMax, d1,d2,d3,dBin;
        int isValidDistBin = 1;
        for(it=c.begin();it!=c.end();++it)
        {
            it2=it;
            ++it2;
            for(;it2!=c.end();++it2)
            {
                dBin = std::sqrt(c[it].squared_distance(c[it2]));
                hMin = std::min((double)c[it].h,(double)c[it2].h);
                hMax = std::max((double)c[it].h,(double)c[it2].h);
                d1 = hMax*0.5;
                d2 = std::max(hMax-3,8.0);
                d3 = std::max(hMin-3,8.0);


                if(!(dBin>d1 || dBin>d2 || dBin>d3))
                {
                    isValidDistBin = 0;

                    std::cout<<"ruleDistPair: d>"<<d1<<" || d>"<<d2<<" || d>"<<d3<<"\n";
                    std::cout<<"  --violation: d="<<dBin<<"\n";

                    fs<<"ruleDistPair: d>"<<d1<<" || d>"<<d2<<" || d>"<<d3<<"\n";
                    fs<<"  --violation: d="<<dBin<<"\n";

                }

            }

        }
        if(isValidDistBin)
        {
            std::cout<<"ruleDistPair\n  --no violation\n";
            fs<<"ruleDistPair\n  --noViolation\n";
        }

        //ces
        ces = area/lot->_area, cos = areaF/lot->_area;

        std::cout<<"ruleCes: no rule\n ";
        std::cout<<"  --ces = "<<ces<<"\n";
        std::cout<<"ruleCos: no rule\n";
        std::cout<<"  --cos = "<<cos<<"\n";

        fs<<"ruleCes: no rule\n ";
        fs<<"  --ces = "<<ces<<"\n";
        fs<<"ruleCos: no rule\n ";
        fs<<"  --cos = "<<cos<<"\n";
    }




}//namespace io

#endif // IO_STAT_HPP
