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
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsTable::addPlayer(const Gen::Uuid& playerId, Gen::ErrorPass& ep)
{
    if (containsUuid(playerId)) return Gen::ReturnCode::Success;
    if (players_.size() == MaxPlayers)
    {
        (void) ep;  // TODO fill out ep
        return Gen::ReturnCode::Fail;
    }
    players_.push_back(playerId);
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsTable::removePlayer(const Gen::Uuid& playerId, Gen::ErrorPass& ep)
{
    // TODO: Do something if player has working bets
    return removeUuid(playerId, ep);
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsTable::addBet(std::shared_ptr<CrapsBet> bet, Gen::ErrorPass& ep)
{
    if (!containsUuid(bet->playerId()))
    {
        (void) ep;  // TODO fill out ep. Player has to join table
        return Gen::ReturnCode::Fail;
    }
    // TODO: is bet allowed to be made on table
    tableBets_[static_cast<size_t>(bet->betName())].push_back(std::move(bet));
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsTable::removeBet(const std::shared_ptr<CrapsBet>& bet, Gen::ErrorPass& ep)
{
    (void) ep;
    // TODO not all bets can be removed.
    tableBets_[static_cast<size_t>(bet->betName())].remove(bet);
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

void
CrapsTable::resolveRoll()
{
    // bm_.resolveBets(point, dice);
    
    CrapsBet bet("Player1", BetName::PassLine, 100, 0);
    // Gbl::pPlayerMgr->processDecision(dr);
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

    // Process all Place bets
    for (auto& b : tableBets_[PlaceBetIndex])
    {
        b->setSkipOn();
    }
}

//----------------------------------------------------------------

bool
CrapsTable::containsUuid(const Gen::Uuid& id) const
{
    return std::find(players_.begin(), players_.end(), id) != players_.end();
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsTable::removeUuid(const Gen::Uuid& id, Gen::ErrorPass& ep)
{
    auto it = std::find(players_.begin(), players_.end(), id);
    if (it != players_.end())
    {
        players_.erase(it);
        return Gen::ReturnCode::Success;
    }
    (void) ep;  // TODO fill out ep
    return Gen::ReturnCode::Fail;
}

//----------------------------------------------------------------

std::vector<Gen::Uuid>
CrapsTable::getPlayerList() const
{
    std::vector<Gen::Uuid> v;
    for (const auto& id : players_)
    {
        v.push_back(id);
    }
    return v;
}

//----------------------------------------------------------------

    
