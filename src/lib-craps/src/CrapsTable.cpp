//----------------------------------------------------------------
//
// File: CrapsTable.cpp
//
//----------------------------------------------------------------

#include <craps/CrapsTable.h>
#include <iostream>
#include <cassert>
#include <controller/Globals.h>
#include <controller/ConfigManager.h>
#include <controller/Events.h>
#include <controller/EventManager.h>
#include <controller/PlayerManager.h>
#include <craps/CrapsBet.h>

using namespace Craps;

/*-----------------------------------------------------------*//**

Private Constructor.

*/
CrapsTable::CrapsTable()
    : tableStats_(tableId_)
    , alltimeStats_(tableId_)
    , houseBank_(InitialStartingBankBalance_, RefillThreshold_, RefillAmount_)
{
}

/*-----------------------------------------------------------*//**

Construct Table from in-memory configuration.

Throws upon error.

*/
CrapsTable*
CrapsTable::fromConfig(const TableId& tableId)
{
    // Not yet implemented
    //
    // see loadFromStrings at end of file

    CrapsTable* ct = new CrapsTable();
    return ct;
}

/*-----------------------------------------------------------*//**

Construct Table from file.

Throws upon error.

*/
CrapsTable*
CrapsTable::fromFile(const TableId& tableId)
{
    std::string filePath = "CrapsTable-" + tableId + ".yaml";

    CrapsTable* ct = new CrapsTable();
    ct->tableId_ = tableId;
    ct->alltimeStats_.tableId = tableId;  // must be set before loading stats
    ct->tableStats_.tableId = tableId;

    // Load alltime stats. Alltime stats come from file.
    // (BTW, current session stats are in-memory only and inited to zero).
    std::string dir =Gbl::pConfigMgr->getString(
        Ctrl::ConfigManager::KeyDirsSysTables).value();
    ct->alltimeStats_.loadFile(dir);

    // Starting table balance picks up where we left off.
    Gen::Money startingBalance =
        ct->alltimeStats_.moneyStats.initialStartingBalance +
        ct->alltimeStats_.moneyStats.amtDeposited           +
        ct->alltimeStats_.moneyStats.amtRefilled            -
        ct->alltimeStats_.moneyStats.amtWithdrawn;

    Bank b(startingBalance, RefillThreshold_, RefillAmount_);
    ct->houseBank_ = b;  // Override default ctor bank values
    ct->tableName_ = "NoName";  // TODO get rid of this. tableId = name
    return ct;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsTable::addPlayer(const Gen::Uuid& playerId, Gen::ErrorPass& ep)
{
    const std::string diag1("Unable to add Player to table. ");
    if (havePlayer(playerId))
    {
        ep.diag = diag1 + "Player is already joined.";
        return Gen::ReturnCode::Fail;
    }
    if (players_.size() == MaxPlayers)
    {
        ep.diag = diag1 + "At max num players " + std::to_string(MaxPlayers) + ".";
        return Gen::ReturnCode::Fail;
    }
    players_.push_back(playerId);
    Gbl::pEventMgr->publish(Ctrl::PlayerJoinedTable{ playerId });
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsTable::removePlayer(const Gen::Uuid& playerId, Gen::ErrorPass& ep)
{
    if (removeUuid(playerId, ep) == Gen::ReturnCode::Fail)
    {
        ep.prepend("Unable to remove player. ");
        return Gen::ReturnCode::Fail;
    }

    // Remove all bets by player, bet money given to the house bank.
    removePlayerBets(playerId);
    Gbl::pEventMgr->publish(Ctrl::PlayerLeftTable{ playerId });
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
    Gen::Money contractAmount,
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
                       unsigned& pivot,
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
        ep.diag = "DontPass is not allowed while there is already a point.";
        return false;
    }
    if (haveBet(playerId, betName, pivot))
    {
        ep.diag = "Player XXX has already made this bet.";
        return false;
    }
    if (betName == BetName::PassLine && point_ != 0)
    {
        // Player made PassLine bet after point already established.
        // Silently coerce the pivot to agree with the point.
        pivot = point_;
    }
    return true;
}

//----------------------------------------------------------------
//
// Change the contract amount of the bet by +/- delta.
//
Gen::ReturnCode
CrapsTable::changeBetAmount(BetIntfcPtr pBet, int delta, Gen::ErrorPass& ep)
{
    int newAmount = pBet->contractAmount() + delta;
    newAmount = std::max(newAmount, 0);
    // Downcast to concrete class.
    std::shared_ptr<CrapsBet> pConcrete = std::dynamic_pointer_cast<CrapsBet>(pBet);
    if (pConcrete->setContractAmount(newAmount, ep) == Gen::ReturnCode::Fail)
    {
        ep.prepend("Unable to change contract bet amount. ");
        return Gen::ReturnCode::Fail;
    }
    return Gen::ReturnCode::Success;
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
            ep.diag = diag + "PassLine|Come bets must remain on table "
                             "until a decision.";
            return Gen::ReturnCode::Fail;
        }
    }
    tableBets_[static_cast<size_t>(pBet->betName())].remove(pBet);
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsTable::setOdds(BetIntfcPtr pBet, unsigned newAmount, Gen::ErrorPass& ep)
{
    std::string diag = "Unable to make odds bet. ";
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
    if (pConcrete->setOddsAmount(newAmount, ep) == Gen::ReturnCode::Fail)
    {
        ep.prepend(diag);
        return Gen::ReturnCode::Fail;
    }
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------
//
// Given a BetIntfcPtr, determine if we already have the given
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
            return true;
        }
    }
    return false;
}

