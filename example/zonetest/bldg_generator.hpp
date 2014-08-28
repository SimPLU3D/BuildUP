#ifndef BLDG_GENERATOR_HPP
#define BLDG_GENERATOR_HPP

/************ geometry ******************/
#include "buildup/rjmcmc/geometry/Cuboid.hpp"
#include "buildup/rjmcmc/geometry/coordinates/Cuboid_coordinates.hpp"
#include "buildup/rjmcmc/geometry/intersection/Cuboid_intersection.hpp"
typedef geometry::Simple_cartesian<double> K;
typedef K::Point_2 Point_2;
typedef geometry::Cuboid<K> Cuboid;
typedef Cuboid object;

/************  energies  ******************/
#include <rjmcmc/rjmcmc/energy/energy_operators.hpp>
#include "buildup/rjmcmc/mpp/energy/unary_energy_plu.hpp"
#include "buildup/rjmcmc/mpp/energy/binary_energy_plu.hpp"
#include "buildup/rjmcmc/mpp/energy/global_energy_plu.hpp"

typedef plu_unary_distBorder<> unary_dBorder;
typedef plu_binary_distPair<> binary_dPair;
typedef plu_binary_heightDiff<> binary_hDiff;
typedef plu_global_lcr<> global_lcr;
typedef plu_global_far<> global_far;

/************ configuration ******************/
#include "buildup/rjmcmc/mpp/configuration/graph_configuration_plu.hpp"
typedef marked_point_process::graph_configuration_plu<
        object
        ,multiplies_energy<constant_energy<>,unary_dBorder>
        ,multiplies_energy<constant_energy<>,binary_dPair>
        ,multiplies_energy<constant_energy<>,binary_hDiff>
        ,multiplies_energy<constant_energy<>,global_lcr>
        ,multiplies_energy<constant_energy<>,global_far>
        > configuration;


/***** mpp smapler *****/
//#include "buildup/rjmcmc/mpp/kernel/kernel_plu.hpp"
#include <rjmcmc/mpp/kernel/uniform_birth.hpp>
#include "buildup/rjmcmc/mpp/kernel/rejection_birth.hpp"
#include "buildup/plu/Predicate.hpp"
typedef marked_point_process::uniform_birth<object> uniform_birth;
typedef marked_point_process::rejection_birth<uniform_birth,Predicate> rejection_birth;

#include <rjmcmc/rjmcmc/distribution/poisson_distribution.hpp>
typedef rjmcmc::poisson_distribution distribution;

#include <rjmcmc/mpp/direct_sampler.hpp>
typedef marked_point_process::direct_sampler<distribution,rejection_birth> mpp_sampler;



/***** rjmcmc kernels *****/
#include "buildup/plu/Control.hpp"
#include "buildup/rjmcmc/mpp/kernel/controlled_birth_death_kernel.hpp"
typedef Control_Rho_Theta_H<Variate_rho_discrete,Variate_theta,Variate_height > Control;
typedef marked_point_process::controlled_birth_death_kernel<uniform_birth,Control>::type  controlled_birth_death_kernel;

//#include <rjmcmc/rjmcmc/kernel/transform.hpp>
#include <rjmcmc/mpp/kernel/uniform_kernel.hpp>
#include "buildup/rjmcmc/geometry/kernels/cuboid_transform_kernel.hpp"
typedef marked_point_process::uniform_kernel<object,1,1,cuboid_edge_translation_transform>::type  edge_translation_kernel;
typedef marked_point_process::uniform_kernel<object,1,1,cuboid_height_scaling_transform>::type  height_scaling_kernel;


/***** rjmcmc sampler *****/
#include <rjmcmc/rjmcmc/sampler/sampler.hpp>
#include <rjmcmc/rjmcmc/acceptance/metropolis_acceptance.hpp>
typedef rjmcmc::metropolis_acceptance acceptance;

typedef rjmcmc::sampler<mpp_sampler,acceptance
        ,controlled_birth_death_kernel
        //,edge_translation_kernel
        //,height_scaling_kernel
        > sampler;

/***** simulated_annealing *****/
#include <rjmcmc/simulated_annealing/schedule/geometric_schedule.hpp>

#include <rjmcmc/simulated_annealing/end_test/max_iteration_end_test.hpp>
#include <rjmcmc/simulated_annealing/end_test/composite_end_test.hpp>
#include "buildup/rjmcmc/simulated_annealing/end_test/energy_end_test.hpp"

#include <rjmcmc/simulated_annealing/visitor/composite_visitor.hpp>
#include "buildup/rjmcmc/simulated_annealing/visitor/energy_visitor.hpp"
#include "buildup/rjmcmc/simulated_annealing/visitor/geom_visitor.hpp"

