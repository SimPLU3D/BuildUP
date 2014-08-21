#include "bldg_generator.hpp"
#include "io/file.hpp"
#include "io/display.hpp"
#include "makerule.hpp"
//#include "io/db.hpp"
#include <stdio.h>
#include <string>
#include <boost/lexical_cast.hpp>

#include "Lot.hpp"
#include "Building.hpp"
#include "Rule.hpp"


int launchTest(int iTest,int idLot);

int main(int argc , char** argv)
{
    //zone1 lot96; rectanglelike; width 5, 10
    //zone2 lot39; nonrectanglelike; width 5,10
    //zone3 lot91;
    //zone4 lot23; nonrectanglelike; width 5,10,15

    int option, iTest, idLot;

    std::cout<<"\nPlease choose your option:\n";
    std::cout<<"1: Launch building generation\n";
    std::cout<<"2: Visualize generated results\n";
    std::cout<<"3: Visualize evolution process\n";

    std::cin>>option;
    switch (option){
    case 1:
    {
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
        launchTest(iTest,idLot);
        break;
    }

    case 3:
    {
        std::string file;
        std::cout<<"\nPlease enter the file path:\n";
        std::cin>>file;
        std::cout<<file;
        break;

    }

    default:
        std::cerr<<"invalid option\n";
        exit(1);
    }



////    display intermedium configurations for one experiment
//    std::map<int, Lot > lotsDisp;
//    std::map<int, std::vector<Bldg> > i_bldgs;
//    io::loadLots( "./output/zone4/parcelleT.shp",lotsDisp);
//    io::loadEvolution( (outPath + "bldgs/bldgs_exp0.txt").c_str(),i_bldgs);
//    io::display(i_bldgs,lotsDisp);


    return 0;
}

