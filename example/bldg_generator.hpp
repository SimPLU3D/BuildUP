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
#include "rjmcmc/rjmcmc/energy/energy_operators.hpp"
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


/********** variates ***********/
#include "buildup/plu/Variate.hpp"
#include "buildup/plu/Functor.hpp"
#include "buildup/plu/Predicate.hpp"
typedef Uniform_Discrete_Variate<Center_Functor> Variate_Center;
typedef Weighted_Discrete_Variate<Width_Functor> Variate_Width;
typedef Uniform_Discrete_Variate<Length_Functor> Variate_Length;
typedef Uniform_Discrete_Variate<Theta_Functor> Variate_Theta;
typedef Uniform_Discrete_Variate<Height_Functor> Variate_Height;
typedef VariatePLU<Variate_Center,Variate_Width,Variate_Length,Variate_Theta,Variate_Height,Predicate> VariateAll;

/***** mpp smapler *****/
#include "rjmcmc/mpp/kernel/uniform_birth.hpp"
typedef marked_point_process::object_birth<object,VariateAll> Birth;

#include "buildup/plu/Distribution.hpp"
typedef poisson_distribution_max distribution;

#include "rjmcmc/mpp/direct_sampler.hpp"
typedef marked_point_process::direct_sampler<distribution,Birth> d_sampler;


/***** rjmcmc kernels *****/
#include "rjmcmc/mpp/kernel/uniform_birth_death_kernel.hpp"
typedef marked_point_process::uniform_birth_death_kernel<Birth>::type birth_death_kernel;

//#include "rjmcmc/mpp/kernel/uniform_kernel.hpp"
//#include "buildup/rjmcmc/geometry/transform/cuboid_transform_kernel.hpp"
//typedef marked_point_process::uniform_kernel<object,1,1,cuboid_replacement<Birth> >::type replacement_kernel;

//#include "rjmcmc/mpp/kernel/uniform_kernel.hpp"
//#include "buildup/rjmcmc/geometry/transform/cuboid_transform_kernel.hpp"
//typedef marked_point_process::uniform_kernel<object,1,1,cuboid_edge_translation_transform< >  >::type  edge_kernel;
//typedef marked_point_process::uniform_kernel<object,1,1,cuboid_height_scaling_transform< > >::type  height_kernel;


/***** rjmcmc sampler *****/
#include "rjmcmc/rjmcmc/sampler/sampler.hpp"
#include "rjmcmc/rjmcmc/acceptance/metropolis_acceptance.hpp"
typedef rjmcmc::metropolis_acceptance acceptance;

typedef rjmcmc::sampler<d_sampler,acceptance
        ,birth_death_kernel
        //,replacement_kernel
        //,height_kernel
        > sampler;

/***** simulated_annealing *****/
#include "rjmcmc/simulated_annealing/schedule/geometric_schedule.hpp"

#include "rjmcmc/simulated_annealing/end_test/max_iteration_end_test.hpp"
//#include "rjmcmc/simulated_annealing/end_test/delta_energy_end_test.hpp"
#include "rjmcmc/simulated_annealing/end_test/composite_end_test.hpp"
#include "buildup/rjmcmc/simulated_annealing/end_test/max_num_end_test.hpp"


#include "buildup/rjmcmc/simulated_annealing/visitor/energy_visitor.hpp"
#include "buildup/rjmcmc/simulated_annealing/visitor/geom_visitor.hpp"
#include "rjmcmc/simulated_annealing/visitor/composite_visitor.hpp"

#include "rjmcmc/simulated_annealing/simulated_annealing.hpp"

#include "buildup/plu/Lot.hpp"
#include <fstream>

void bldg_generator_info(Lot*,std::string& outfile
, std::ofstream& ofs_energy,std::ofstream& ofs_geom,std::ofstream& ofs_stat
,double& lcr,double& far,double& e,double& time);
#endif // BLDG_GENERATOR_HPP
