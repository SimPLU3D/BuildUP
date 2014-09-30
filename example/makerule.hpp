#ifndef MAKERULE_HPP_INCLUDED
#define MAKERULE_HPP_INCLUDED

#include "buildup/plu/Rule.hpp"

RuleEnergy* makeRule_dFront(int iTest);
RuleEnergy* makeRule_dSide(int iTest);
RuleEnergy* makeRule_dBack(int iTest);
RuleEnergy* makeRule_dPair(int iTest);
RuleEnergy* makeRule_hDiff(int iTest);
RuleEnergy* makeRule_lcr(int iTest);
RuleEnergy* makeRule_far(int iTest);
RuleGeom* makeRuleGeom(int iTest);
bool isRectLike(int iTest);

#endif // MAKERULE_HPP_INCLUDED
