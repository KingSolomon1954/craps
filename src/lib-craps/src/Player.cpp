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
#include <gen/FileUtils.h>
#include <gen/Logger.h>
#include <gen/Uuid.h>

using namespace Craps;

//----------------------------------------------------------------
//
// Constructor.
//
Player::Player(
    const PlayerId&     playerId,
    const PlayerConfig& config,
    EventManager&       eventMgr)
    : playerId_(playerId)
    , config_(config)
    , eventMgr_(eventMgr)
    , wallet_(InitialStartingBankBalance_, RefillThreshold_, RefillAmount_)
{
}

//----------------------------------------------------------------
//
// Will create a fresh PlayerId
//
Player*
Player::createPlayer(const std::string&  playerName,
                     const PlayerConfig& config,
                     EventManager&       eventMgr)
{
    Player* p = new Player(Gen::generateUuid(), config, eventMgr);
    p->setName(playerName);
    return p;
}
    
/*-----------------------------------------------------------*//**

Construct Player from in-memory YAML node.

Throws upon error.

Static function.
*/
Player*
Player::fromString(const std::string&  yaml,
                   const PlayerId&     playerId,
                   const PlayerConfig& config,
                   EventManager&       eventMgr)
{
    Player* p = new Player(playerId, config, eventMgr);
    YAML::Node root = YAML::Load(yaml);
    p->fromYAML(root);
    return p;
}

/*-----------------------------------------------------------*//**

Construct Player from YAML file.

Throws upon error.

Static function.
*/
Player*
Player::fromFile(const PlayerId& playerId,
                 const PlayerConfig& config,
                 EventManager& eventMgr)
{
    Player* p = new Player(playerId, config, eventMgr);
    p->loadFile();
    return p;
}

//----------------------------------------------------------------
//
// Save Player to file
//
void
Player::saveFile() const
{
    if (config_.playerPath.empty())
    {
        LOG_DEBUG("Player::saveFile() skipping; playerPath is empty");
        return;
    }
    
    LOG_DEBUG("Player::saveFile(" + config_.playerPath.string()  + ")");
    std::ofstream fout(config_.playerPath);
    fout << toYAML();
}

//----------------------------------------------------------------
//
// Load Player from file
//
void
Player::loadFile()
{
    try
    {
        std::ifstream fin =
            Gen::FileUtils::openOrThrow(config_.playerPath);  // throws
        YAML::Node root = YAML::Load(fin);
        fromYAML(root);                                       // throws
    }
    catch (const std::runtime_error& e)
    {
        std::string diag("Player::loadFile(): \"" +
                         config_.playerPath.string() + "\"; ");
        throw std::runtime_error(diag + e.what());
    }
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
        ep.prepend("Player::joinTable(): " + playerName_ + 
                   " failed to join table. ");
        return Gen::ReturnCode::Fail;
    }
    setupSubscriptions();

    return Gen::ReturnCode::Success;
}

/*-----------------------------------------------------------*//**

Makes a bet on the table.

@param[in] betName
    the type of bet to make

@param[in] contractAmount
    the dollar amount to bet

@param[in] the pivot
    The number this bet is focused on. For example a Place bet must set
    the pivot to 4,5,6,8,9 or 10. For PassLine/Come/DontPass/DontCome bets,
    the caller sets the pivot to zero. Zero indicates the pivot number
    needs to be set later. See CrapsBet::CrapsBet() constructor.

@param[in,out] ep
    If an error occurs, ep holds the reason

@return
    If successful, the shared pointer to the CrapsBet is returned, 
    otherwise a nullptr and ep has the reason for failure

@internal
@li fif prefix means "fault if"
*/
BetPtr
Player::makeBet(BetName betName,
                Gen::Money contractAmount,
                unsigned pivot,
                Gen::ErrorPass& ep)
{
    // fif prefix means "fault if"
    
    if (fifNoTable(1, ep))                                    return nullptr;
    if (fifInsufficientFunds(nullptr, contractAmount, 1, ep)) return nullptr;
    auto pBet = makeShared(betName, contractAmount, pivot, ep);
    if (pBet == nullptr)                                      return nullptr;
    if (fifBadAddBet(pBet, ep))                               return nullptr;
    wallet_.withdraw(contractAmount);
    bets_.push_back(pBet);
    return pBet;
}