int launchTest(int iTest,int idLot)
{
    std::string dataPath("./data/zone" + boost::lexical_cast<std::string>(iTest) + "/");
    if(!boost::filesystem::exists(dataPath))
    {
        std::cerr<<"error:data folder doesn't exist\n";
        return 1;
    }

    std::string fileLot = dataPath + "parcelle.shp";
    if(!boost::filesystem::exists(fileLot))
    {
        std::cerr<<"error: shapefile of parcel data doesn't exist\n";
        return 2;
    }

    std::string fileBorder = dataPath + "bordureSeg.shp";
    if(!boost::filesystem::exists(fileBorder))
    {
        std::cerr<<"error: shapefile of border data doesn't exist\n";
        return 3;
    }


    std::map<int,Lot> lots;

    //load parcel and border data from shapefile
    io::loadLots(fileLot.c_str(),lots);
    io::loadBorders(fileBorder.c_str(),lots);

    //translate parcel coordinates for the sake of computation
    std::map<int,Lot>::iterator it;
    double xMin = lots.begin()->second.xMin();
    double yMin = lots.begin()->second.yMin();
    for(it=lots.begin();it!=lots.end();++it)
    {
        xMin = std::min(it->second.xMin(),xMin);
        yMin = std::min(it->second.yMin(),yMin);
    }
    for(it=lots.begin();it!=lots.end();++it)
        it->second.translate(xMin,yMin);


    //add rules for the target parcel
    Lot* lot = &(lots.find(idLot)->second);

    lot->insert_ruleEnergy(RuleType::DistFront, io::makeRule_dFront(iTest));
    lot->insert_ruleEnergy(RuleType::DistSide, io::makeRule_dSide(iTest));
    lot->insert_ruleEnergy(RuleType::DistBack, io::makeRule_dBack(iTest));
    lot->insert_ruleEnergy(RuleType::DistPair,io::makeRule_dPair(iTest));
    lot->insert_ruleEnergy(RuleType::HeightDiff,io::makeRule_hDiff(iTest));
    lot->insert_ruleEnergy(RuleType::LCR,io::makeRule_lcr(iTest));
    lot->insert_ruleEnergy(RuleType::FAR,io::makeRule_far(iTest));
    lot->set_ruleGeom(io::makeRuleGeom(iTest));
    lot->set_isRectLike(io::isRectLike(iTest));


    //enter number of experiments
    int nExp = -1;
    std::cout<<"\nHow many experiments do you want to run?\n";
    std::cin>>nExp;
    while(nExp<1)
    {
        std::cout<<"\nPlease enter an integer greater than 0?\n";
        std::cin>>nExp;
    }


    std::string outPath("./output/");
    if(!boost::filesystem::exists(outPath))
        boost::filesystem::create_directory(outPath);

    outPath = outPath + "zone"+ boost::lexical_cast<std::string>(iTest) + "/";
    if(!boost::filesystem::exists(outPath))
        boost::filesystem::create_directory(outPath);

    if(!boost::filesystem::is_empty(outPath))
    {
        std::cout<<"\nWarning: output directory is not empty. Existing results will be erased.\nPress Enter to continue...\n";
        std::cin.ignore();
        std::cin.get();
        boost::filesystem::remove_all(outPath);
        boost::filesystem::create_directory(outPath);
    }

    //file to save statistic data
    const char* fileStat = (outPath+"statistics.txt").c_str();
    if(boost::filesystem::exists(fileStat))
        remove(fileStat);

    std::ofstream fs;
    fs.open(fileStat,std::fstream::app);

    std::map<int,std::vector<Bldg> > bldgs_exp;
    std::map<int,double> cesVals,cosVals,eVals,times;

    std::cout<<"\n"<<nExp<<" experiment(s) to run ... \n";

    for(int i=0;i<nExp;++i)
    {
        std::cout<<"\n--------------------------------\n";
        std::cout<<"Experiment "<<i<<"\n";

        fs<<"\n----------------------------------\n";
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


        bldg_generator_info(lot,fsEnergy,fsBldg,bldgs_exp[i],fs,cesVals[i],cosVals[i],eVals[i],times[i]);


        fsEnergy.close();
        fsBldg.close();

        //io::display(bldgs_exp[i],lots); //display in 3D for each experiment

        //revert coordinates
        for(size_t k=0;k<bldgs_exp[i].size();++k)
            bldgs_exp[i][k].translate(-xMin,-yMin);

        //save buildings to shapefile
        std::string pathBldgFinal(outPath+"bldgs_final/");
        if(!boost::filesystem::exists(pathBldgFinal.c_str()))
            boost::filesystem::create_directory(pathBldgFinal.c_str());

        std::string fileBldgFinal(pathBldgFinal+"bldgs_final_exp");
        fileBldgFinal = fileBldgFinal + boost::lexical_cast<std::string>(i) + ".shp";
        io::save_bldgs2shp(fileBldgFinal.c_str(),bldgs_exp[i]);

    }


    {
        double min_lcr = cesVals[0], max_lcr = cesVals[0],avg_lcr = 0.,
        min_far = cosVals[0],max_far = cosVals[0],avg_far = 0.,
        min_e = eVals[0],max_e = eVals[0],avg_e = 0.,
        min_t = times[0],max_t = times[0],avg_t = 0.;

        std::map<int,double>::iterator it;
        double lcr,far,e,t;
        for(it=cesVals.begin();it!=cesVals.end();++it)
        {
            lcr = it->second;
            if(lcr < min_lcr)
                min_lcr = lcr;
            if(lcr > max_lcr)
                max_lcr = lcr;
            avg_lcr += lcr;
        }
        for(it=cosVals.begin();it!=cosVals.end();++it)
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

        avg_lcr /= cesVals.size();
        avg_far /= cosVals.size();
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


        fs<<"\nmin LCR "<<min_lcr<<"\n";
        fs<<"max LCR "<<max_lcr<<"\n";
        fs<<"average LCR "<<avg_lcr<<"\n\n";

        fs<<"min FAR "<<min_far<<"\n";
        fs<<"max FAR "<<max_far<<"\n";
        fs<<"average FAR "<<avg_far<<"\n\n";

        fs<<"min energy "<<min_e<<"\n";
        fs<<"max energy "<<max_e<<"\n";
        fs<<"average energy "<<avg_e<<"\n\n";

        fs<<"min time "<<min_t<<"\n";
        fs<<"max time "<<max_t<<"\n";
        fs<<"average times "<<avg_t<<"\n";
    }

    fs.close();

    //revert coordinates
    for(it=lots.begin();it!=lots.end();++it)
        it->second.translate(-xMin,-yMin);

    //display final results for all experiments
    io::display(bldgs_exp,lots);

    return 0;
}


