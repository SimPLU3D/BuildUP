#ifndef BLDG_GENERATOR_HPP
#define BLDG_GENERATOR_HPP

/************ geometry ******************/
#include "geometry/geometry.hpp"
#include "geometry/Rectangle_2.hpp"
#include "geometry/Cuboid_bldg.hpp"
typedef geometry::Simple_cartesian<double> K;
typedef K::Point_2 Point_2;
//typedef K::Vector_2 Vector_2;
//typedef K::Segment_2 Segment_2;
typedef geometry::Iso_rectangle_2_traits<K>::type Iso_rectangle_2; //for bounding box
typedef geometry::Rectangle_2<K> Rectangle_2;
typedef geometry::Cuboid_bldg<K> Cuboid_bldg;
typedef Cuboid_bldg object;

#include "geometry/coordinates/Rectangle_2_coordinates.hpp"
#include "geometry/intersection/Rectangle_2_intersection.hpp"

#include "geometry/coordinates/Cuboid_bldg_coordinates.hpp"
#include "geometry/intersection/Cuboid_bldg_intersection.hpp"

/************  energies  ******************/
#include "rjmcmc/energy/energy_operators.hpp"
#include "mpp/energy/plu_unary_energy.hpp"
#include "mpp/energy/plu_binary_energy.hpp"
#include "mpp/energy/plu_global_energy.hpp"
//typedef plu_unary_ces<> unary_ces;
//typedef plu_unary_cos<> unary_cos;
//typedef plu_binary_ces_overlap<> ces_overlap;
//typedef plu_binary_cos_overlap<> cos_overlap;
typedef plu_unary_border_d<> unary_border_d;
//typedef plu_unary_border_h<> unary_border_h;
//typedef plu_binary_intersection<> binary_overlap;
typedef plu_binary_distance<> binary_distance;
typedef plu_global_ces_simple<> global_ces;
typedef plu_global_cos_simple<> global_cos;
//typedef plu_global_ces<> global_ces;
//typedef plu_global_cos<> global_cos;
/************ configuration ******************/
#include "mpp/configuration/graph_configuration.hpp"
typedef marked_point_process::graph_configuration<
        object
        //,multiplies_energy<constant_energy<>,unary_ces>
        //,multiplies_energy<constant_energy<>,unary_cos>
        //,multiplies_energy<constant_energy<>,ces_overlap>
        //,multiplies_energy<constant_energy<>,cos_overlap>
        ,multiplies_energy<constant_energy<>,unary_border_d>
      //  ,multiplies_energy<constant_energy<>,unary_border_h>
      //  ,multiplies_energy<constant_energy<>,binary_overlap>
        ,multiplies_energy<constant_energy<>,binary_distance>
        ,multiplies_energy<constant_energy<>,global_ces>
        ,multiplies_energy<constant_energy<>,global_cos>
        > configuration;

/***** kernels *****/
#include "mpp/kernel/kernel.hpp"
typedef marked_point_process::result_of_make_uniform_birth_death_kernel<object>::type  birth_death_kernel;

#include "rjmcmc/kernel/transform.hpp"
#include "geometry/kernels/cuboid_transform_kernel.hpp"
//typedef marked_point_process::result_of_make_uniform_modification_kernel<cuboid_shortEdge_translation_transform  >::type  shortEdge_modification_kernel;
//typedef marked_point_process::result_of_make_uniform_modification_kernel<cuboid_longEdge_translation_transform  >::type  longEdge_modification_kernel;
typedef marked_point_process::result_of_make_uniform_modification_kernel<cuboid_edge_translation_transform  >::type  edge_modification_kernel;
typedef marked_point_process::result_of_make_uniform_modification_kernel<cuboid_height_scaling_transform>::type  height_modification_kernel;


/***** smaplers *****/
//[building_footprint_rectangle_definition_distribution
#include "rjmcmc/distribution/poisson_distribution.hpp"
typedef rjmcmc::poisson_distribution distribution;
//]

#include "rjmcmc/sampler/sampler.hpp"
#include "plu/Filter.hpp"
typedef marked_point_process::uniform_birth<object> uniform_birth;
typedef marked_point_process::rejection_birth<uniform_birth,Filter> rejection_birth;

#include "mpp/direct_sampler.hpp"
typedef marked_point_process::direct_sampler<distribution,rejection_birth> d_sampler;



#include "rjmcmc/acceptance/metropolis_acceptance.hpp"
typedef rjmcmc::metropolis_acceptance acceptance;

//typedef rjmcmc::sampler<d_sampler,acceptance,birth_death_kernel> sampler;
typedef rjmcmc::sampler<d_sampler,acceptance
        ,birth_death_kernel
        ,edge_modification_kernel
        ,height_modification_kernel
        > sampler;

/***** simulated_annealing *****/
/*< Choice of the schedule (/include/ and /typedef/) >*/
#include "simulated_annealing/schedule/geometric_schedule.hpp"
typedef simulated_annealing::geometric_schedule<double> schedule;
/*< Choice of the end_test (/include/ and /typedef/) >*/
#include "simulated_annealing/end_test/max_iteration_end_test.hpp"
typedef simulated_annealing::max_iteration_end_test     end_test;

#include "simulated_annealing/simulated_annealing.hpp"

