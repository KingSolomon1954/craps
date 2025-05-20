//----------------------------------------------------------------
//
// File: Player.cpp
//
//----------------------------------------------------------------

#include "craps/Player.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include "craps/CrapsBetIntfc.h"
#include "craps/CrapsTable.h"
#include "craps/DecisionRecord.h"
#include "craps/Events.h"
#include "craps/EventManager.h"
#include "craps/PlayerManager.h"

using namespace Craps;

//----------------------------------------------------------------
//
// Constructor.
//
Player::Player()
{
    setupSubscriptions();
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
    setupSubscriptions();
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
    setupSubscriptions();
}

//----------------------------------------------------------------

void
Player::setupSubscriptions()
{
    Gbl::pEventMgr->subscribe<BettingClosed>(
        [this](const BettingClosed&)
        {
            this->onBettingClosed();
        });
    Gbl::pEventMgr->subscribe<BettingOpened>(
        [this](const BettingOpened&)
        {
            this->onBettingOpened();
        });
    Gbl::pEventMgr->subscribe<DiceThrowStart>(
        [this](const DiceThrowStart&)
        {
            this->onDiceThrowStart();
        });
    Gbl::pEventMgr->subscribe<DiceThrowEnd>(
        [this](const DiceThrowEnd&)
        {
            this->onDiceThrowEnd();
        });
    Gbl::pEventMgr->subscribe<AnnounceDiceNumber>(
        [this](const AnnounceDiceNumber& evt)
        {
            this->onAnnounceDiceNumber(evt);
        });
    Gbl::pEventMgr->subscribe<PointEstablished>(
        [this](const PointEstablished& evt)
        {
            this->onPointEstablished(evt);
        });
    Gbl::pEventMgr->subscribe<SevenOut>(
        [this](const SevenOut&)
        {
            this->onSevenOut();
        });
    Gbl::pEventMgr->subscribe<PassLineWinner>(
        [this](const PassLineWinner&)
        {
            this->onPassLineWinner();
        });
    Gbl::pEventMgr->subscribe<NewShooter>(
        [this](const NewShooter& evt)
        {
            this->onNewShooter(evt);
        });
}
                
//----------------------------------------------------------------

Gen::ReturnCode
Player::joinTable(Gen::ErrorPass& ep)
{
    // For now, using a single global craps table.
    if (Gbl::pTable->addPlayer(uuid_, ep) == Gen::ReturnCode::Fail)
    {
        ep.prepend("Player " + name_ + " joining table. ");
        return Gen::ReturnCode::Fail;
    }
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
Player::makeBet(BetName betName,
                Gbl::Money contractAmount,
                unsigned pivot,
                Gen::ErrorPass& ep)
{
    // TODO check sufficient funds first 
    auto pBet = Gbl::pTable->addBet(uuid_, betName, contractAmount, pivot, ep);
    if (pBet == nullptr)
    {
        return Gen::ReturnCode::Fail;
    }
    wallet_.withdraw(contractAmount);
    bets_.push_back(pBet);
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

void
Player::processWin(const DecisionRecord& dr)
{
    assert(dr.win > 0);

    // Obtain pointer to our bet
    auto pBet = findBetById(dr.betId);
    if (pBet == nullptr)
    {
        diagBadBetId("processWin() ", dr.betId);
        assert(false);
        return;
    }

    wallet_.deposit(dr.returnToPlayer);
    wallet_.deposit(dr.win + pBet->contractAmount() + pBet->oddsAmount());
    
    std::cout << name_ << ": processWin(" << pBet->betName() <<
        ") won:" << dr.win << " balance:" << wallet_.getBalance() << "\n";

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

    std::cout << name_ << ": processLose(" << pBet->betName() <<
        ") lost:" << dr.lose << " balance:" << wallet_.getBalance() << "\n";

    // TODO update lose stats before removing bet
    // pBet->startTime - endTime ...

    (void) removeBetByPtr(pBet);  // Done with this bet
}

//----------------------------------------------------------------

void
Player::processKeep(const DecisionRecord& dr)
{
    // Obtain pointer to the bet (for stats and stuff)
    auto pBet = findBetById(dr.betId);
    if (pBet == nullptr)
    {
        diagBadBetId("processKeep() ", dr.betId);
        assert(false);
        return;
    }

    std::cout << name_ << ": processKeep(" << pBet->betName() <<
        ") lost:" << dr.lose << " won:" << dr.win
        << " balance:" << wallet_.getBalance() << "\n";

    assert(dr.lose == 0); assert(dr.win == 0);
    
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
//
// Remove bet by name.
//
Gen::ReturnCode
Player::removeBet(BetName betName, unsigned pivot, Gen::ErrorPass& ep)
{
    auto it = std::remove_if(bets_.begin(), bets_.end(),
        [betName, pivot](const BetIntfcPtr& b)
        {
            return (b->betName() == betName) && (b->pivot() == pivot);
        });
    if (it == bets_.end())
    {
        ep.diag = "not found";
        return Gen::ReturnCode::Fail;
    }

    if (Gbl::pTable->removeBet(*it, ep) == Gen::ReturnCode::Fail)
    {
        ep.prepend("problem");
    }
    bets_.erase(it, bets_.end());  // remove from our list of bets
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------
//
// Returns the amount of money currently bet on the table.
//
unsigned
Player::getAmountOnTable() const
{
    unsigned amount = 0;
    for (auto& b : bets_)
    {
        amount += b->contractAmount() + b->oddsAmount();
    }
    return amount;
}

//----------------------------------------------------------------
//
// Returns number of bets currently on the table.
//
unsigned
Player::getNumBetsOnTable() const
{
    return bets_.size();
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

Gbl::Money
Player::getBalance() const
{
    return wallet_.getBalance();
}

//----------------------------------------------------------------

void
Player::onBettingClosed()
{
    // TODO
    std::cout << name_ << " acknowledges BettingClosed\n";
}

//----------------------------------------------------------------

void
Player::onBettingOpened()
{
    // TODO
    std::cout << name_ << " acknowledges BettingOpen\n";
}

//----------------------------------------------------------------

void
Player::onDiceThrowStart()
{
    // TODO
    std::cout << name_ << " acknowledges DiceThrowStart\n";
}

//----------------------------------------------------------------

void
Player::onDiceThrowEnd()
{
    // TODO
    std::cout << name_ << " acknowledges DiceThrowEnd\n";
}

//----------------------------------------------------------------

void
Player::onAnnounceDiceNumber(const AnnounceDiceNumber& evt)
{
    // TODO
    std::cout << name_ << " acknowledges AnnounceDiceNumber " << evt.val
              << "(" << evt.d1 << "," << evt.d2 << ")\n";
}

//----------------------------------------------------------------

void
Player::onPointEstablished(const PointEstablished& evt)
{
    // TODO
    std::cout << name_ << " acknowledges PointEstablished " << evt.point << "\n";
}

//----------------------------------------------------------------

void
Player::onSevenOut()
{
    // TODO
    std::cout << name_ << " acknowledges SevenOut\n";
}

//----------------------------------------------------------------

void
Player::onPassLineWinner()
{
    // TODO
    std::cout << name_ << " acknowledges PassLineWinner\n";
}

//----------------------------------------------------------------

void
Player::onNewShooter(const NewShooter& evt)
{
    // TODO
    std::cout << name_ << " acknowledges NewShooter " <<
        Gbl::pPlayerMgr->getPlayer(evt.shooterId)->getName() << "\n";
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


