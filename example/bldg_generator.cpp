#include "bldg_generator.hpp"

#include "rjmcmc/param/parameter.hpp"
typedef parameters< parameter > Param;
#include "rjmcmc/param/parameters_inc.hpp"
#include "bldg_generator_para.hpp"
#include <ctime>
#include <chrono>
#include "properties.hpp"
#include "buildup/io/file.hpp"

configuration* bldg_generator(Param& p, Lot* lot,std::ofstream& ofs_energy,std::ofstream& ofs_geom,int& iter)
{
    double eRej = p.get<double>("erej");
   /*** configuration ***/
    configuration* conf = new configuration(
                         p.get<double>("wdborder")*unary_dBorder(lot,eRej)
                        ,p.get<double>("wdpair")*binary_dPair(lot,eRej)
                        ,p.get<double>("whdiff")*binary_hDiff(lot,eRej)
                        ,p.get<double>("wlcr")*global_lcr(lot,eRej)
                        ,p.get<double>("wfar")*global_far(lot,eRej)
                        );

    /*** sampler ***/
    Point_2 p1(lot->xMin(),lot->yMin()); //bottom left point of the bounding box
    Point_2 p2(lot->xMax(),lot->yMax()); //upper right point


    double sampRate = 0.5;
    Center_Functor fCenter(lot,sampRate);
    Width_Functor fWidth(lot,sampRate);
    Theta_Functor fTheta(lot);
    Length_Functor fLength(lot,sampRate);
    Height_Functor fHeight(lot);

    Variate_Center vC(fCenter);
    Variate_Width vW(fWidth);
    Variate_Length vL(fLength);
    Variate_Theta vT(fTheta);
    Variate_Height vH(fHeight);
    Predicate predicate(lot->polygon());
    VariateAll vAll(vC,vW,vL,vT,vH,predicate);
    Birth birth(vAll);

    distribution cs(lot->nBldgMax());
    d_sampler ds( cs, birth );

    sampler samp( ds, acceptance()
                , marked_point_process::make_uniform_birth_death_kernel(birth, 1., p.get<double>("pbirth"))
               // , marked_point_process::make_uniform_kernel<object,1,1>(cuboid_edge_translation_transform<>(),0.2)
               // , marked_point_process::make_uniform_kernel<object,1,1>(cuboid_height_scaling_transform<>(lot->ruleGeom()->hMin(),lot->ruleGeom()->hMax(),lot->ruleGeom()->hFloor()),0.2)
                );


    simulated_annealing::geometric_schedule<double> schedule(p.get<double>("temp"),p.get<double>("deccoef"));
    simulated_annealing::max_iteration_end_test end1(p.get<int>("nb_iter"));
    simulated_annealing::max_num_end_test end2(lot->nBldgMax(),p.get<int>("nb_duration"));

    simulated_annealing::composite_end_test<simulated_annealing::max_iteration_end_test
    ,simulated_annealing::max_num_end_test> endtest(end1,end2);


//    simulated_annealing::energy_visitor eVisitor(p.get<int>("nb_save_energy"),ofs_energy);
//    simulated_annealing::geom_visitor gVisitor(p.get<int>("nb_save_geom"),ofs_geom,lot->invTransX(),lot->invTransY());
//    simulated_annealing::composite_visitor< simulated_annealing::energy_visitor,simulated_annealing::geom_visitor> visitor(eVisitor,gVisitor);

    rjmcmc::mt19937_generator& engine = rjmcmc::random();
    /*< This is the way to launch the optimization process. Here, the magic happen... >*/
    iter = simulated_annealing::optimize(engine,*conf,samp,schedule,endtest);
    return conf;

}

void bldg_generator_info(Lot* lot,std::string& outfile
, std::ofstream& ofs_energy,std::ofstream& ofs_geom,std::ofstream& ofs_stat
,double& lcr,double& far,double& e,double& time)
{
    Param param;
    initialize_parameters(&param);
    int iter=-1;

    std::clock_t c_start = std::clock();
    configuration* conf = bldg_generator(param,lot,ofs_energy,ofs_geom,iter);
    std::clock_t c_end = std::clock();
    time = (double)(1000.0*(c_end-c_start)/CLOCKS_PER_SEC);

    std::cout<<" CPU time "<<time<<" ms\n";
    ofs_stat<<" CPU time "<<time<<" ms\n";

    std::cout<<" iteration "<<iter<<" \n";
    ofs_stat<<" iteration "<<iter<<" \n";

    properties(param,*conf,lot,ofs_stat,lcr,far,e);
    //io::config2bldgs(*conf,bldgs,lot->id());
    io::save_config2shp(*conf,outfile.c_str(),lot->id(),lot->invTransX(),lot->invTransY());
    delete conf;
}

