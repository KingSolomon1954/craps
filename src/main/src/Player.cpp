//----------------------------------------------------------------
//
// File: Player.cpp
//
//----------------------------------------------------------------

#include "Player.h"
#include <sstream>
#include <fstream>

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
    , balance_(startingBalance)
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
    , balance_(startingBalance)
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
Player::processDecision(const DecisionRecord& dr)
{
    (void) dr;
}

//----------------------------------------------------------------
//
// For saving/loading
//
std::string
Player::serialize() const
{
    return uuid_ + "," + name_ + "," + std::to_string(balance_);
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
