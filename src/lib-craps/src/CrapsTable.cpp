//----------------------------------------------------------------
//
// File: CrapsTable.cpp
//
//----------------------------------------------------------------

#include <craps/CrapsTable.h>
#include <cassert>
#include <fstream>
#include <iostream>
#include <cassert>
#include <craps/CrapsBet.h>
#include <craps/Events.h>
#include <gen/FileUtils.h>
#include <gen/Logger.h>

using namespace Craps;

/*-----------------------------------------------------------*//**

Private Constructor.

*/
CrapsTable::CrapsTable(const TableId&     tableId,
                       const TableConfig& config,
                       EventManager&      eventMgr)
    : tableId_(tableId)
    , config_(config)
    , eventMgr_(eventMgr)
    , houseBank_(InitialStartingBankBalance_, RefillThreshold_, RefillAmount_)
{
    alltimeStats_.sessionHistory.setMaxSessions(config_.maxSessions);
}

/*-----------------------------------------------------------*//**

Construct Table from in-memory YAML string.

Throws upon error.

*/
CrapsTable*
CrapsTable::fromString(std::string        yaml,
                       const TableId&     tableId,
                       const TableConfig& config,
                       EventManager&      eventMgr)
{
    CrapsTable* ct = new CrapsTable(tableId, config, eventMgr);
    YAML::Node root = YAML::Load(yaml);
    ct->fromYAML(root);
    return ct;
}

/*-----------------------------------------------------------*//**

Construct Craps table from file.

Throws upon error.

*/
CrapsTable*
CrapsTable::fromFile(const TableId& tableId,
                     const TableConfig& config,
                     EventManager& eventMgr)
{
    CrapsTable* ct = new CrapsTable(tableId, config, eventMgr);
    ct->loadFile();
    return ct;
}

//-----------------------------------------------------------------

void
CrapsTable::saveFile() const
{
    if (config_.tablePath.empty())
    {
        LOG_DEBUG("CrapsTable::saveFile() skipping; tablePath is empty");
        return;
    }

    LOG_DEBUG("CrapsTable::saveFile(" + config_.tablePath.string()  + ")");
    std::ofstream fout(config_.tablePath);
    fout << toYAML();
}

//-----------------------------------------------------------------

void
CrapsTable::loadFile()
{
    try
    {
        std::ifstream fin =
            Gen::FileUtils::openOrThrow(config_.tablePath);  // throws
        YAML::Node root = YAML::Load(fin);
        fromYAML(root);                                      // throws
    }
    catch (const std::runtime_error& e)
    {
        std::string diag("CrapsTable::loadFile(): \"" +
                         config_.tablePath.string() + "\"; ");
        throw std::runtime_error(diag + e.what());
    }
}

//-----------------------------------------------------------------

YAML::Node
CrapsTable::toYAML() const
{
    YAML::Node node;
    node["tableId"]          = tableId_;
    node["tableName"]        = tableName_;
    node["shortDescription"] = shortDescription_;
    node["fullDescription"]  = fullDescription_;

    node["Rules"] = rulesToYAML();
    node["Bank"]  = houseBank_.toYAML();
    alltimeStats_.toYAML(node);
    return node;
}

//-----------------------------------------------------------------

void
CrapsTable::fromYAML(const YAML::Node& node)
{
    auto tableIdInFile = node["tableId"].as<std::string>();
    tableName_         = node["tableName"].as<std::string>();
    shortDescription_  = node["shortDescription"].as<std::string>();
    fullDescription_   = node["fullDescription"].as<std::string>();

    // YAML tableId must agree with id from ctor
    if (tableIdInFile != tableId_)
    {
        throw std::runtime_error("Table ID mismatch: expected " +
                                 tableId_ + ", found " + tableIdInFile);
    }

    rulesFromYAML(node["Rules"]);
    houseBank_.fromYAML(node["Bank"]);
    alltimeStats_.fromYAML(node);
}

//----------------------------------------------------------------

void
CrapsTable::rulesFromYAML(const YAML::Node& node)
{
    maxOdds_     = node["maxOdds"].as<unsigned>();
    minLineBet_  = node["minLineBet"].as<unsigned>();
    maxLineBet_  = node["maxLineBet"].as<unsigned>();
    minPlaceBet_ = node["minPlaceBet"].as<unsigned>();
    maxPlaceBet_ = node["maxPlaceBet"].as<unsigned>();
    minFieldBet_ = node["minFieldBet"].as<unsigned>();
    maxFieldBet_ = node["maxFieldBet"].as<unsigned>();
    minCandEBet_ = node["minCandEBet"].as<unsigned>();
    maxCandEBet_ = node["maxCandEBet"].as<unsigned>();
    minHornBet_  = node["minHornBet"].as<unsigned>();
    maxHornBet_  = node["maxHornBet"].as<unsigned>();
}