//----------------------------------------------------------------
//
// Find a bet by ID.
//
CrapsBetIntfc*
CrapsTable::findBetById(unsigned betId) const

{
    for (size_t i = 0; i < tableBets_.size(); ++i)
    {
        auto& bets = tableBets_[i];
        for (auto& b : bets)
        {
            if (betId == b->betId())
            {
                return b.get();
            }
        }
    }
    return nullptr;
}

//----------------------------------------------------------------
//
// Suppports unit testing. Not meant for callers.
//
void
CrapsTable::testRollDice(unsigned d1, unsigned d2)
{
    isTestRoll_ = true;
    testRollDice_.set(d1, d2);
    rollDice();
}

//----------------------------------------------------------------
//
// Suppports unit testing. Not meant for callers.
//
void
CrapsTable::testSetState(unsigned point, unsigned d1, unsigned d2)
{
    point_ = point;
    dice_.set(d1, d2);
}

//----------------------------------------------------------------

void
CrapsTable::rollDice()
{
    declareBettingClosed();
    throwDice();
    resolveBets();
    advanceState();      // Update point, update shooter
    tableStats_.recordDiceRoll(point_, dice_);
    declareBettingOpen();
}

//----------------------------------------------------------------

void
CrapsTable::declareBettingClosed()
{
    bettingOpen_ = false; // No more bets
    Gbl::pEventMgr->publish(Ctrl::BettingClosed{});
}

//----------------------------------------------------------------

void
CrapsTable::declareBettingOpen()
{
    bettingOpen_ = true;
    Gbl::pEventMgr->publish(Ctrl::BettingOpened{});
}

//----------------------------------------------------------------

void
CrapsTable::throwDice()
{
    Gbl::pEventMgr->publish(Ctrl::DiceThrowStart{});

    if (isTestRoll_) dice_ = testRollDice_; else dice_.roll();
    std::cout << "point:" << point_ << " dice:" << dice_.value()
              << "(" << dice_.d1() << "," << dice_.d2() << ")\n";
    Gbl::pEventMgr->publish(Ctrl::DiceThrowEnd{});
    Gbl::pEventMgr->publish(Ctrl::AnnounceDiceNumber{dice_.value(), dice_.d1(), dice_.d2()});
}

//----------------------------------------------------------------
//
// Update point, update shooter, update stats
//
void
CrapsTable::advanceState()
{
    if (point_ == 0) // come out roll
    {
        if (CrapsBet::pointNums_.contains(dice_.value()))
        {
            point_ = dice_.value();
            Gbl::pEventMgr->publish(Ctrl::PointEstablished{point_});
        }
    }
    else if (dice_.value() == 7)
    {
        point_ = 0;
        Gbl::pEventMgr->publish(Ctrl::SevenOut{});
        advanceShooter();
    }
    else if (point_ == dice_.value())
    {
        point_ = 0;
        Gbl::pEventMgr->publish(Ctrl::PassLineWinner{});
    }
}

//----------------------------------------------------------------

void
CrapsTable::advanceShooter()
{
    if (players_.empty()) return;

    Gen::Uuid prev = currentShooterId_;

    auto it = std::find(players_.begin(), players_.end(), currentShooterId_);

    // If not found or at the end, start from beginning
    if (it == players_.end() || std::next(it) == players_.end())
    {
        currentShooterId_ = players_.front();
    }
    else
    {
        currentShooterId_ = *std::next(it);
    }

    if (currentShooterId_ != prev)
    {
        Gbl::pEventMgr->publish(Ctrl::NewShooter{currentShooterId_});
    }
}

//----------------------------------------------------------------

