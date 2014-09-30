#ifndef IO_OSG_HPP
#define IO_OSG_HPP

#include "buildup/plu/Lot.hpp"
#include "buildup/plu/Building.hpp"
#include <map>

namespace io
{
    void display(std::map<int,std::vector<Building> >& exp_bldgs, std::map<int,Lot>& lots);
}

#endif // IO_OSG_HPP