//----------------------------------------------------------------

YAML::Node
CrapsTable::rulesToYAML() const
{
    YAML::Node node;
    node["maxOdds"]     = maxOdds_;
    node["minLineBet"]  = minLineBet_;
    node["maxLineBet"]  = maxLineBet_;
    node["minPlaceBet"] = minPlaceBet_;
    node["maxPlaceBet"] = maxPlaceBet_;
    node["minFieldBet"] = minFieldBet_;
    node["maxFieldBet"] = maxFieldBet_;
    node["minCandEBet"] = minCandEBet_;
    node["maxCandEBet"] = maxCandEBet_;
    node["minHornBet"]  = minHornBet_;
    node["maxHornBet"]  = maxHornBet_;
    return node;
}

//----------------------------------------------------------------
//
// Save to files, disable timers, etc
//
// Called when user decides to switch tables, so this one ends.
// Called when exiting program.
//
void
CrapsTable::close()
{
    // Create an entry for today's session. CrapsTable is the only class
    // with visibility to alltimeStats_, players and currentStats_.
    //
    alltimeStats_.sessionHistory.addSessionSummary(
        players_.size(),
        currentStats_.betStats.totNumBetsAllBets,
        houseBank_.getAmtDeposited(),
        houseBank_.getAmtWithdrawn());

    // Merge alltime stats with today's session, then save.
    alltimeStats_.merge(currentStats_);
    houseBank_.mergeStats();
    saveFile();
}

