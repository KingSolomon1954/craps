//----------------------------------------------------------------
//
// File: Player.cpp
//
//----------------------------------------------------------------

#include <craps/Player.h>
#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <craps/CrapsTable.h>
#include <craps/DecisionRecord.h>
#include <craps/EventManager.h>
#include <gen/ErrorPass.h>

using namespace Craps;

//----------------------------------------------------------------
//
// Constructor.
//
// Player::Player()
// {
//     setupSubscriptions();
// }

//----------------------------------------------------------------
//
// Constructor.
//
Player::Player(
    const std::string& name,
    unsigned startingBalance,
    EventManager& eventMgr)
    : uuid_(Gen::generateUuid())
    , name_(name)
    , wallet_(startingBalance, 500, 500)  // TODO
    , eventMgr_(eventMgr)
{
}

//----------------------------------------------------------------
//
// Constructor.
//
Player::Player(
    const Gen::Uuid& uuid,
    const std::string& name,
    unsigned startingBalance,
    EventManager& eventMgr)
    : uuid_(uuid)
    , name_(name)
    , wallet_(startingBalance, 500, 500) // TODO)
    , eventMgr_(eventMgr)
{
}

//----------------------------------------------------------------

void
Player::setupSubscriptions()
{
    eventMgr_.subscribe<BettingClosed>(
        [this](const BettingClosed&)
        {
            this->onBettingClosed();
        });
    eventMgr_.subscribe<BettingOpened>(
        [this](const BettingOpened&)
        {
            this->onBettingOpened();
        });
    eventMgr_.subscribe<DiceThrowStart>(
        [this](const DiceThrowStart&)
        {
            this->onDiceThrowStart();
        });
    eventMgr_.subscribe<DiceThrowEnd>(
        [this](const DiceThrowEnd&)
        {
            this->onDiceThrowEnd();
        });
    eventMgr_.subscribe<AnnounceDiceNumber>(
        [this](const AnnounceDiceNumber& evt)
        {
            this->onAnnounceDiceNumber(evt);
        });
    eventMgr_.subscribe<PointEstablished>(
        [this](const PointEstablished& evt)
        {
            this->onPointEstablished(evt);
        });
    eventMgr_.subscribe<SevenOut>(
        [this](const SevenOut&)
        {
            this->onSevenOut();
        });
    eventMgr_.subscribe<PassLineWinner>(
        [this](const PassLineWinner&)
        {
            this->onPassLineWinner();
        });
    eventMgr_.subscribe<NewShooter>(
        [this](const NewShooter& evt)
        {
            this->onNewShooter(evt);
        });
}
                
//----------------------------------------------------------------

