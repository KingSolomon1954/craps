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
    CrapsBet bet("Player1", BetName::PassLine, 100, 0);
    std::cout << bet << std::endl;
    unsigned point = 4;
    // dice.roll();
    Dice dice; dice.set(2,2);

    CrapsBet::DecisionRecord dr;
    Gen::ErrorPass ep;    
    if (bet.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success)
    {
        std::cout << dr << std::endl;
    }
    else
    {
        std::cout << ep.diag << std::endl;
    }

    // Process all bets
    for (size_t i = 0; i < static_cast<size_t>(BetName::Count); ++i)
    {
        auto& bets = tableBets_[i];
        for (auto& b : bets)
        {
            b->setSkipOn();
        }
    }

    // Process all bets
    for (size_t i = 0; i < tableBets_.size(); ++i)
    {
        auto& bets = tableBets_[i];
        for (auto& b : bets)
        {
            b->setSkipOn();
        }
    }
    
    // Process all bets
    for (auto name : EnumBetName::enumerators)
    {
        auto& bets = tableBets_[static_cast<size_t>(name)];
        for (auto& b : bets)
        {
            b->setSkipOn();
        }
    }
}

//----------------------------------------------------------------

void
CrapsTable::addPlayer(const Gen::Uuid& playerId)
{
    (void) playerId;
    // Player player& = PlayerManager::getPlayer(playerId);
}

void
CrapsTable::removePlayer(const Gen::Uuid& playerId)
{
    (void) playerId;
}

//----------------------------------------------------------------
