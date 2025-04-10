//----------------------------------------------------------------
//
// File: CrapsBetEvaluator.cpp
//
//----------------------------------------------------------------

#include "CrapsBetEvaluator.h"
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
    (void) bet;
    (void) point;
    (void) dice;
    return true;
}

//----------------------------------------------------------------
