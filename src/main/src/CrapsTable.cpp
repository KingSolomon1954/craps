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
#include "DecisionRecord.h"

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
    if (havePlayer(playerId))
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

/*-----------------------------------------------------------*//**

Creates a bet and adds it to the table.

It is an error if the same bet name already exists for this plapyer.

@param playerId
@param betName
@param contractAmount
@param ep
@return Interface pointer to the CrapsBet

*/
CrapsTable::BetIntfcPtr
CrapsTable::addBet(
    const Gen::Uuid& playerId,
    BetName betName,
    Money contractAmount,
    unsigned pivot,
    Gen::ErrorPass& ep)
{
    std::string diag = "Unable to add bet. ";
    if (!betAllowed(playerId, betName, pivot, ep))
    {
        ep.prepend(diag);
        return nullptr;
    }
    try
    {
        BetIntfcPtr b = std::make_shared<CrapsBet>(playerId, betName, contractAmount, pivot);
        tableBets_[static_cast<size_t>(betName)].push_back(b);
        return b;
    }
    catch (const std::invalid_argument& e)
    {
        ep.diag = diag + e.what();
        return nullptr;
    }
}

//----------------------------------------------------------------

bool
CrapsTable::betAllowed(const Gen::Uuid& playerId,
                       BetName betName,
                       unsigned pivot,
                       Gen::ErrorPass& ep) const
{
    if (!bettingOpen_)
    {
        ep.diag = "Betting is closed at the moment - dice roll is underway.";
        return false;
    }
    if (!havePlayer(playerId))
    {
        ep.diag = "Player is not joined with this table.";
        return false;
    }
    if (betName == BetName::Come || betName == BetName::DontCome)
    {
        if (point_ == 0)
        {
            ep.diag = "Betting Come or DontCome is not allowed during come out roll.";
            return false;
        }
    }
    if (betName == BetName::DontPass && point_ != 0)
    {
        ep.diag = "Betting DontPass is not allowed after a point is established.";
        return false;
    }
    if (haveBet(playerId, betName, pivot))
    {
        ep.diag = "Player XXX has already made this bet.";
        return false;
    }
    return true;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsTable::removeBet(BetIntfcPtr pBet, Gen::ErrorPass& ep)
{
    std::string diag = "Unable to remove bet. ";
    if (!haveBet(pBet))
    {
        ep.diag = diag + "This bet instance is not on the table.";
        return Gen::ReturnCode::Fail;
    }
    if (pBet->betName() == BetName::PassLine ||
        pBet->betName() == BetName::Come)
    {
        if (pBet->pivot() != 0)  // This bet has a point.
        {
            ep.diag = diag + "PassLine bets must remain on table "
                             "until a decision.";
            return Gen::ReturnCode::Fail;
        }
    }
    tableBets_[static_cast<size_t>(pBet->betName())].remove(pBet);
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsTable::addOdds(BetIntfcPtr pBet, Money oddsAmount, Gen::ErrorPass& ep)
{
    std::string diag = "Unable to add odds. ";
    if (!bettingOpen_)
    {
        ep.diag = diag +  "Betting is closed at the moment.";
        return Gen::ReturnCode::Fail;
    }
    if (!havePlayer(pBet->playerId()))
    {
        ep.diag = diag + "Player is not joined with this table.";
        return Gen::ReturnCode::Fail;
    }
    if (!haveBet(pBet))
    {
        ep.diag = diag + "This bet instance is not on the table.";
        return Gen::ReturnCode::Fail;
    }

    // Downcast to concrete class.
    std::shared_ptr<CrapsBet> pConcrete = std::dynamic_pointer_cast<CrapsBet>(pBet);
    if (pConcrete->setOddsAmount(oddsAmount, ep) == Gen::ReturnCode::Fail)
    {
        ep.prepend(diag);
        return Gen::ReturnCode::Fail;
    }
    return Gen::ReturnCode::Success;
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
//
// Given a BetIntfcPtr, dDetermine if we already have the given
// bet on the table.
// 
bool
CrapsTable::haveBet(const BetIntfcPtr bet) const
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
// Determine if we already have the given bet on the table.
// 
bool
CrapsTable::haveBet(const Gen::Uuid& playerId, BetName betName,
                    unsigned pivot) const
{
    auto& bets = tableBets_[static_cast<size_t>(betName)];
    for (auto& b : bets)
    {
        if (playerId == b->playerId() &&
            betName == b->betName() &&
            pivot == b->pivot())
        {
std::cout << "Howie 1 returning true pivot:" << pivot << std::endl;            
            return true;
        }
    }
std::cout << "Howie 2 returning false pivot:" << pivot << std::endl;            
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

    DecisionRecord dr;
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
            b->pivot();
        }
    }

    // Process all bets
    for (size_t i = 0; i < tableBets_.size(); ++i)
    {
        auto& bets = tableBets_[i];
        for (auto& b : bets)
        {
            b->pivot();
        }
    }
    
    // Process all bets
    for (auto name : EnumBetName::enumerators)
    {
        auto& bets = tableBets_[static_cast<size_t>(name)];
        for (auto& b : bets)
        {
            b->pivot();
        }
    }

    // Process all Place bets
    for (auto& b : tableBets_[PlaceBetIndex])
    {
        b->pivot();
    }
}

//----------------------------------------------------------------

bool
CrapsTable::havePlayer(const Gen::Uuid& id) const
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
//
// Returns current point, or 0 if in come-out
//
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
//
// Returns number of bets currently on the table.
//
unsigned
CrapsTable::getNumBets() const
{
    unsigned num = 0;
    // Vist each list of bets
    for (size_t i = 0; i < tableBets_.size(); ++i)
    {
        auto& bets = tableBets_[i];
        num += bets.size();
    }
    return num;
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