void
CrapsTable::resolveBets()
{
    Gbl::pEventMgr->publish(Ctrl::ResolveBetsStart{});
    evaluateBets();
    dispenseResults();
    trimTableBets();
    clearDrl();
    Gbl::pEventMgr->publish(Ctrl::ResolveBetsEnd{});
}

//----------------------------------------------------------------
//
// Visit each bet on the table for a decsion.
//
// Upon returning, the decision results list (DRL) is populated.
//
void
CrapsTable::evaluateBets()
{
    assert(drl_.empty());
    for (size_t i = 0; i < tableBets_.size(); ++i)
    {
        auto& bets = tableBets_[i];
        for (auto& b : bets)
        {
            evalOneBet(b);
        }
    }
}

//----------------------------------------------------------------
//
// Creates a decision record for the given bet and adds it to the DRL.
//
void
CrapsTable::evalOneBet(const BetIntfcPtr pBet)
{
    DecisionRecord dr;
    Gen::ErrorPass ep;

    // Downcast to concrete class.
    std::shared_ptr<CrapsBet> pConcrete = std::dynamic_pointer_cast<CrapsBet>(pBet);
    if (pConcrete->evaluate(point_, dice_, dr, ep) == Gen::ReturnCode::Success)
    {
        drl_.push_back(dr);
        std::cout << dr << std::endl;
    }
    else
    {
        std::cout << ep.diag << std::endl;
    }
}

//----------------------------------------------------------------
//
// Inform Players and Bank of results.
//
// Update dice roll stats here, while decision list is available.
//
void
CrapsTable::dispenseResults()
{
    disburseHouseResults();
    disbursePlayerWins();
    disbursePlayerLoses();
    disbursePlayerKeeps();
}

//----------------------------------------------------------------

void
CrapsTable::disburseHouseResults()
{
    for (const auto& r : drl_)
    {
        if (r.lose > 0)  // player loses, house wins
        {
            houseBank_.deposit(r.lose);
            tableStats_.recordDeposit(r.lose);
        }
        if (r.win > 0)  // player wins, house loses
        {
            tableStats_.recordWithdrawal(r.win);
            Gen::Money amtRefill = houseBank_.withdraw(r.win);
            if (amtRefill > 0)
            {
                tableStats_.recordRefill(amtRefill);
            }
        }
        if (r.commission > 0)
        {
            houseBank_.deposit(r.commission);
        }
    }
}

//----------------------------------------------------------------

void
CrapsTable::disbursePlayerWins()
{
    for (const auto& r : drl_)
    {
        if (r.win > 0)
        {
            Gbl::pPlayerMgr->disburseWin(r);
            CrapsBetIntfc* b = findBetById(r.betId);
            assert(b != nullptr);
            tableStats_.recordWin(*b, r.win);
        }
    }
}

//----------------------------------------------------------------

void
CrapsTable::disbursePlayerLoses()
{
    for (const auto& r : drl_)
    {
        if (r.lose > 0)
        {
            Gbl::pPlayerMgr->disburseLose(r);
            CrapsBetIntfc* b = findBetById(r.betId);
            assert(b != nullptr);
            tableStats_.recordLose(*b, r.lose);
        }
    }
}

//----------------------------------------------------------------

void
CrapsTable::disbursePlayerKeeps()
{
    for (const auto& r : drl_)
    {
        if (!r.decision)
        {
            Gbl::pPlayerMgr->disburseKeep(r);
            CrapsBetIntfc* b = findBetById(r.betId);
            assert(b != nullptr);
            tableStats_.recordKeep(*b);
        }
    }
}

//----------------------------------------------------------------
//
// Remove bets from table that had a decision.
//
// Only trim after dispensing results. Need the bet object in-scope
// when player processes a decision record. A player implementation
// might not be holding their bet pointers, yet still want to access
// the bet. It should be valid for player to lookup bet by id while
// processing decision record.
//
void
CrapsTable::trimTableBets()
{
    for (const auto& dr : drl_)
    {
        if (!dr.decision) continue;

        for (size_t i = 0; i < tableBets_.size(); ++i)
        {
            auto& bets = tableBets_[i];
            if (removeMatchingBetId(bets, dr.betId))
            {
                break;
            }
        }
    }
}

//----------------------------------------------------------------

bool
CrapsTable::removeMatchingBetId(BetList& bets, unsigned betId)
{
    auto it = std::remove_if(bets.begin(), bets.end(),
                   [betId](const BetIntfcPtr& b)
                   {
                       return b->betId() == betId;
                   });
    if (it != bets.end())
    {
        bets.erase(it, bets.end());
        return true;
    }
    return false;
}

