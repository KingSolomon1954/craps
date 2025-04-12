//----------------------------------------------------------------
//
// File: CrapsTable.cpp
//
//----------------------------------------------------------------

#include "CrapsTable.h"
#include "CrapsBet.h"

using namespace App;

/*-----------------------------------------------------------*//**

Constructor

*/
CrapsTable::CrapsTable()
{
    CrapsBet bet(BetName::PassLine, 100, 0);
    unsigned point = 0;
    Dice dice;

    dice.roll();
    bet.evaluate(point, dice);
}

//----------------------------------------------------------------