#include <rjmcmc/simulated_annealing/simulated_annealing.hpp>


#include <rjmcmc/param/parameter.hpp>
#include "bldg_generator_para.hpp"

#include "buildup/plu/Lot.hpp"
#include "buildup/plu/Building.hpp"
#include "buildup/io/file.hpp"
#include "properties.hpp"

#include <ctime>
#include <chrono>
#include <fstream>

configuration* bldg_generator(parameters< parameter >& p, Lot* lot
,std::ofstream& ofs_energy,std::ofstream& ofs_geom)
{

    double eRej = p.get<double>("erej");
   /*** configuration ***/
    configuration* config = new configuration(
                         p.get<double>("wdborder")*unary_dBorder(lot,eRej)
                        ,p.get<double>("wdpair")*binary_dPair(lot,eRej)
                        ,p.get<double>("whdiff")*binary_hDiff(lot,eRej)
                        ,p.get<double>("wlcr")*global_lcr(lot,eRej)
                        ,p.get<double>("wfar")*global_far(lot,eRej)
                        );
    config->init_energy(p.get<double>("wlcr")*eRej,p.get<double>("wfar")*eRej);

     /*** sampler ***/
    Point_2 p1(lot->xMin(),lot->yMin()); //bottom left point of the bounding box
    Point_2 p2(lot->xMax(),lot->yMax()); //upper right point

    //TODO: estimate maxRatio according to parcel shape
    double maxRatio = 5;

    uniform_birth uniBirth(
         Cuboid(p1,lot->ruleGeom()->rhoMin(),0,    1,      lot->ruleGeom()->hMin())
        ,Cuboid(p2,lot->ruleGeom()->rhoMax(),M_PI,maxRatio,lot->ruleGeom()->hMax())
    );


    Predicate predicate(lot->polygon());
    rejection_birth rejectionBirth(uniBirth,predicate);
    distribution cs(p.get<double>("poisson"));
    mpp_sampler mppSamp( cs, rejectionBirth );

    Variate_rho_discrete vRho(lot->ruleGeom()->rho());
    Variate_theta vTheta(lot);
    Variate_height vH(lot);
    Control control(vRho,vTheta,vH);

    sampler samp( mppSamp, acceptance()
                , marked_point_process::make_controlled_birth_death_kernel(uniBirth, control, p.get<double>("pbirth"), p.get<double>("pdeath") )
                //, marked_point_process::make_uniform_kernel<object,1,1>(cuboid_edge_translation_transform(),0.8)
                //, marked_point_process::make_uniform_kernel<object,1,1>(height_scaling_transform(lot->_rule._hMin,lot->_rule._hMax,lot->_rule._hFloor),0.6)
                );

    simulated_annealing::geometric_schedule<double> schedule(p.get<double>("temp"),p.get<double>("deccoef"));

    simulated_annealing::max_iteration_end_test end1(p.get<int>("nbiter"));
    simulated_annealing::energy_end_test end2;
    simulated_annealing::composite_end_test<simulated_annealing::max_iteration_end_test,simulated_annealing::energy_end_test> endtest(end1,end2);

    simulated_annealing::energy_visitor eVisitor(p.get<int>("nbsave_energy"),ofs_energy);
    simulated_annealing::geom_visitor gVisitor(p.get<int>("nbsave_geom"),ofs_geom,lot->invTransX(),lot->invTransY());
    simulated_annealing::composite_visitor< simulated_annealing::energy_visitor,simulated_annealing::geom_visitor> visitor(eVisitor,gVisitor);

    rjmcmc::mt19937_generator& engine = rjmcmc::random();

    /*< This is the way to launch the optimization process. Here, the magic happen... >*/
    simulated_annealing::optimize(engine,*config,samp,schedule,endtest,visitor);

    return config;

}



void bldg_generator_info(Lot* lot,std::string& outfile
, std::ofstream& ofs_energy,std::ofstream& ofs_geom,std::ofstream& ofs_stat
,double& lcr,double& far,double& e,double& time)
{
    parameters< parameter > param;
    initialize_parameters(&param);

    std::clock_t c_start = std::clock();
    configuration* conf = bldg_generator(param,lot,ofs_energy,ofs_geom);
    std::clock_t c_end = std::clock();
    time = (double)(1000.0*(c_end-c_start)/CLOCKS_PER_SEC);

    std::cout<<" CPU time "<<time<<" ms\n";
    ofs_stat<<" CPU time "<<time<<" ms\n";

    properties(param,*conf,lot,ofs_stat,lcr,far,e);
    //io::config2bldgs(*conf,bldgs,lot->id());
    io::save_config2shp(*conf,outfile.c_str(),lot->id(),lot->invTransX(),lot->invTransY());
    delete conf;
}
#endif // BLDG_GENERATOR_HPP
