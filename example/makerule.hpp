#ifndef MAKERULE_HPP_INCLUDED
#define MAKERULE_HPP_INCLUDED

#include "buildup/plu/Rule.hpp"

Rule* makeRule_dFront(int iTest);
Rule* makeRule_dSide(int iTest);
Rule* makeRule_dBack(int iTest);
Rule* makeRule_dPair(int iTest);
Rule* makeRule_hDiff(int iTest);
Rule* makeRule_lcr(int iTest);
Rule* makeRule_far(int iTest);
RuleGeom* makeRuleGeom(int iTest);
bool isRectLike(int iTest);
int makeRule_nMax(int iTest);
double makeRule_lengthHasWindow(int iTest);
#endif // MAKERULE_HPP_INCLUDED