//----------------------------------------------------------------
//
//  Clears out players and bets, save to files,
//  disable timers, etc
//
void
CrapsTable::prepareForShutdown()
{
    // First, clear out players
    auto playersCopy = players_;      // copy of player list
    for (auto& player : playersCopy)
    {
        // Give players opportunity to cleanup on their own
        player->prepareForShutdown(); // will modify original players_
    }

    if (getNumBetsOnTable() > 0)
    {
        // boo boo problem, players did not clean up
        assert(false);
    }
    close();
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsTable::addPlayer(Player* pPlayer, Gen::ErrorPass& ep)
{
    assert(pPlayer != nullptr);
    const std::string diag1("CrapsTable::addPlayer(): Unable to add "
                            "Player " + pPlayer->getName() + " to table; ");
    if (havePlayer(pPlayer))
    {
        ep.diag = diag1 + "Player is already joined.";
        return Gen::ReturnCode::Fail;
    }
    if (players_.size() == MaxPlayers)
    {
        ep.diag = diag1 + "At max num players " +
            std::to_string(MaxPlayers) + ".";
        return Gen::ReturnCode::Fail;
    }
    players_.push_back(pPlayer);
    eventMgr_.publish(PlayerJoinedTable{ pPlayer->getPlayerId() });
    return Gen::ReturnCode::Success;
}

/*-----------------------------------------------------------*//**

Removes a player and his bets from the table.

Any outstanding bets are abandoned meaning funds are claimed by the
house bank and not returned to the player.  If that is not desired
behavior then first remove player's bets with force
(Player::removeBetForce()) ahead of this call.

@param[in] pPlayer
    the player of interest

@param[in,out] ep
    if error, ep has the reason

@return
    Success, otherwise fail and ep holds the reason.
    Fails only if player is not joined to the table.
*/
Gen::ReturnCode
CrapsTable::removePlayer(Player* pPlayer, Gen::ErrorPass& ep)
{
    if (!havePlayer(pPlayer))
    {
        ep.diag = "CrapsTable::removePlayer(); Unable to remove player; " +
                  pPlayer->getName() + ":" + pPlayer->getPlayerId()       +
                  "; Player has not joined this table.";
        return Gen::ReturnCode::Fail;
    }
    
    (void) removePlayerByPtr(pPlayer, ep);  // ignore error if any

    // Remove all bets by player, bet money given to the house bank.
    removePlayerBets(pPlayer);
    eventMgr_.publish(PlayerLeftTable{ pPlayer->getPlayerId() });
    return Gen::ReturnCode::Success;
}

/*-----------------------------------------------------------*//**

Places the bet on the table.

If the bet passes table rules it is placed on the table.

It is an error if the same bet name already exists for this player.
Use setContractAmount() or setOddsAmount() if you need to change the
bet amount or odds.

@param[in] pBet
    the bet of interest
@param[in,out] ep
    if error, ep has the reason
@return
    Success if accepted, otherwise Fail and ep has the reason
*/
Gen::ReturnCode
CrapsTable::addBet(BetPtr pBet, Gen::ErrorPass& ep)
{
    if (!betAllowed(*pBet, ep)) return Gen::ReturnCode::Fail;

    tableBets_[static_cast<size_t>(pBet->betName())].push_back(pBet);
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

bool
CrapsTable::betAllowed(CrapsBet& bet, Gen::ErrorPass& ep) const
{
    // fif prefix means "fault if"

    if (fifBettingClosed     (bet, 1, ep))                       return false;
    if (fifMissingPlayer     (bet, ep))                          return false;
    if (fifHaveBet           (bet, ep))                          return false;
    if (fifZeroAmount        (bet, bet.contractAmount(), 1, ep)) return false;
    if (fifComeDisallowed    (bet, ep))                          return false;
    if (fifDontPassDisallowed(bet, ep))                          return false;
    if (fifBadMinMaxLineBets (bet, bet.contractAmount(), 1, ep)) return false;
    if (fifBadMinMaxSideBets (bet, bet.contractAmount(), 1, ep)) return false;
    if (fifBadMultiples      (bet, bet.contractAmount(), 1, ep)) return false;

    if (bet.betName() == BetName::PassLine && point_ != 0)
    {
        // Player made PassLine bet after point already established.
        // Silently coerce the pivot to agree with the point.
        bet.setPivotInternal(point_);
    }
    return true;
}

//----------------------------------------------------------------

bool
CrapsTable::isBetRemovable(BetPtr pBet) const
{
    if (pBet == nullptr) return false;
    if (!haveBet(*pBet)) return false;
    if ((pBet->distance() > 0))
    {
        if (pBet->betName() == BetName::PassLine || 
            pBet->betName() == BetName::Come)
        {
            return false;
        }
    }
    return true;
}

/*-----------------------------------------------------------*//**

Removes the bet from the table subject to craps table rules.

If successful return, it is expected that Player recovers the bet amount
back into their wallet.

@param[in,out] pBet
    The bet of interest.

@param[in,out] ep
    Holds reason for error

@return
    Success if bet is removed, otherwise Fail and ep has reason.
*/
Gen::ReturnCode
CrapsTable::removeBet(BetPtr pBet, Gen::ErrorPass& ep)
{
    std::string diag = "CrapsTable::removeBet(): Unable to remove bet. ";
    if (!haveBet(*pBet))
    {
        ep.diag = diag + "This bet instance is not on the table.";
        return Gen::ReturnCode::Fail;
    }
    if (!isBetRemovable(pBet))
    {
        ep.diag = diag + "PassLine|Come bets with points must remain "
            "on table until a decision; " + pBet->diagBetId() + ".";
        return Gen::ReturnCode::Fail;
    }
    tableBets_[static_cast<size_t>(pBet->betName())].remove(pBet);
    return Gen::ReturnCode::Success;
}

/*-----------------------------------------------------------*//**

Removes the bet from the table regardless of craps table rules.

Funds associated with the bet are not claimed by the house bank. It is
expected that Player recovers the bet amount into their wallet.  This is
meant to be called upon program shutdown or when player switches table
and player is allowed to recover their bets (instead of abandoning
them).

@param[in,out] pBet
    The bet of interest.

@param[in,out] ep
    Holds reason for error

@return
    Success if bet is removed, otherwise Fail and ep has reason.
    Fails only if bet is not found.
*/
Gen::ReturnCode
CrapsTable::removeBetForce(BetPtr pBet, Gen::ErrorPass& ep)
{
    std::string diag = "CrapsTable::removeBetForce(): Unable to remove bet. ";
    if (!haveBet(*pBet))
    {
        ep.diag = diag + "This bet instance is not on the table.";
        return Gen::ReturnCode::Fail;
    }
    tableBets_[static_cast<size_t>(pBet->betName())].remove(pBet);
    return Gen::ReturnCode::Success;
}

/*-----------------------------------------------------------*//**

Changes the contract amount of a bet on the table.

@li The bet must already exist on the table.
@li Overwrites the previous amount.
@li Validates the change against table rules.

@param[in,out] pBet
    The bet of interest.

@param[in] newAmount
    Sets the contract bet amount to newAmount

@param[in,out] ep
    Holds reason for error

@return
    Success if amount has been changed, otherwise Fail and ep has reason

@internal
@li fif prefix means "fault if"
@li Sca suffix means "set contract amount"
@li 2 is the index to select the right diagPrefix
*/
Gen::ReturnCode
CrapsTable::setContractAmount(BetPtr pBet,
                              Gen::Money newAmount,
                              Gen::ErrorPass& ep)
{
    if (fifBettingClosed    (*pBet, 2, ep))            return Gen::ReturnCode::Fail;
    if (fifMissingBet       (*pBet, 2, ep))            return Gen::ReturnCode::Fail;
    if (fifZeroAmount       (*pBet, newAmount, 2, ep)) return Gen::ReturnCode::Fail;
    if (fifBadPassLineChange(*pBet, newAmount, 2, ep)) return Gen::ReturnCode::Fail;
    if (fifBadDontPassChange(*pBet, newAmount, 2, ep)) return Gen::ReturnCode::Fail;
    if (fifBadMinMaxLineBets(*pBet, newAmount, 2, ep)) return Gen::ReturnCode::Fail;
    if (fifBadMinMaxSideBets(*pBet, newAmount, 2, ep)) return Gen::ReturnCode::Fail;
    if (fifBadMultiples     (*pBet, newAmount, 2, ep)) return Gen::ReturnCode::Fail;

    pBet->setContractAmountInternal(newAmount);
    return Gen::ReturnCode::Success;
}

/*-----------------------------------------------------------*//**

Sets, changes, or removes the amount for an odds bet.

Overwrites the previous amount, if any. A value
of zero removes it altogether.

It is only permissable to set an odds amount if the following
conditions are true:

@li the bet is already on the table
@li the bet is a PassLine, DontPass, Come, or DontCome bet
@li the bet has an assigned pivot (point) (i.e, pivot is non-zero)
@li the new amount is subject to table limits min/max odds

@param[in,out] pBet
    The bet of interest.

@param[in] amount
    The amount to set it to. Clobbers any previous setting.

@param[in,out] ep
    Holds reason for error

@returns
    Success if the bet was accepted, otherwise Fail and ep has
    the reason.

@internal
    soa prefix means "set odds amount"  - need variants for
    diagnostic messages look like this:
    CrapsBet::setOddsAmount(): Unable to set odds bet; bet(betId:157, betName:DontPass(4)). Odds bet amount of $1 is too small. Minimum odds for this bet is 2.
    CrapsBet::setOddsAmount(): Unable to set odds bet; bet(betId:159, betName:DontPass(6)). Exceeds table limit of 5x odds; Contract amount is $1 which allows max odds amount of $5.
*/
Gen::ReturnCode
CrapsTable::setOddsAmount(BetPtr pBet,
                          Gen::Money oddsAmount,
                          Gen::ErrorPass& ep)
{
    if (fifBettingClosed    (*pBet, 3, ep))          return Gen::ReturnCode::Fail;
    if (fifMissingBet       (*pBet, 3, ep))          return Gen::ReturnCode::Fail;
    if (fifBadBetTypeForOdds(*pBet, ep))             return Gen::ReturnCode::Fail;
    if (fifZeroPivotForOdds (*pBet, ep))             return Gen::ReturnCode::Fail;
    if (fifBadMinMaxForOdds (*pBet, oddsAmount, ep)) return Gen::ReturnCode::Fail;

    pBet->setOddsAmountInternal(oddsAmount);
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------
//
// Determine if we have the given bet on the table.
//
bool
CrapsTable::haveBet(const CrapsBet& bet) const
{
    return findBetById(bet.betId()) != nullptr;
}

//----------------------------------------------------------------
//
// Find a bet by ID.
//
BetPtr
CrapsTable::findBetById(BetId betId) const
{
    // Loop over all bets
    for (size_t i = 0; i < tableBets_.size(); ++i)
    {
        auto& bets = tableBets_[i];
        for (auto& b : bets)
        {
            if (betId == b->betId())
            {
                return b;
            }
        }
    }
    return nullptr;
}

//----------------------------------------------------------------

void
CrapsTable::rollDice()
{
    declareBettingClosed();
    throwDice();
    resolveBets();
    advanceState();            // Updates point, updates shooter
    bumpRecentRolls(dice_);
    currentStats_.recordDiceRoll(point_, dice_);
    lastRollStats_.rollCount = getNumRolls();
    declareBettingOpen();
}

//----------------------------------------------------------------

void
CrapsTable::declareBettingClosed()
{
    bettingOpen_ = false; // No more bets
    eventMgr_.publish(BettingClosed{});
}

//----------------------------------------------------------------

void
CrapsTable::declareBettingOpen()
{
    bettingOpen_ = true;
    eventMgr_.publish(BettingOpened{});
}

//----------------------------------------------------------------

void
CrapsTable::throwDice()
{
    eventMgr_.publish(DiceThrowStart{});
    if (isTestRoll_) dice_ = testRollDice_; else dice_.roll();
    
//  std::cout << "point:" << point_ << " dice:" << dice_.value()
//            << "(" << dice_.d1() << "," << dice_.d2() << ")\n";
    eventMgr_.publish(DiceThrowEnd{});
    eventMgr_.publish(AnnounceDiceNumber{dice_.value(), dice_.d1(), dice_.d2()});
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
            eventMgr_.publish(PointEstablished{point_});
        }
    }
    else if (dice_.value() == 7)
    {
        point_ = 0;
        eventMgr_.publish(SevenOut{});
        advanceShooter();
    }
    else if (point_ == dice_.value())
    {
        point_ = 0;
        eventMgr_.publish(PassLineWinner{});
    }
}

//----------------------------------------------------------------

void
CrapsTable::advanceShooter()
{
    if (players_.empty()) return;

    Player* prev = pCurrentShooter_;

    auto it = std::find(players_.begin(), players_.end(), pCurrentShooter_);

    // If not found or at the end, start from beginning
    if (it == players_.end() || std::next(it) == players_.end())
    {
        pCurrentShooter_ = players_.front();
    }
    else
    {
        pCurrentShooter_ = *std::next(it);
    }

    if (pCurrentShooter_ != prev)
    {
        eventMgr_.publish(NewShooter{pCurrentShooter_->getPlayerId()});
    }
}

//----------------------------------------------------------------

void
CrapsTable::resolveBets()
{
    eventMgr_.publish(ResolveBetsStart{});
    lastRollStats_.prep(getAmountOnTable(), getNumBetsOnTable());
    evaluateBets();
    dispenseResults();
    trimTableBets();
    clearDrl();
    eventMgr_.publish(ResolveBetsEnd{});
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
            evalOneBet(*b);
        }
    }
}

