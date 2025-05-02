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
//    CrapsBet bet("Player1", BetName::PassLine, 100, 0);
//    std::cout << bet << std::endl;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsTable::addPlayer(const Gen::Uuid& playerId, Gen::ErrorPass& ep)
{
    if (containsUuid(playerId))
    {
        ep.diag = "Unable to add Player to table. "
            "Player is already joined.";
        return Gen::ReturnCode::Fail;
    }
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
    if (removeUuid(playerId, ep) == Gen::ReturnCode::Fail)
    {
        ep.prepend("Unable to remove player. ");
        return Gen::ReturnCode::Fail;
    }
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsTable::addBet(std::shared_ptr<CrapsBet> bet, Gen::ErrorPass& ep)
{
    if (!betAllowed(*bet, ep))
    {
        ep.prepend("This bet is disallowed. ");
        return Gen::ReturnCode::Fail;
    }
    tableBets_[static_cast<size_t>(bet->betName())].push_back(std::move(bet));
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

bool
CrapsTable::betAllowed(const CrapsBet& bet, Gen::ErrorPass& ep) const
{
    if (!bettingOpen_)
    {
        ep.diag = "Betting is closed at the moment - dice roll is underway.";
        return false;
    }
    if (!containsUuid(bet.playerId()))
    {
        ep.diag = "Player is not joined with this table.";
        return false;
    }
    if (bet.betName() == BetName::Come || bet.betName() == BetName::DontCome)
    {
        if (point_ == 0)
        {
            ep.diag = "Betting Come or DontCome is not allowed during come out roll.";
            return false;
        }
    }
    if (bet.betName() == BetName::DontPass && point_ != 0)
    {
        ep.diag = "Betting DontPass is not allowed after a point is established.";
        return false;
    }
    return true;
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

Gen::ReturnCode
CrapsTable::addOdds(std::shared_ptr<CrapsBet> bet, Money oddsAmount, Gen::ErrorPass& ep)
{
    if (!bettingOpen_)
    {
        ep.diag = "Unable to add odds. Betting is closed at the moment.";
        return Gen::ReturnCode::Fail;
    }
    if (!containsUuid(bet->playerId()))
    {
        ep.diag = "Unable to add odds. Player is not joined with this table.";
        return Gen::ReturnCode::Fail;
    }
    if (!containsBet(bet))      //  Is this bet instance on the table?
    {
        ep.diag = "Unable to add odds. This bet instance is not on the table.";
        return Gen::ReturnCode::Fail;
    }
    
    return bet->setOddsAmount(oddsAmount, ep);
}

//----------------------------------------------------------------

void
CrapsTable::rollDice()
{
    bettingOpen_ = false; // No more bets
    dice_.roll();
    // announce outcome
    // 7-out, point,
    resolveBets();
    advanceState();      // Update point, update shooter, update stats
    bettingOpen_ = true; // Betting allowed
}

//----------------------------------------------------------------
//
// update point, update shooter, update stats
//
void
CrapsTable::advanceState()
{
#if 0
    if (point_ == 0)
    {
        if d = [ 4, 5, 6, 8 9 10 ]
            point_ - d;
            move puck
    }
    else
    {
        if d = 7
        {
            7-out
            clear puck
            advance shooter
        }
    }
#endif    
}


//----------------------------------------------------------------

bool
CrapsTable::containsBet(const BetPtr bet) const
{
    // Process all bets
    for (size_t i = 0; i < tableBets_.size(); ++i)
    {
        auto& bets = tableBets_[i];
        auto it = std::find(bets.begin(), bets.end(), bet);
        if (it != bets.end())
        {
            return true;
        }
    }
    return false;
}

//----------------------------------------------------------------
//
// Resolves bets for current roll
//
void
CrapsTable::resolveBets()
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
    ep.diag = "No player with UUID:\"" + id + "\".";
    return Gen::ReturnCode::Fail;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsTable::updatePlayerId(const Gen::Uuid& oldId,
                           const Gen::Uuid& newId,
                           Gen::ErrorPass& ep)
{
    auto it = std::find(players_.begin(), players_.end(), oldId);
    if (it != players_.end())
    {
        *it = newId;
        return Gen::ReturnCode::Success;
    }
    (void) ep; // TODO: fill out ep
    return Gen::ReturnCode::Fail;
}

//----------------------------------------------------------------

std::vector<Gen::Uuid>
CrapsTable::getPlayers() const
{
    std::vector<Gen::Uuid> v;
    for (const auto& id : players_)
    {
        v.push_back(id);
    }
    return v;
}

//----------------------------------------------------------------

unsigned
CrapsTable::getNumPlayers() const
{
    return players_.size();
}

//----------------------------------------------------------------

// Returns current point, or 0 if in come-out

unsigned
CrapsTable::getPoint() const
{
    return point_;
}

//----------------------------------------------------------------

Gen::Uuid
CrapsTable::getIdShooter() const
{
    return shooterId_;
}

//----------------------------------------------------------------

Dice
CrapsTable::getLastRoll() const
{
    return dice_;
}

//----------------------------------------------------------------

bool
CrapsTable::isComeOutRoll() const
{
    return point_ == 0;
}

//----------------------------------------------------------------

bool
CrapsTable::isBettingOpen() const
{
    return bettingOpen_;
}

//----------------------------------------------------------------