//----------------------------------------------------------------
//
// Purpose of this is to issue std::make_shared in try/catch block.
// Convert exception to ErrorPass.
//
BetPtr
Player::makeShared(BetName betName,
                   Gen::Money contractAmount,
                   unsigned pivot,
                   Gen::ErrorPass& ep)
{
    try
    {
        return std::make_shared<CrapsBet>(this, betName,
                                          contractAmount, pivot);
    }
    catch(std::invalid_argument& e)
    {
        ep.diag = diagPrefix(1) + e.what();
        return nullptr;
    }
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

@param[in] oddsAmount
    The amount to set it to. Clobbers any previous setting.

@param[in,out] ep
    If error occurs, ep holds the reason

@returns
    Success if the bet was accepted, otherwise Fail and ep has
    the reason.

@internal
@li fif prefix means "fault if"
*/
Gen::ReturnCode
Player::setOddsAmount(BetPtr pBet,
                      Gen::Money oddsAmount,
                      Gen::ErrorPass& ep)
{
    // fif prefix means "fault if"
    
    if (fifMissingBet(pBet, ep))                       return Gen::ReturnCode::Fail;
    if (fifNoTable(2, ep))                             return Gen::ReturnCode::Fail;
    if (fifInsufficientFunds(pBet, oddsAmount, 2, ep)) return Gen::ReturnCode::Fail;
    if (fifBadSetOdds(pBet, oddsAmount, ep))           return Gen::ReturnCode::Fail;

    // Adjust wallet. Handle increase or decrease in odds bet
    if (oddsAmount < pBet->oddsAmount())
    {
        wallet_.deposit(pBet->oddsAmount() - oddsAmount);
    }
    if (oddsAmount > pBet->oddsAmount())
    {
        wallet_.withdraw(oddsAmount - pBet->oddsAmount());
    }
    return Gen::ReturnCode::Success;
}

/*-----------------------------------------------------------*//**

Process WIN decision

Called by CrapsTable to dish out a winning bet to a Player.

*/
void
Player::processWin(const DecisionRecord& dr)
{
    assert(dr.pBet != nullptr);
    assert(dr.win > 0);

    // Confirm we actually own the bet in dr.pBet (raw pointer)
    // Obtain shared_ptr to our bet
    // 
    auto pBet = findBetById(dr.pBet->betId());
    if (pBet == nullptr)
    {
        diagBadBetId("processWin() ", dr.pBet->betId());
        assert(false);
        return;
    }

    wallet_.deposit(dr.returnToPlayer);
    wallet_.deposit(dr.win + pBet->contractAmount() + pBet->oddsAmount());
    
    // std::cout << playerName_ << ": processWin(" << pBet->betName() <<
    //     ") won:" << dr.win << " balance:" << wallet_.getBalance() << "\n";

    // TODO update win stats before removing bet
    // pBet->startTime - endTime ...

    (void) removeBetByPtr(pBet);
}

/*-----------------------------------------------------------*//**

Process LOSE decision

Called by CrapsTable to dish out a losing bet to a Player.

*/
void
Player::processLose(const DecisionRecord& dr)
{
    assert(dr.pBet != nullptr);
    assert(dr.lose > 0);

    // Confirm we actually own the bet in dr.pBet (raw pointer)
    // Obtain shared_ptr to our bet
    // 
    auto pBet = findBetById(dr.pBet->betId());
    if (pBet == nullptr)
    {
        diagBadBetId("processLose() ", dr.pBet->betId());
        assert(false);
        return;
    }

    // No need to update money. Money was withdrawn from wallet
    // when making the bet. Just need hand return to player, if any.
    // 
    wallet_.deposit(dr.returnToPlayer);


//    std::cout << playerName_ << ": processLose(" << pBet->betName() <<
//        ") lost:" << dr.lose << " balance:" << wallet_.getBalance() << "\n";

    
    // TODO update lose stats before removing bet
    // pBet->startTime - endTime ...

    (void) removeBetByPtr(pBet);  // Done with this bet
}

/*-----------------------------------------------------------*//**

Process KEEP decision

Called by CrapsTable to inform Player that the current roll of
dice resulted in no decision for the assocated bet.

*/
void
Player::processKeep(const DecisionRecord& dr)
{
    assert(dr.pBet != nullptr);
    
    // Confirm we actually own the bet in dr.pBet (raw pointer)
    // Obtain shared_ptr to our bet
    // 
    auto pBet = findBetById(dr.pBet->betId());
    if (pBet == nullptr)
    {
        diagBadBetId("processKeep() ", dr.pBet->betId());
        assert(false);
        return;
    }

//    std::cout << playerName_ << ": processKeep(" << pBet->betName() <<
//        ") lost:" << dr.lose << " won:" << dr.win
//        << " balance:" << wallet_.getBalance() << "\n";

    assert(dr.lose == 0); assert(dr.win == 0);
    
    // TODO
    // maybe the pivot was assigned, if so do auto odds?
    // update stats
}

//----------------------------------------------------------------

bool
Player::haveBet(const CrapsBet& bet) const
{
    return findBetById(bet.betId()) != nullptr;
}

//----------------------------------------------------------------
//
// Search for bet by ID
//
BetPtr
Player::findBetById(BetId betId) const
{
    auto it = std::find_if(bets_.begin(), bets_.end(),
                   [betId](const BetPtr& b)
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
//
// Private function. Used to remove a bet from this player's
// collection. Meant to be called after processing a bet result.
//
bool
Player::removeBetByPtr(BetPtr& b)
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
        [betName, pivot](const BetPtr& b)
        {
            return (b->betName() == betName) && (b->pivot() == pivot);
        });
    if (it == bets_.end())
    {
        ep.diag = "Player::removeBet(): unable to remove bet; Player " +
            playerName_ + " does not have such a bet(" +
            EnumBetName::toString(betName) + ":" + std::to_string(pivot) + ").";
        return Gen::ReturnCode::Fail;
    }

    // We have a bet, then we must have valid pTable. No need to check.
    // We have a bet, then it must be on the table. See makeBet().
    // If table can't remove it then player can't remove it.
    if (pTable_->removeBet(*it, ep) == Gen::ReturnCode::Fail)
    {
        ep.prepend("Player::removeBet(): ");
        return Gen::ReturnCode::Fail;
    }

    wallet_.deposit((*it)->contractAmount() + (*it)->oddsAmount());
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

const PlayerId&
Player::getPlayerId() const
{
    return playerId_;
}

//----------------------------------------------------------------

const std::string&
Player::getName() const
{
    return playerName_;
}

//----------------------------------------------------------------

void
Player::setName(const std::string& playerName)
{
    playerName_ = playerName;
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
    // std::cout << playerName_ << " acknowledges BettingClosed\n";
}

//----------------------------------------------------------------

void
Player::onBettingOpened()
{
    // TODO
    // std::cout << playerName_ << " acknowledges BettingOpen\n";
}

//----------------------------------------------------------------

void
Player::onDiceThrowStart()
{
    // TODO
    // std::cout << playerName_ << " acknowledges DiceThrowStart\n";
}

//----------------------------------------------------------------

void
Player::onDiceThrowEnd()
{
    // TODO
    // std::cout << playerName_ << " acknowledges DiceThrowEnd\n";
}

//----------------------------------------------------------------

void
Player::onAnnounceDiceNumber(const AnnounceDiceNumber& evt)
{
    // TODO
    // std::cout << playerName_ << " acknowledges AnnounceDiceNumber " << evt.val
    //           << "(" << evt.d1 << "," << evt.d2 << ")\n";
}

//----------------------------------------------------------------

void
Player::onPointEstablished(const PointEstablished& evt)
{
    // TODO
    // std::cout << playerName_ << " acknowledges PointEstablished " << evt.point << "\n";
}

//----------------------------------------------------------------

void
Player::onSevenOut()
{
    // TODO
    // std::cout << playerName_ << " acknowledges SevenOut\n";
}

//----------------------------------------------------------------

void
Player::onPassLineWinner()
{
    // TODO
    // std::cout << playerName_ << " acknowledges PassLineWinner\n";
}

//----------------------------------------------------------------

void
Player::onNewShooter(const NewShooter& evt)
{
    // TODO
    // std::cout << playerName_ << " acknowledges NewShooter " <<
    //      Gbl::pPlayerMgr->getPlayer(evt.shooterId)->getName() << "\n";
}

//----------------------------------------------------------------

void
Player::diagBadBetId(const std::string& funcName, BetId betId) const
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

std::string
Player::diagPrefix(size_t idx) const
{
    std::string diag("Player::");
    if (idx == 1)
    {
        diag += "makeBet(): Unable to make bet; ";
    }
    if (idx == 2)
    {
        diag += "setOdds(): Unable to set odds; ";
    }
    if (idx > 2) assert(false);
    return diag;
}

//----------------------------------------------------------------

bool
Player::fifNoTable(size_t idx, Gen::ErrorPass& ep) const
{
    // fault if player is not joined to a table and sets ep error diag
    if (pTable_ == nullptr)
    {
        ep.diag = diagPrefix(idx) + "Player " + playerName_ +
            " has not yet joined a table.";
        return true;
    }
    return false;
}

//----------------------------------------------------------------

bool
Player::fifInsufficientFunds(BetPtr pBet, Gen::Money amount,
                             size_t idx, Gen::ErrorPass& ep) const
{
    // fault if insufficient funds and sets ep error diag
    int diff = 0;
    if (pBet == nullptr)
    {
        diff = amount;
    }
    else
    {
        diff = amount - pBet->oddsAmount();
    }

    if (diff <= 0)
    {
        return false;  // Reducing existing bet, always enough funds.
    }
    
    if (diff > wallet_.getBalance())
    {
        ep.diag = diagPrefix(idx) + "Player " + playerName_ +
            " has insufficient funds to make a " +
            Gen::MoneyUtils::toString(amount)    +
            " bet; current balance:"             +
            Gen::MoneyUtils::toString(getBalance()) + ".";
        return true;
    }
    return false;
}

//----------------------------------------------------------------

bool
Player::fifBadAddBet(BetPtr pBet, Gen::ErrorPass& ep)
{
    // fault if can't add bet and sets ep error diag
    assert(pBet != nullptr);
    assert(pTable_ != nullptr);
    
    if (pTable_->addBet(pBet, ep) == Gen::ReturnCode::Fail)
    {
        ep.prepend(diagPrefix(1));
        return true;
    }
    return false;
}

//----------------------------------------------------------------

bool
Player::fifMissingBet(BetPtr pBet, Gen::ErrorPass& ep) const
{
    // fault if bet is not owned by this player and sets ep error diag
    assert(pBet != nullptr);
    
    if (findBetById(pBet->betId()) == nullptr)
    {
        ep.diag = diagPrefix(2) +
            "Player " + playerName_ + " does not own this bet; " +
            pBet->diagBetId() + ".";
        return true;
    }
    return false;
}

//----------------------------------------------------------------

bool
Player::fifBadSetOdds(BetPtr pBet, Gen::Money oddsAmount,
                      Gen::ErrorPass& ep)
{
    assert(pBet != nullptr);
    assert(pTable_ != nullptr);
    
    if (pTable_->setOddsAmount(pBet, oddsAmount, ep) == Gen::ReturnCode::Fail)
    {
        ep.prepend(diagPrefix(2));
        return true;
    }
    return false;
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