#include "rjmcmc/sampler/any_sampler.hpp"
#include "simulated_annealing/visitor/energy_visitor.hpp"
#include "simulated_annealing/visitor/any_visitor.hpp"
//#include "simulated_annealing/visitor/ostream_visitor.hpp"
#include "simulated_annealing/visitor/bldg_visitor.hpp"
//# include "simulated_annealing/visitor/shp/shp_visitor.hpp"

#include "param/parameter.hpp"
typedef parameters< parameter > param;
#include "bldg_generator_para.hpp"

#include "plu/Lot.hpp"
#include "plu/Bldg.hpp"
#include "plu/Filter.hpp"
#include "io/stat.hpp"
#include <ctime>
#include <chrono>
#include <fstream>

configuration* bldg_generator(Lot* lot,std::ofstream& ofs_energy,std::ofstream& ofs_bldg)
{
    param *p = new param;
    initialize_parameters(p);


    double eRej = p->get<double>("erej");
   /*** configuration ***/
    configuration* conf = new configuration(
                        p->get<double>("wdborder")*unary_border_d(lot,eRej)
                        //,0*unary_border_h(lot,eRej)
                        //,p->get<double>("woverlap")*binary_overlap(lot,eRej)
                        ,p->get<double>("wdpair")*binary_distance(lot,eRej)
                        ,p->get<double>("wces")*global_ces(lot,eRej)
                        ,p->get<double>("wcos")*global_cos(lot,eRej)
                        );

    double initCesEnergy = lot->_rule._cesMax * p->get<double>("wces")*eRej;
    double initCosEnergy = p->get<double>("wcos")*eRej;
    conf->init_energy(initCesEnergy,initCosEnergy);


     /*** sampler ***/
    Point_2 p1(lot->_box2d.MinX,lot->_box2d.MinY); //bottom left point of the bounding box
    Point_2 p2(lot->_box2d.MaxX,lot->_box2d.MaxY); //upper right point

    //double maxratio = 10;
    //K::Vector_2 v(10,10);

//    double maxsize = lot->_area;//*lot->_rule._cesMax;
//    double vLen = sqrt(maxsize)/2;
//    K::Vector_2 v(vLen,vLen);
 //   std::cout<<"vLen:"<<vLen<<"\n";


    double maxLength = 80;
    double maxWidth = 25;
    double minWidth = 4;
    double maxRatio = 5;
    double fixTheta = lot->getRefTheta();


    uniform_birth birth(
            Cuboid_bldg(p1,minWidth*0.5,fixTheta, 1/maxRatio,lot->_rule._hMin)
            ,Cuboid_bldg(p2,maxWidth*0.5,fixTheta,maxRatio,lot->_rule._hMax)
    );

    Filter filter(lot);
    rejection_birth rejection(birth,filter);


    distribution cs(p->get<double>("poisson"));

    d_sampler ds( cs, rejection );

    sampler samp( ds, acceptance()
                , marked_point_process::make_uniform_birth_death_kernel(birth, p->get<double>("pbirth"), p->get<double>("pdeath") )
                , marked_point_process::make_uniform_modification_kernel(cuboid_edge_translation_transform(),0.8)
                , marked_point_process::make_uniform_modification_kernel(cuboid_height_scaling_transform(lot->_rule._hMin,lot->_rule._hMax,lot->_rule._hFloor),0.6)
                // , 0.5 * modif2
                );

    schedule sch(p->get<double>("temp"),p->get<double>("deccoef"));
    end_test end(p->get<int>("nbiter"));

    typedef rjmcmc::any_sampler<configuration> any_sampler;
    any_sampler anySampler(samp);

    typedef simulated_annealing::any_composite_visitor<configuration,any_sampler> composite_visitor;
    composite_visitor visitors;


    visitors.push_back(simulated_annealing::energy_visitor(ofs_energy));
    visitors.push_back(simulated_annealing::bldg_visitor(ofs_bldg));
    visitors[0].init(0,p->get<int>("nbsave_energy"));
    visitors[1].init(0,p->get<int>("nbsave_bldg"));

    /*< This is the way to launch the optimization process. Here, the magic happen... >*/
    simulated_annealing::optimize(*conf,anySampler,sch,end,visitors);

//    rjmcmc::any_sampler<configuration> anySampler(samp);
//    simulated_annealing::optimize(*conf,anySampler,sch,end);

    delete p;
    return conf;

}

void bldg_generator_info(Lot* lot,std::ofstream& ofs_energy,std::ofstream& ofs_bldg,std::vector<Bldg>& bldgs,std::ofstream& ofs_stat,double& ces,double & cos)
{
    std::clock_t c_start = std::clock();
    //auto t_start = std::chrono::high_resolution_clock::now();

    configuration* conf = bldg_generator(lot,ofs_energy,ofs_bldg);

    std::clock_t c_end = std::clock();
    //auto t_end = std::chrono::high_resolution_clock::now();

    double t_cpu = (double)(1000.0*(c_end-c_start)/CLOCKS_PER_SEC);
    //double t_wall = std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_start).count();

    std::cout<<" CPU time "<<t_cpu<<" ms\n";
    //std::cout<<" wall time "<<t_wall<<" ms\n";
    ofs_stat<<" CPU time "<<t_cpu<<" ms\n";
    //fs<<" wall time "<<t_wall<<" ms\n";

    io::statistics_test1(*conf,lot,ofs_stat,ces,cos);
    //io::statistics_test2(*conf,lot,ofs_stat,ces,cos);

    io::save2bldg(*conf,lot,bldgs);
    delete conf;
}
#endif // BLDG_GENERATOR_HPP
