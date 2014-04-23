#include "bldg_generator.hpp"
#include "io/shp.hpp"
#include "io/osg.hpp"
#include "io/rule.hpp"
//#include "io/db.hpp"
#include <stdio.h>
#include <string>
#include <boost/lexical_cast.hpp>

#include "plu/Lot.hpp"
#include "plu/Bldg.hpp"
#include "plu/Rule.hpp"


int main(int argc , char** argv)
{

    int iTest = 1;
    int idLot = 96;
    std::string dataPath("./data/zone1/");
    std::string outPath("./output/zone1/test_ed_50_cos/");

////    display intermedium configurations for one experiment
//    io::loadLots( "./output/zone1/parcelleT.shp",lotsDisp);
//    io::loadEvolution( (outPath + "bldgs/bldgs_exp0.txt").c_str(),i_bldgs);
//    io::display(i_bldgs,lotsDisp);


    if(!boost::filesystem::exists(outPath.c_str()))
        boost::filesystem::create_directory(outPath.c_str());

    int nExp = 50;//number of experiments

    std::map<int,Lot> lots;
 //   io::pq_import_lots(lots);

    io::loadLots( (dataPath + "parcelle.shp").c_str(),lots);
 //   io::exportBorderSegs((dataPath + "bordureSeg.shp").c_str(),lots); //manually add border seg type after export
    io::completeBorders((dataPath + "bordureSeg.shp").c_str(),lots);//load border seg info and form borders



    //translate coordinates for the sake of computation
    std::map<int,Lot>::iterator it;
    double MinX = lots.begin()->second._box2d.MinX;
    double MinY = lots.begin()->second._box2d.MinY;
    for(it=lots.begin();it!=lots.end();++it)
    {
        MinX = (it->second._box2d.MinX < MinX) ? it->second._box2d.MinX: MinX;
        MinY = (it->second._box2d.MinY < MinY) ? it->second._box2d.MinY: MinY;
    }

    //translate lots
    for(it=lots.begin();it!=lots.end();++it)
        it->second.translate(MinX,MinY);

    //export translated lots
    //io::exportLots("./output/zone1/parcelleT.shp",lots);

//    //translate roads
//    std::map<int,Road> roads;
//    io::loadRoads("./data/zone1/route.shp",roads);
//    std::map<int,Road>::iterator itRd;
//    for(itRd=roads.begin();itRd!=roads.end();++itRd)
//        itRd->second.translate(MinX,MinY);


    Lot* lot = &(lots[idLot]);
    lot->setRefSeg(true);
 //   lot->_ruleRd = RuleRoad(9.0);

    lot->_rules[RT_DistanceFront] = io::makeRule_dFront(iTest);
    lot->_rules[RT_DistanceSide] = io::makeRule_dSide(iTest);
    lot->_rules[RT_DistanceBack] = io::makeRule_dBack(iTest);
    lot->_rules[RT_DistancePairwise] = io::makeRule_dPair(iTest);
    lot->_rules[RT_CES] = io::makeRule_ces(lot,iTest);
    lot->_rules[RT_COS] = io::makeRule_cos(lot,iTest);
    //lot->_rules[RT_Overlap] = io::makeRule_overlap();
    io::makeRule_height(lot,iTest);


    if(!boost::filesystem::exists(outPath.c_str()))
        boost::filesystem::create_directory(outPath.c_str());

    const char* fileStat = (outPath+"statistics.txt").c_str();
    if(boost::filesystem::exists(fileStat))
        remove(fileStat);

    std::ofstream fs;
    fs.open(fileStat,std::fstream::app);

    std::map<int,std::vector<Bldg> > bldgs_exp;
    std::map<int,double> cesVals;
    std::map<int,double> cosVals;

    for(int i=0;i<nExp;++i)
    {
        std::cout<<"\n----------------------------------------------------------\n";
        std::cout<<"Experiment "<<i<<"\n";

        fs<<"\n----------------------------------------------------------\n";
        fs<<"Experiment "<<i<<"\n";


        std::string pathEnergy(outPath+"energy/");
        if(!boost::filesystem::exists(pathEnergy.c_str()))
            boost::filesystem::create_directory(pathEnergy.c_str());
        std::string fileEnergy(pathEnergy+"energy_exp");
        fileEnergy = fileEnergy + boost::lexical_cast<std::string>(i) + ".txt";
        std::ofstream fsEnergy;
        fsEnergy.open(fileEnergy.c_str(),std::ofstream::app);

        std::string pathBldg(outPath+"bldgs/");
        if(!boost::filesystem::exists(pathBldg.c_str()))
            boost::filesystem::create_directory(pathBldg.c_str());
        std::string fileBldg(pathBldg+"bldgs_exp");
        fileBldg = fileBldg + boost::lexical_cast<std::string>(i) + ".txt";
        std::ofstream fsBldg;
        fsBldg.open(fileBldg.c_str(),std::ofstream::app);


        bldg_generator_info(lot,fsEnergy,fsBldg,bldgs_exp[i],fs,cesVals[i],cosVals[i]);


        fsEnergy.close();
        fsBldg.close();

        //io::display(bldgs_exp[i],lots); //display in 3D for each experiment

        //revert coordinates
        for(int k=0;k<bldgs_exp[i].size();++k)
            bldgs_exp[i][k].translate(-MinX,-MinY);

        //save buildings to shapefile
        std::string pathBldgFinal(outPath+"bldgs_final/");
        if(!boost::filesystem::exists(pathBldgFinal.c_str()))
            boost::filesystem::create_directory(pathBldgFinal.c_str());

        std::string fileBldgFinal(pathBldgFinal+"bldgs_final_exp");
        fileBldgFinal = fileBldgFinal + boost::lexical_cast<std::string>(i) + ".shp";
        io::save_bldgs2shp(fileBldgFinal.c_str(),bldgs_exp[i]);

    }

    double min_ces = cesVals[0], max_ces = cesVals[0], min_cos = cosVals[0],max_cos = cosVals[0];
    double avg_ces = 0.,avg_cos = 0.;
    {
        std::map<int,double>::iterator it1,it2;
        double ces,cos;
        for(it1=cesVals.begin();it1!=cesVals.end();++it1)
        {
            ces = it1->second;
            if(ces < min_ces)
                min_ces = ces;
            if(ces > max_ces)
                max_ces = ces;
            avg_ces += ces;
        }
        for(it2=cosVals.begin();it2!=cosVals.end();++it2)
        {
            cos = it2->second;
            if(cos<min_cos)
                min_cos = cos;
            if(cos>max_cos)
                max_cos =cos;
            avg_cos += cos;
        }
        avg_ces /= cesVals.size();
        avg_cos /= cosVals.size();
    }

    std::cout<<"min ces "<<min_ces<<"\n";
    std::cout<<"max ces "<<max_ces<<"\n";
    std::cout<<"average ces "<<avg_ces<<"\n\n";

    std::cout<<"min cos "<<min_cos<<"\n";
    std::cout<<"max cos "<<max_cos<<"\n";
    std::cout<<"average cos "<<avg_cos<<"\n";

    fs<<"min ces "<<min_ces<<"\n";
    fs<<"max ces "<<max_ces<<"\n";
    fs<<"average ces "<<avg_ces<<"\n\n";

    fs<<"min cos "<<min_cos<<"\n";
    fs<<"max cos "<<max_cos<<"\n";
    fs<<"average cos "<<avg_cos<<"\n";
    fs.close();



    //revert coordinates
    for(it=lots.begin();it!=lots.end();++it)
        it->second.translate(-MinX,-MinY);

    //display final results for all experiments
    io::display(bldgs_exp,lots);

    return 0;
}