//----------------------------------------------------------------
//
// Creates a decision record for the given bet and adds it to the DRL.
//
void
CrapsTable::evalOneBet(CrapsBet& bet)
{
    DecisionRecord dr;
    Gen::ErrorPass ep;

    if (bet.evaluate(point_, dice_, dr, ep) == Gen::ReturnCode::Success)
    {
        drl_.push_back(dr);
//      std::cout << dr << std::endl;
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
            lastRollStats_.amountWin += r.lose;
            lastRollStats_.numBetsWin++;
        }
        if (r.win > 0)  // player wins, house loses
        {
            houseBank_.withdraw(r.win);
            lastRollStats_.amountLose += r.win;
            lastRollStats_.numBetsLose++;
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
            r.pBet->player().processWin(r);
            currentStats_.recordWin(*(r.pBet), r.win);
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
            r.pBet->player().processLose(r);
            currentStats_.recordLose(*(r.pBet), r.lose);
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
            r.pBet->player().processKeep(r);
            currentStats_.recordKeep(*(r.pBet));
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
// player is processing decision record.
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
            if (removeMatchingBet(bets, dr.pBet))
            {
                break;
            }
        }
    }
}

//----------------------------------------------------------------
//
// Part of trimTableBets processing chain.
// 
bool
CrapsTable::removeMatchingBet(BetList& bets, CrapsBet* pBet)
{
    auto it = std::find_if(bets.begin(), bets.end(),
        [pBet](const BetPtr& b)
        {
            return b.get() == pBet;
        });

    if (it != bets.end())
    {
        bets.erase(it);
        return true;
    }
    return false;
}

