//----------------------------------------------------------------
//
// File: CrapsBetEvaluator.cpp
//
//----------------------------------------------------------------

#include "CrapsBetEvaluator.h"

#include <cassert>
#include "CrapsBet.h"
#include "EnumBetName.h"

using namespace App;

/*-----------------------------------------------------------*//**

Constructor

*/
CrapsBetEvaluator::CrapsBetEvaluator()
{
    Decision dcn = Win;
    (void) dcn;
    // Empty.
}

/*-----------------------------------------------------------*//**


*/
bool
CrapsBetEvaluator::evaluate(CrapsBet& bet, unsigned point, const Dice& dice)
{
    bet.win_ = bet.lose_ = bet.returnToPlayer_ = 0;
    if (!CrapsBetEvaluator::isBetValid(bet)) return false;
                         
    bet.distance_++;
    
    switch (bet.betName_)
    {
    case BetName::PassLine: CrapsBetEvaluator::evalPassLine(bet, point, dice); break;
    default: return true;

    }
    return true;
}

//----------------------------------------------------------------

bool
CrapsBetEvaluator::isBetValid(const CrapsBet& b)
{
    (void) b;
    return true;    
}

//----------------------------------------------------------------

bool
CrapsBetEvaluator::isBoxBetValid(const CrapsBet& b)
{
    assert(b.win_ == 0 && b.lose_ == 0 && b.returnToPlayer_ == 0);
    (void) b;
    return true;    
}

//----------------------------------------------------------------

bool
CrapsBetEvaluator::evalPassLine(
    CrapsBet& bet,
    unsigned point,
    const Dice& dice)
{
    (void) bet;
    (void) point;
    (void) dice;
    return true;
}

//----------------------------------------------------------------