//----------------------------------------------------------------
//
// Administrative function to clear out all bets owned by
// a player becasue he left the table. Money for each bet found
// is given to the house, as would occur in a real craps grame.
//
void
CrapsTable::removePlayerBets(const Gen::Uuid& playerId)
{
    for (size_t i = 0; i < tableBets_.size(); ++i)
    {
        removeBetsByPlayerId(tableBets_[i], playerId);
    }
}

//----------------------------------------------------------------
//
// Part 2 of removePlayerBets, this part traverses the
// std::list of bets and removes it if it matches playerId.
// Claims bet money for the house bank.
//
void
CrapsTable::removeBetsByPlayerId(BetList& bets, const Gen::Uuid& playerId)
{
    for (auto it = bets.begin(); it != bets.end(); )
    {
        if (*it && (*it)->playerId() == playerId)
        {
            BetIntfcPtr p = *it;
            houseBank_.deposit(p->contractAmount());
            houseBank_.deposit(p->oddsAmount());
            it = bets.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

//----------------------------------------------------------------
//
// Zero out decision result list.
//
void
CrapsTable::clearDrl()
{
    drl_.clear();
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
    ep.diag = "Unable to update playerId. No such ID:" + oldId;
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
CrapsTable::getShooterId() const
{
    return currentShooterId_;
}

//----------------------------------------------------------------

Dice
CrapsTable::getCurRoll() const
{
    return dice_;
}

//----------------------------------------------------------------
//
// Returns number of bets currently on the table.
//
unsigned
CrapsTable::getNumBetsOnTable() const
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
//
// Returns the amount of money currently bet on the table.
//
Gen::Money
CrapsTable::getAmountOnTable() const
{
    unsigned amount = 0;
    // Vist each list of bets
    for (size_t i = 0; i < tableBets_.size(); ++i)
    {
        auto& bets = tableBets_[i];
        for (auto& b : bets)
        {
            amount += b->contractAmount() + b->oddsAmount();
        }
    }
    return amount;
}

/*-----------------------------------------------------------*//**

Returns read-only access to current session table stats.

@return current session table stats (read-only)

*/
const TableStats&
CrapsTable::getTableStats() const
{
    return tableStats_;
}

/*-----------------------------------------------------------*//**

Returns read-only access to all-time stats.

All-time stats are an aggregation of all sessions played on this table.

Note that all-time stats are only updated when the current session ends.

@return all-time table stats (read-only)

*/
const TableStats&
CrapsTable::getAlltimeStats() const
{
    return alltimeStats_;
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
//
//  Save to files, disable timers, etc
//
void
CrapsTable::prepareForShutdown()
{
    // Create an entry for today's session. CrapsTable is the only class
    // with visibility to alltimeStats_, players and current tableStats_.
    //
    alltimeStats_.sessionHistory.addSessionSummary(
        players_.size(),
        tableStats_.betStats.totNumBetsAllBets,
        tableStats_.moneyStats.amtDeposited,
        tableStats_.moneyStats.amtWithdrawn);
    
    // Merge alltime stats with today's session, then save.
    alltimeStats_.merge(tableStats_);

    // Directory where to read/write table stats.
    std::string dir = Gbl::pConfigMgr->getString(
        Ctrl::ConfigManager::KeyDirsSysTables).value();
    alltimeStats_.saveFile(dir);
}

//----------------------------------------------------------------
//
// Shows various ways to walk tableBets_
//
#if 0
void
CrapsTable::resolveBetsOld()
{
    CrapsBet bet("Player1", BetName::Passine, 100, 0);
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
#endif

//----------------------------------------------------------------
//
// Shows how to load yaml config from strings
//
// loadFromStrings
//
#if 0

#include <yaml-cpp/yaml.h>
#include <string>

int main() {
    std::string input = R"(
config:
  level: debug
  retries: 3
  enabled: true
)";

    YAML::Node root = YAML::Load(input);

    std::cout << "Level: " << root["config"]["level"].as<std::string>() << "\n";
    std::cout << "Retries: " << root["config"]["retries"].as<int>() << "\n";
    std::cout << "Enabled: " << root["config"]["enabled"].as<bool>() << "\n";
}

TEST_CASE("Config loads correctly from string") {
    std::string yaml = R"(
db:
  host: localhost
  port: 5432
)";
    YAML::Node root = YAML::Load(yaml);
    REQUIRE(root["db"]["port"].as<int>() == 5432);
}

// Build it programmatically
YAML::Node node;
node["name"] = "craps";
node["version"] = 1.0;

std::cout << node << "\n";


#endif