//----------------------------------------------------------------
//
// Administrative function to clear out all bets owned by
// a player because he left the table. Money for each bet found
// is given to the house, as would occur in a real craps grame.
//
void
CrapsTable::removePlayerBets(Player* pPlayer)
{
    for (size_t i = 0; i < tableBets_.size(); ++i)
    {
        removeBetsByPlayerPtr(tableBets_[i], pPlayer);
    }
}

//----------------------------------------------------------------
//
// Part 2 of removePlayerBets, this part traverses the
// std::list of bets and removes bet if it matches playerId.
// Claims bet money for the house bank.
//
void
CrapsTable::removeBetsByPlayerPtr(BetList& bets, Player* pPlayer)
{
    for (auto it = bets.begin(); it != bets.end(); )
    {
        if (*it && (*it)->pPlayer_ == pPlayer)
        {
            BetPtr p = *it;
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
CrapsTable::havePlayer(Player* pPlayer) const
{
    return std::find(players_.begin(), players_.end(), pPlayer) != players_.end();
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsTable::removePlayerByPtr(Player* pPlayer, Gen::ErrorPass& ep)
{
    auto it = std::find(players_.begin(), players_.end(), pPlayer);
    if (it != players_.end())
    {
        players_.erase(it);
        return Gen::ReturnCode::Success;
    }
    ep.diag = "Player has not joined this table.";
    return Gen::ReturnCode::Fail;
}

//----------------------------------------------------------------

void
CrapsTable::resetStats()
{
    recentRolls_.clear();
    currentStats_.reset();
    houseBank_.resetStats();
}

//----------------------------------------------------------------

std::vector<Player*>
CrapsTable::getPlayers() const
{
    std::vector<Player*> v;
    for (const auto pPlayer : players_)
    {
        v.push_back(pPlayer);
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

Player*
CrapsTable::getCurrentShooter() const
{
    return pCurrentShooter_;
}

//----------------------------------------------------------------

Dice
CrapsTable::getCurrentRoll() const
{
    return dice_;
}

//----------------------------------------------------------------

unsigned
CrapsTable::getNumRolls() const
{
    return currentStats_.rollStats.numRolls;
}

//----------------------------------------------------------------

unsigned
CrapsTable::getMinLineBet() const
{
    return minLineBet_;
}

//----------------------------------------------------------------

unsigned
CrapsTable::getMaxLineBet() const
{
    return maxLineBet_;
}

//----------------------------------------------------------------

unsigned
CrapsTable::getMaxOdds() const
{
    return maxOdds_;
}

//----------------------------------------------------------------

const std::deque<Dice>&
CrapsTable::getRecentRolls() const
{
    return recentRolls_;
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
CrapsTable::getCurrentStats() const
{
    return currentStats_;
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

const BankStats&
CrapsTable::getBankCurrentStats() const
{
    return houseBank_.getCurrentStats();
}

//----------------------------------------------------------------

const BankStats&
CrapsTable::getBankAlltimeStats() const
{
    return houseBank_.getAlltimeStats();
}

//----------------------------------------------------------------

const LastRollStats&
CrapsTable::getLastRollStats() const
{
    return lastRollStats_;
}

//----------------------------------------------------------------
//
// Return session history.
//
// This is just a relay function for convenience.
//
const SessionHistory::Sessions&
CrapsTable::getSessionHistory() const
{
    return alltimeStats_.sessionHistory.getSessions();
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

//-----------------------------------------------------------------

void
CrapsTable::bumpRecentRolls(const Dice& dice)
{
    if (recentRolls_.size() >= config_.maxRecentRolls)
    {
        recentRolls_.pop_front();
    }
    recentRolls_.push_back(dice);
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



#if 0

// old

//----------------------------------------------------------------
//
// Determine if we have the given bet on the table.
//
bool
CrapsTable::haveBet(const BetPtr bet) const
{
    return findBetById(bet->betId()) != nullptr;

    // Loop over all bets
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
CrapsTable::haveBet(const PlayerId& playerId, BetName betName,
                    unsigned pivot) const
{
    auto& bets = tableBets_[static_cast<size_t>(betName)];
    for (auto& b : bets)
    {
        if (playerId == b->playerId() &&
            betName  == b->betName()  &&
            pivot    == b->pivot())
        {
            return true;
        }
    }
    return false;
}


//----------------------------------------------------------------
//
// Determine if we have the given bet on the table.
//
bool
CrapsTable::haveBet(const CrapsBet& bet) const
{
    // Loop over all bets
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

#endif
