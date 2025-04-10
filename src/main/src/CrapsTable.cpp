//----------------------------------------------------------------
//
// File: CrapsTable.cpp
//
//----------------------------------------------------------------

#include "CrapsTable.h"
#include "CrapsBet.h"
#include "CrapsBetEvaluator.h"

using namespace App;

/*-----------------------------------------------------------*//**

Constructor

*/
CrapsTable::CrapsTable()
{
    CrapsBet bet;
    unsigned point = 0;
    Dice dice;
    
    CrapsBetEvaluator::evaluate(bet, point, dice);
}

//----------------------------------------------------------------
