//----------------------------------------------------------------
//
// File: Player.cpp
//
//----------------------------------------------------------------

#include "Player.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include "CrapsBet.h"
#include "DecisionRecord.h"

using namespace App;

//----------------------------------------------------------------
//
// Constructor.
//
Player::Player()
{
}

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

    // Obtain pointer to the bet (for stats and stuff)
    auto pBet = findBetById(dr.betId);
    if (pBet == nullptr)
    {
        diagBadBetId("processWin() ", dr.betId);
        assert(false);
        return;
    }

    // TODO update win stats before removing bet
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

    // Obtain pointer to the bet (for stats and stuff)
    auto pBet = findBetById(dr.betId);
    if (pBet == nullptr)
    {
        diagBadBetId("processLose() ", dr.betId);
        assert(false);
        return;
    }

    // TODO update lose stats before removing bet
    // pBet->startTime - endTime ...

    (void) removeBetByPtr(pBet);  // Done with this bet
}

//----------------------------------------------------------------

void
Player::processKeep(const DecisionRecord& dr)
{
    assert((dr.lose == dr.win) == 0);
    // Obtain pointer to the bet (for stats and stuff)
    auto pBet = findBetById(dr.betId);
    if (pBet == nullptr)
    {
        diagBadBetId("processKeep() ", dr.betId);
        assert(false);
        return;
    }

    // TODO
    // maybe the pivot was assigned, if so do auto odds?
    // update stats
}

//----------------------------------------------------------------

void
Player::diagBadBetId(const std::string& funcName, unsigned betId) const
{
    std::string diag =
        "Internal Error: Unable to process decision record. "
        "Player::" + funcName + "cant match "
        "decision record betId against any betId held "
        "in player betList. Bad betId" + std::to_string(betId);
    // TODO: error manager
    std::cerr << diag << std::endl;
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
// Save Player to file
//
bool
Player::saveToFile(const std::string& path) const
{
    std::ofstream out(path);
    if (!out) return false;
    out << toJson().dump(2);
    return true;
}

//----------------------------------------------------------------
//
// Load Player from file
//
bool
Player::loadFromFile(const std::string& path)
{
    std::ifstream in(path);
    if (!in) return false;
    json j;
    in >> j;
    fromJson(j);
    return true;
}

//----------------------------------------------------------------
//
// Convert Player to JSON
//
json Player::toJson() const
{
    return json{
        {"uuid", uuid_},
        {"name", name_},
        {"balance", wallet_.getBalance()}
    };
}

//----------------------------------------------------------------
//
// Convert JSON to Player
//
void
Player::fromJson(const json& j)
{
    uuid_ = j.at("uuid").get<std::string>();
    name_ = j.at("name").get<std::string>();
    // TODO wallet_.balance = j.at("balance").get<int64_t>();
}

//----------------------------------------------------------------

const Gen::Uuid&
Player::getUuid() const
{
    return uuid_;
}

//----------------------------------------------------------------

const std::string&
Player::getName() const
{
    return name_;
}

//----------------------------------------------------------------













#if 0
std::string
Player::serialize() const
{
    return uuid_ + "," + name_ + "," + std::to_string(wallet_.getBalance());
}
#endif

//----------------------------------------------------------------

#if 0
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
#endif

//----------------------------------------------------------------