Gen::ReturnCode
Player::joinTable(CrapsTable* pTable, Gen::ErrorPass& ep)
{
    assert(pTable != nullptr);
    pTable_ = pTable;
    
    if (pTable_->addPlayer(this, ep) == Gen::ReturnCode::Fail)
    {
        ep.prepend("Player " + name_ + " joining table. ");
        return Gen::ReturnCode::Fail;
    }
    setupSubscriptions();  // TODO maybe setupTableSubscriptions?

    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

CrapsBet::BetPtr
Player::makeBet(BetName betName,
                Gen::Money contractAmount,
                unsigned pivot,
                Gen::ErrorPass& ep)
{
    if (pTable_ == nullptr)
    {
        // TODO set ep
        ep.diag = "Not joined a table";
        return nullptr;
    }
    // TODO check sufficient funds first
    try
    {
        auto pBet = std::make_shared<CrapsBet>
            (this, betName, contractAmount, pivot);
        assert(pBet != nullptr);   // In case we miss an exception

        // Place the bet
        if (pTable_->addBet(pBet, ep) == Gen::ReturnCode::Fail)
        {
            // TODO add to diag
            assert(false);
            return nullptr;
        }
        
        wallet_.withdraw(contractAmount);
        bets_.push_back(pBet);
        return pBet;
    }
    catch(std::invalid_argument& e)
    {
        return nullptr;
    }
}

//----------------------------------------------------------------

Gen::ReturnCode
Player::setOddsAmount(CrapsBet::BetPtr pBet,
                      Gen::Money oddsAmount,
                      Gen::ErrorPass& ep)
{
    // TODO check if player owns this bet

    if (pTable_ == nullptr)
    {
        // TODO set ep
        ep.diag = "Not joined a table";
        return Gen::ReturnCode::Success;
    }
    
    // TODO check sufficient funds first
    if (pTable_->setOddsAmount(pBet, oddsAmount, ep) == Gen::ReturnCode::Fail)
    {
        // TODO diagnostics
        return Gen::ReturnCode::Fail;
    }
    wallet_.withdraw(oddsAmount);
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

void
Player::processWin(const DecisionRecord& dr)
{
    assert(dr.win > 0);

    // Obtain pointer to our bet
    auto pBet = findBetById(dr.pBet->betId());
    if (pBet == nullptr)
    {
        diagBadBetId("processWin() ", dr.pBet->betId());
        assert(false);
        return;
    }

    wallet_.deposit(dr.returnToPlayer);
    wallet_.deposit(dr.win + pBet->contractAmount() + pBet->oddsAmount());
    
    // std::cout << name_ << ": processWin(" << pBet->betName() <<
    //     ") won:" << dr.win << " balance:" << wallet_.getBalance() << "\n";

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
    auto pBet = findBetById(dr.pBet->betId());
    if (pBet == nullptr)
    {
        diagBadBetId("processLose() ", dr.pBet->betId());
        assert(false);
        return;
    }

//    std::cout << name_ << ": processLose(" << pBet->betName() <<
//        ") lost:" << dr.lose << " balance:" << wallet_.getBalance() << "\n";

    // TODO update lose stats before removing bet
    // pBet->startTime - endTime ...

    (void) removeBetByPtr(pBet);  // Done with this bet
}

//----------------------------------------------------------------

void
Player::processKeep(const DecisionRecord& dr)
{
    // Obtain pointer to the bet (for stats and stuff)
    auto pBet = findBetById(dr.pBet->betId());
    if (pBet == nullptr)
    {
        diagBadBetId("processKeep() ", dr.pBet->betId());
        assert(false);
        return;
    }

//    std::cout << name_ << ": processKeep(" << pBet->betName() <<
//        ") lost:" << dr.lose << " won:" << dr.win
//        << " balance:" << wallet_.getBalance() << "\n";

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
CrapsBet::BetPtr
Player::findBetById(unsigned betId) const
{
    auto it = std::find_if(bets_.begin(), bets_.end(),
                   [betId](const CrapsBet::BetPtr& b)
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
Player::removeBetByPtr(CrapsBet::BetPtr& b)
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
        [betName, pivot](const CrapsBet::BetPtr& b)
        {
            return (b->betName() == betName) && (b->pivot() == pivot);
        });
    if (it == bets_.end())
    {
        ep.diag = "not found";
        return Gen::ReturnCode::Fail;
    }

    if (pTable_ == nullptr)
    {
        // Maybe this can't happen. look at makeBet(), see if bet is created
        // before and part of our list before adding to table.
        bets_.erase(it, bets_.end());  // remove from our list of bets
        // TODO set ep
        ep.diag = "removed bet locally but it was never placed on table";
        return Gen::ReturnCode::Fail;
    }
    
    if (pTable_->removeBet(*it, ep) == Gen::ReturnCode::Fail)
    {
        ep.prepend("problem removing bet");
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
    // TODO    
    return true;
}

//----------------------------------------------------------------
//
// Load Player from file
//
bool
Player::loadFromFile(const std::string& path)
{
    return true;
}

//----------------------------------------------------------------
//
// Convert Player to YAML
//
YAML::Node
Player::toYAML() const
{
    // TODO
    YAML::Node node;
    return node;
}

//----------------------------------------------------------------
//
// Convert YAML to Player
//
void
Player::fromYAML(const YAML::Node& node)
{
    // TODO
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

Gen::Money
Player::getBalance() const
{
    return wallet_.getBalance();
}

//----------------------------------------------------------------

void
Player::onBettingClosed()
{
    // TODO
    // std::cout << name_ << " acknowledges BettingClosed\n";
}

//----------------------------------------------------------------

void
Player::onBettingOpened()
{
    // TODO
    // std::cout << name_ << " acknowledges BettingOpen\n";
}

//----------------------------------------------------------------

void
Player::onDiceThrowStart()
{
    // TODO
    // std::cout << name_ << " acknowledges DiceThrowStart\n";
}

//----------------------------------------------------------------

void
Player::onDiceThrowEnd()
{
    // TODO
    // std::cout << name_ << " acknowledges DiceThrowEnd\n";
}

//----------------------------------------------------------------

void
Player::onAnnounceDiceNumber(const AnnounceDiceNumber& evt)
{
    // TODO
    // std::cout << name_ << " acknowledges AnnounceDiceNumber " << evt.val
    //           << "(" << evt.d1 << "," << evt.d2 << ")\n";
}

//----------------------------------------------------------------

void
Player::onPointEstablished(const PointEstablished& evt)
{
    // TODO
    // std::cout << name_ << " acknowledges PointEstablished " << evt.point << "\n";
}

//----------------------------------------------------------------

void
Player::onSevenOut()
{
    // TODO
    // std::cout << name_ << " acknowledges SevenOut\n";
}

//----------------------------------------------------------------

void
Player::onPassLineWinner()
{
    // TODO
    // std::cout << name_ << " acknowledges PassLineWinner\n";
}

//----------------------------------------------------------------

void
Player::onNewShooter(const NewShooter& evt)
{
    // TODO
    // std::cout << name_ << " acknowledges NewShooter " <<
    //      Gbl::pPlayerMgr->getPlayer(evt.shooterId)->getName() << "\n";
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
