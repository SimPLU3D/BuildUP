#include "buildup/io/file.hpp"
#include "buildup/plu/Lot.hpp"
#include "makerule.hpp"
#include "bldg_generator.hpp"
#include <map>

int main(int argc , char** argv)
{
    std::map<int,Lot> lots;

    io::load_lots_shp("./data/parcelle.shp",lots);
    io::load_borders_shp("./data/bordureSeg.shp",lots);

    int idLot = 96;
    Lot* lot = &(lots.find(idLot)->second);
    //translate parcel coordinates for the sake of computation
    lot->translate(-(lot->xMin()),-(lot->yMin()));

    lot->insert_ruleEnergy(RuleType::DistFront, makeRule_dFront());
    lot->insert_ruleEnergy(RuleType::DistSide, makeRule_dSide());
    lot->insert_ruleEnergy(RuleType::DistBack, makeRule_dBack());
    lot->insert_ruleEnergy(RuleType::DistPair,makeRule_dPair());
    lot->insert_ruleEnergy(RuleType::HeightDiff,makeRule_hDiff());
    lot->insert_ruleEnergy(RuleType::LCR,makeRule_lcr());
    lot->insert_ruleEnergy(RuleType::FAR,makeRule_far());
    lot->set_ruleGeom(makeRuleGeom());
    lot->set_isRectLike(true);

    bldg_generator(lot);

    return 0;
}
