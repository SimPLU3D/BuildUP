#ifndef BLDG_GENERATOR_PARA_HPP
#define BLDG_GENERATOR_PARA_HPP

#include "param/parameters_inc.hpp"
#include "plu/Lot.hpp"

template<typename T>
void initialize_parameters(T* p)
{
    p-> caption("Building generation parameters");
    p-> template insert<double>("temp",'t',150,"Initial Temperature");
    p-> template insert<double>("deccoef",'C',0.999999,"Decrease coefficient");
    p-> template insert<int>("nbiter",'I',3000000,"Number of iterations");
//    p->template insert<double>("qtemp",'q',0.5,"Sampler (q) [0;1]");
    p-> template insert<int>("nbdump",'d',3000000,"Number of iterations between each result display");
//    p->template insert<bool>("dosave",'b',false, "Save intermediate results");
    p-> template insert<int>("nbsave_energy",'Se',100,"Number of iterations between each save");
    p-> template insert<int>("nbsave_bldg",'Sb',300,"Number of iterations between each save");
    p-> template insert<double>("poisson",'p',200, "Poisson processus parameter");
    p-> template insert<double>("pbirth",'B',0.6, "Birth probability");
    p-> template insert<double>("pdeath",'D',0.4, "Death probability");

    p-> template insert<double>("erej",'eRej',100, "rejection energy");
    p-> template insert<double>("wces",'ces',20, "ces weight");
    p-> template insert<double>("wcos",'cos',50, "cos weight");
    p-> template insert<double>("wdborder",'borderD',30,"border distance weight");
    p-> template insert<double>("wdpair",'distbi',20, "binary distance weight");
//   p-> template insert<double>("woverlap",'overlap',0, "no overlap weight");

//    p-> template insert<double>("rej_energy_ces",'rjces',200, "rejection energy of ces");
//    p-> template insert<double>("rej_energy_cos",'rjcos',200, "rejection energy of cos");
//    p-> template insert<double>("rej_energy_border_d",'rjborderd',200, "rejection energy of border distance ");
//    p-> template insert<double>("rej_energy_border_h",'rjborderh',200, "rejection energy of border height ");
//    p-> template insert<double>("rej_energy_overlap",'rjoverlap',200,"rejection energy of overlap energy");
//    p-> template insert<double>("rej_energy_dist_bi",'rjdistbi',200, "rejection energy of binary distance ");

}

#endif // BLDG_GENERATOR_PARA_HPP
