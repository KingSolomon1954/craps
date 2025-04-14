//----------------------------------------------------------------
//
// File: CrapsTable.cpp
//
//----------------------------------------------------------------

#include "CrapsTable.h"

#include <iostream>
#include "gen/ErrorPass.h"
#include "gen/ReturnCode.h"
#include "CrapsBet.h"

using namespace App;

/*-----------------------------------------------------------*//**

Constructor

*/
CrapsTable::CrapsTable()
{
    CrapsBet bet(BetName::PassLine, 100, 0);
    std::cout << bet << std::endl;
    unsigned point = 7;
    Dice dice;

    dice.roll();
    CrapsBet::DecisionRecord dr;
    Gen::ErrorPass ep;    
    if (bet.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success)
    {
        std::cout << "TODO: print decision record " << std::endl;
    }
    else
    {
        std::cout << ep.diag << std::endl;
    }
}

//----------------------------------------------------------------
