#define USE_OSG

#include "buildup/plu/Lot.hpp"
#include "buildup/plu/Building.hpp"
#include "buildup/plu/Rule.hpp"
#include "buildup/io/file.hpp"

#include "makerule.hpp"
#include "bldg_generator.hpp"

#include <stdio.h>
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>

#ifdef USE_OSG
#include "buildup/io/osg.hpp"
#endif

int launchTest(int,int,std::string&,std::string&);

int main(int argc , char** argv)
{
    std::string filenameLot("parcelle.shp");
    std::string filenameBorder("bordureSeg.shp");

    int iTest, idLot;

    std::cout<<"\nPlease choose test zone:\n";
    std::cout<<"1: ParcelleAV178, La courneuve, 12 avenue Lénine, secteur UAA\n";
    std::cout<<"2: Parcelle540,Ile st denis, quai de l’aéroplane chatelier\n";
    std::cin>>iTest;
    switch (iTest){
    case 1:
        idLot=96;
        break;
    case 2:
        idLot=39;
        break;
    default:
        std::cerr<<"invalid test zone\n";
        exit(1);
    }
    launchTest(iTest,idLot,filenameLot,filenameBorder);


    return 0;
}



int launchTest(int iTest,int idLot,std::string& filenameLot,std::string& filenameBorder)
{
    std::string dataDir("./data/zone" + boost::lexical_cast<std::string>(iTest) );
    std::string fileLot = dataDir + "/" + filenameLot;
    std::string fileBorder = dataDir + "/" + filenameBorder;

    std::map<int,Lot> lots;

    //load parcel and border data from shapefile
    io::load_lots_shp(fileLot.c_str(),lots);
    io::load_borders_shp(fileBorder.c_str(),lots);

    //find the target parcel
    Lot* lot = &(lots.find(idLot)->second);

    //translate parcel coordinates for the sake of computation
    lot->translate(-(lot->xMin()),-(lot->yMin()));
    std::cout<<"area "<<lot->area()<<"\n";

    //add rules for the target parcel
    lot->insert_ruleEnergy(RuleType::DistFront, makeRule_dFront(iTest));
    lot->insert_ruleEnergy(RuleType::DistSide, makeRule_dSide(iTest));
    lot->insert_ruleEnergy(RuleType::DistBack, makeRule_dBack(iTest));
    lot->insert_ruleEnergy(RuleType::DistPair,makeRule_dPair(iTest));
    lot->insert_ruleEnergy(RuleType::HeightDiff,makeRule_hDiff(iTest));
    lot->insert_ruleEnergy(RuleType::LCR,makeRule_lcr(iTest));
    lot->insert_ruleEnergy(RuleType::FAR,makeRule_far(iTest));
    lot->set_ruleGeom(makeRuleGeom(iTest));
    lot->set_isRectLike(isRectLike(iTest));

    //enter number of experiments
    int nExp = -1;
    std::cout<<"\nHow many experiments do you want to run?\n";
    std::cin>>nExp;
    while(nExp<1)
    {
        std::cout<<"\nPlease enter an integer greater than 0?\n";
        std::cin>>nExp;
    }

    //output directories
    std::string outDir("./output");
    if(!boost::filesystem::exists(outDir))
        boost::filesystem::create_directory(outDir);

    outDir = outDir + "/zone"+ boost::lexical_cast<std::string>(iTest);
    if(!boost::filesystem::exists(outDir))
        boost::filesystem::create_directory(outDir);

    if(!boost::filesystem::is_empty(outDir))
    {
        std::cout<<"\nWarning: output directory is not empty. Existing results will be erased.\nPress Enter to continue...\n";
        std::cin.ignore();
        std::cin.get();
        boost::filesystem::remove_all(outDir);
        boost::filesystem::create_directory(outDir);
    }

    //dir to save energy evolution
    std::string dirEnergy(outDir+"/energy");
    if(!boost::filesystem::exists(dirEnergy))
        boost::filesystem::create_directory(dirEnergy);
    //dir to save geometry evolution
    std::string dirBldg(outDir+"/bldgs");
    if(!boost::filesystem::exists(dirBldg))
        boost::filesystem::create_directory(dirBldg);
    ////TODO: copy parcel to this folder

    //dir to save final results
    std::string dirBldgFinal(outDir+"/bldgs_final");
    if(!boost::filesystem::exists(dirBldgFinal))
        boost::filesystem::create_directory(dirBldgFinal);
    ////TODO: copy parcel to this folder


    //file to save statistic data
    const char* fileStat = (outDir+"/statistics.txt").c_str();
    if(boost::filesystem::exists(fileStat))
        remove(fileStat);
    std::ofstream fsStat;
    fsStat.open(fileStat,std::fstream::app);
    //statistics for all experiments
    std::map<int,double> lcrVals,farVals,eVals,times;

    for(int i=0;i<nExp;++i)
    {
        std::cout<<"\n--------------------------------\n";
        std::cout<<"Experiment "<<i<<"\n";

        fsStat<<"\n----------------------------------\n";
        fsStat<<"Experiment "<<i<<"\n";

        //file to save final result
        std::string fileBldgFinal(dirBldgFinal+"/bldgs_final_exp");
        fileBldgFinal = fileBldgFinal + boost::lexical_cast<std::string>(i) + ".shp";

        //filestream to save energy evolution
        std::string fileEnergy(dirEnergy+"/energy_exp");
        fileEnergy = fileEnergy + boost::lexical_cast<std::string>(i) + ".txt";
        std::ofstream fsEnergy;
        fsEnergy.open(fileEnergy.c_str(),std::ofstream::app);

        //filestreal to save geometry evolution
        std::string fileGeom(dirBldg+"/bldgs_exp");
        fileGeom = fileGeom + boost::lexical_cast<std::string>(i) + ".txt";
        std::ofstream fsGeom;
        fsGeom.open(fileGeom.c_str(),std::ofstream::app);

        //launch generator
        bldg_generator_info(lot,fileBldgFinal,fsEnergy,fsGeom,fsStat,lcrVals[i],farVals[i],eVals[i],times[i]);

        fsEnergy.close();
        fsGeom.close();

    }

    //output statistics
    {
        double min_lcr = lcrVals[0], max_lcr = lcrVals[0],avg_lcr = 0.,
        min_far = farVals[0],max_far = farVals[0],avg_far = 0.,
        min_e = eVals[0],max_e = eVals[0],avg_e = 0.,
        min_t = times[0],max_t = times[0],avg_t = 0.;

        std::map<int,double>::iterator it;
        double lcr,far,e,t;
        for(it=lcrVals.begin();it!=lcrVals.end();++it)
        {
            lcr = it->second;
            if(lcr < min_lcr)
                min_lcr = lcr;
            if(lcr > max_lcr)
                max_lcr = lcr;
            avg_lcr += lcr;
        }
        for(it=farVals.begin();it!=farVals.end();++it)
        {
            far = it->second;
            if(far<min_far)
                min_far = far;
            if(far>max_far)
                max_far =far;
            avg_far += far;
        }
        for(it=eVals.begin();it!=eVals.end();++it)
        {
            e = it->second;
            if(e<min_e)
                min_e = e;
            if(e>max_e)
                max_e =e;
            avg_e += e;
        }

        for(it=times.begin();it!=times.end();++it)
        {
            t = it->second;
            if(t<min_t)
                min_t = t;
            if(t>max_t)
                max_t =t;
            avg_t += t;
        }

        avg_lcr /= lcrVals.size();
        avg_far /= farVals.size();
        avg_e /= eVals.size();
        avg_t /= times.size();

        std::cout<<"\n----All experiments-------\n";
        std::cout<<"\nmin LCR "<<min_lcr<<"\n";
        std::cout<<"max LCR "<<max_lcr<<"\n";
        std::cout<<"average LCR "<<avg_lcr<<"\n\n";

        std::cout<<"min FAR "<<min_far<<"\n";
        std::cout<<"max FAR "<<max_far<<"\n";
        std::cout<<"average FAR "<<avg_far<<"\n\n";

        std::cout<<"min energy "<<min_e<<"\n";
        std::cout<<"max energy "<<max_e<<"\n";
        std::cout<<"average energy "<<avg_e<<"\n\n";

        std::cout<<"min time "<<min_t<<"\n";
        std::cout<<"max time "<<max_t<<"\n";
        std::cout<<"average times "<<avg_t<<"\n";


        fsStat<<"\nmin LCR "<<min_lcr<<"\n";
        fsStat<<"max LCR "<<max_lcr<<"\n";
        fsStat<<"average LCR "<<avg_lcr<<"\n\n";

        fsStat<<"min FAR "<<min_far<<"\n";
        fsStat<<"max FAR "<<max_far<<"\n";
        fsStat<<"average FAR "<<avg_far<<"\n\n";

        fsStat<<"min energy "<<min_e<<"\n";
        fsStat<<"max energy "<<max_e<<"\n";
        fsStat<<"average energy "<<avg_e<<"\n\n";

        fsStat<<"min time "<<min_t<<"\n";
        fsStat<<"max time "<<max_t<<"\n";
        fsStat<<"average times "<<avg_t<<"\n";
    }

    fsStat.close();

    #ifdef USE_OSG
    //display results
    //reverse translate
    lot->translate(lot->invTransX(),lot->invTransY());
    std::map<int,std::vector<Building> > exp_bldgs;
    io::load_bldgsFinal_shp(dirBldgFinal,nExp,exp_bldgs);
    io::display(exp_bldgs,lots);

    #endif
    return 0;
}


