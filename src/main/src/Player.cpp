//----------------------------------------------------------------
//
// File: Player.cpp
//
//----------------------------------------------------------------

#include "Player.h"
#include <cassert>
#include <fstream>
#include <sstream>
#include "CrapsBet.h"
#include "DecisionRecord.h"

using namespace App;

//----------------------------------------------------------------
//
// Constructor.
// 
Player::Player(
    const std::string& name,
    unsigned startingBalance)
    : uuid_(Gen::generateUuid())
    , name_(name)
    , wallet_(startingBalance)
{
    // empty
}

//----------------------------------------------------------------
//
// Constructor.
// 
Player::Player(
    const Gen::Uuid& uuid,
    const std::string& name,
    unsigned startingBalance)
    : uuid_(uuid)
    , name_(name)
    , wallet_(startingBalance)
{
    // empty
}

//----------------------------------------------------------------

#if 0

PlayerManager pm;(global)
    user selects from saved-file players
    Player alice();
    Player john();

main()
    CasinoTables casinoTables(global)
        load tables from file
        user selects table or
        user creates new craps table
        serialize/deserialize tables
    PlayerManager (global)
        load players from file
        user selects player or
        user creates new player
        serialize/deserialize tables
        maintains list of active players

CrapsTable table_ = new selected table;
    BetManager bm_;

alice.joinTable(table);
    table_.addPlayer(alice.uuid);
john.joinTable(table);
    table_.addPlayer(john.uuid);

Player  - constructs CraptBet
    BetPtr bet = createBet(BetName::PassLine, 100);

Player - place bet on table

    Gen::ReturnCode rc = table_.placeBet(BetPtr b, Gen::ErrorPass ep);
    if (rc == Gen::ReturnCode::Fail)
        process ep
    else
        add bet to list of bets
        bm_.addBet(b);

    table.startCountdown()
    user shoots dice
    table.resolveBets();
        bm_.resolveBets(point, dice)
        assert(dr lists are empty)
        for (bet in tableBets_)
            DecisionRecord dr;
            bet.evaluate(point, dice, dr);
            if dr == keep
                add dr to keep list
                no action to bet itself
            if dr == lost
                add dr to lose list
                remove bet from table
            if dr == win
                add dr to win list
                remove bet from table
        endfor
        processLose()
        processWin()
        processKeep()
        for (dr in lose list)
            pm.processDecision(id, dr);
            lookup player by ID
            player.processDecision(const DecisionRecord& dr)
        endfor                                

#endif

//----------------------------------------------------------------

void
Player::processWin(const DecisionRecord& dr)
{
    assert(dr.win > 0);
    wallet_.deposit(dr.returnToPlayer);
    wallet_.deposit(dr.win);
    // TODO update win stats
    auto pBet = findBetById(dr.betId);
    // pBet->startTime - endTime ...
    (void) removeBetByPtr(pBet);
}

//----------------------------------------------------------------

void
Player::processLose(const DecisionRecord& dr)
{
    assert(dr.lose > 0);
    wallet_.deposit(dr.returnToPlayer);
    // Money was already withdrawn from wallet when making the bet
    // TODO update lose stats before removing bet
    auto pBet = findBetById(dr.betId);
    // pBet->startTime - endTime ...
    (void) removeBetByPtr(pBet);
}

//----------------------------------------------------------------

void
Player::processKeep(const DecisionRecord& dr)
{
    assert((dr.lose == dr.win) == 0);
    // TODO
    // maybe pivot assigned, if so do auto odds?
    // update stats
}

//----------------------------------------------------------------
//
// Search for a bet by ID
//
Player::BetIntfcPtr
Player::findBetById(unsigned betId) const
{
    auto it = std::find_if(bets_.begin(), bets_.end(),
                   [betId](const BetIntfcPtr& b)
                   {
                       return b->betId() == betId;
                   });
    if (it != bets_.end())
    {
        return *it;
    }
    return nullptr;
}

//----------------------------------------------------------------

bool
Player::removeBetByPtr(BetIntfcPtr& b)
{
    auto it = std::find(bets_.begin(), bets_.end(), b);
    if (it != bets_.end())
    {
        bets_.erase(it);
        return true;
    }
    return false;
}

//----------------------------------------------------------------

#if 0
bool
Player::removeBetById(unsigned betId)
{
    auto it = std::remove_if(bets_.begin(), bets_.end(),
                   [betId](const BetIntfcPtr& b)
                   {
                       return b->betId() == betId;
                   });
    if (it != bets_.end())
    {
        bets_.erase(it, bets_.end());
        return true;
    }
    return false;
}
#endif

//----------------------------------------------------------------
//
// For saving/loading
//
std::string
Player::serialize() const
{
    return uuid_ + "," + name_ + "," + std::to_string(wallet_.getBalance());
}

//----------------------------------------------------------------

Player
Player::deserialize(const std::string& line)
{
    std::istringstream ss(line);
    std::string uuidStr, nameStr, balanceStr;
    std::getline(ss, uuidStr, ',');
    std::getline(ss, nameStr, ',');
    std::getline(ss, balanceStr);
    return Player(uuidStr, nameStr, std::stol(balanceStr));
}

//----------------------------------------------------------------

const Gen::Uuid&
Player::getUuid() const
{
    return uuid_;
}

//----------------------------------------------------------------
