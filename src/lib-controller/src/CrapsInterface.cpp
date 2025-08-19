//----------------------------------------------------------------
//
// File: CrapsInterface.cpp
//
//----------------------------------------------------------------

#include <controller/CrapsInterface.h>
#include <controller/Globals.h>
#include <controller/PlayerManager.h>
#include <controller/TableManager.h>
#include <craps/BankStats.h>
#include <craps/CrapsTable.h>
#include <craps/LastRollStats.h>
#include <craps/Player.h>
#include <craps/PlayerStats.h>
#include <craps/SessionHistory.h>

using namespace Ctrl;

//----------------------------------------------------------------
//
// Player related
//
//----------------------------------------------------------------

Gen::ReturnCode
CrapsInterface::playerJoinTable(
    const Craps::PlayerId& playerId,
    const Craps::TableId& tableId,
    Gen::ErrorPass& ep)
{
    Craps::Player* p = Gbl::pPlayerMgr->getPlayer(playerId, ep);
    if (p == nullptr)
    {
        ep.prepend("CrapsInterface::joinTable(): unable to join table; ");
        return Gen::ReturnCode::Fail;
    }
    return p->joinTable(Gbl::pTable, ep);
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsInterface::playerLeaveTable(
    const Craps::PlayerId& playerId,
    Gen::ErrorPass& ep)
{
    Craps::Player* p = Gbl::pPlayerMgr->getPlayer(playerId, ep);
    if (p == nullptr)
    {
        ep.prepend("CrapsInterface::leaveTable(): unable to leave table; ");
        return Gen::ReturnCode::Fail;
    }
    return p->leaveTable(ep);
}

//----------------------------------------------------------------

Craps::BetId
CrapsInterface::playerMakeBet(
    const Craps::PlayerId& playerId,
    BetName betName,
    Gen::Money contractAmount,
    unsigned pivot,
    Gen::ErrorPass& ep)
{
    Craps::Player* p = Gbl::pPlayerMgr->getPlayer(playerId, ep);
    if (p == nullptr)
    {
        ep.prepend("CrapsInterface::makeBet(): unable to makeBet; ");
        return 0;
    }
    Craps::BetPtr pBet = p->makeBet(betName, contractAmount, pivot, ep);
    if (pBet == nullptr)
    {
        ep.prepend("CrapsInterface::makeBet(): unable to makeBet; ");
        return 0;
    }
    return pBet->betId();
}

//----------------------------------------------------------------

bool
CrapsInterface::playerHaveBet(
    const Craps::PlayerId& playerId,
    const Craps::BetId& betId)
{
    auto pBet = Gbl::pTable->getBet(betId);
    if (pBet == nullptr) return false;
    return pBet->player().getPlayerId() == playerId;
}

//----------------------------------------------------------------

const std::string&
CrapsInterface::playerName(
    const Craps::PlayerId& playerId)
{
    Gen::ErrorPass ep;
    Craps::Player* p = Gbl::pPlayerMgr->getPlayer(playerId, ep);
    if (p == nullptr)
    {
        static std::string empty;
        return empty;
    }
    return p->getName();
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsInterface::playerAmountOnTable(
    const Craps::PlayerId& playerId,
    Gen::Money& amount,
    Gen::ErrorPass& ep)
{
    Craps::Player* p = Gbl::pPlayerMgr->getPlayer(playerId, ep);
    if (p == nullptr) return Gen::ReturnCode::Fail;
    amount = p->getAmountOnTable();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsInterface::playerNumBetsOnTable(
    const Craps::PlayerId& playerId,
    unsigned& numBets,
    Gen::ErrorPass& ep)
{
    Craps::Player* p = Gbl::pPlayerMgr->getPlayer(playerId, ep);
    if (p == nullptr) return Gen::ReturnCode::Fail;
    numBets = p->getNumBetsOnTable();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
playerBalance(
    const Craps::PlayerId& playerId,
    Gen::Money& balance,
    Gen::ErrorPass& ep)
{
    Craps::Player* p = Gbl::pPlayerMgr->getPlayer(playerId, ep);
    if (p == nullptr) return Gen::ReturnCode::Fail;
    balance = p->getBalance();
    return Gen::ReturnCode::Success;
    
}

//----------------------------------------------------------------

const Craps::LastRollStats&
CrapsInterface::playerLastRollStats(
    const Craps::PlayerId& playerId)
{
    Gen::ErrorPass ep;
    Craps::Player* p = Gbl::pPlayerMgr->getPlayer(playerId, ep);
    if (p == nullptr)
    {
        static Craps::LastRollStats empty;
        return empty;
    }
    return p->getLastRollStats();
}

//----------------------------------------------------------------

const Craps::PlayerStats&
CrapsInterface::playerCurrentStats(
    const Craps::PlayerId& playerId)
{
    Gen::ErrorPass ep;
    Craps::Player* p = Gbl::pPlayerMgr->getPlayer(playerId, ep);
    if (p == nullptr)
    {
        static Craps::PlayerStats empty;
        return empty;
    }
    return p->getCurrentStats();
}

//----------------------------------------------------------------

const Craps::PlayerStats&
CrapsInterface::playerAlltimeStats(
    const Craps::PlayerId& playerId)
{
    Gen::ErrorPass ep;
    Craps::Player* p = Gbl::pPlayerMgr->getPlayer(playerId, ep);
    if (p == nullptr)
    {
        static Craps::PlayerStats empty;
        return empty;
    }
    return p->getAlltimeStats();
}

//----------------------------------------------------------------

const Craps::BankStats&
CrapsInterface::playerCurrentStatsBank(
    const Craps::PlayerId& playerId)
{
    Gen::ErrorPass ep;
    Craps::Player* p = Gbl::pPlayerMgr->getPlayer(playerId, ep);
    if (p == nullptr)
    {
        static Craps::BankStats empty;
        return empty;
    }
    return p->getBankCurrentStats();
}

//----------------------------------------------------------------

const Craps::BankStats&
CrapsInterface::playerAlltimeStatsBank(
    const Craps::PlayerId& playerId)
{
    Gen::ErrorPass ep;
    Craps::Player* p = Gbl::pPlayerMgr->getPlayer(playerId, ep);
    if (p == nullptr)
    {
        static Craps::BankStats empty;
        return empty;
    }
    return p->getBankAlltimeStats();
}

//----------------------------------------------------------------

const Craps::SessionHistory::Sessions&
CrapsInterface::playerSessionHistory(
    const Craps::PlayerId& playerId)
{
    Gen::ErrorPass ep;
    Craps::Player* p = Gbl::pPlayerMgr->getPlayer(playerId, ep);
    if (p == nullptr)
    {
        static Craps::SessionHistory::Sessions empty;
        return empty;
    }
    return p->getSessionHistory();
}

//----------------------------------------------------------------
//
// Bet related
//
//----------------------------------------------------------------

Gen::ReturnCode
CrapsInterface::betSetOddsAmount(
    const Craps::BetId& betId,
    Gen::Money oddsAmount,
    Gen::ErrorPass& ep)
{
    auto pBet = Gbl::pTable->getBet(betId);
    if (pBet == nullptr)
    {
        ep.diag = betDiag("betSetOddsAmount", "setOddsAmount", betId);
        return Gen::ReturnCode::Fail;
    }
    return pBet->player().setOddsAmount(pBet, oddsAmount, ep);
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsInterface::betSetContractAmount(
    const Craps::BetId& betId,
    Gen::Money contractAmount,
    Gen::ErrorPass& ep)
{
    auto pBet = Gbl::pTable->getBet(betId);
    if (pBet == nullptr)
    {
        ep.diag = betDiag("betSetContractAmount", "setContractAmount", betId);
        return Gen::ReturnCode::Fail;
    }
    return pBet->player().setContractAmount(betId, contractAmount, ep);
}

//----------------------------------------------------------------

bool
CrapsInterface::betIsRemovable(
    const Craps::BetId& betId)
{
    auto pBet = Gbl::pTable->getBet(betId);
    return Gbl::pTable->isBetRemovable(pBet);
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsInterface::betRemove(
    const Craps::BetId& betId,
    Gen::ErrorPass& ep)
{
    auto pBet = Gbl::pTable->getBet(betId);
    if (pBet == nullptr)
    {
        ep.diag = betDiag("betRemove", "remove bet", betId);
        return Gen::ReturnCode::Fail;
    }
    return pBet->player().removeBet(betId, ep);
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsInterface::betPivot(
    const Craps::BetId& betId,
    unsigned& pivot,
    Gen::ErrorPass& ep)
{
    auto pBet = Gbl::pTable->getBet(betId);
    if (pBet == nullptr)
    {
        ep.diag = betDiag("betPivot", "retrieve pivot", betId);
        return Gen::ReturnCode::Fail;
    }
    pivot = pBet->pivot();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsInterface::betPlayerId(
    const Craps::BetId& betId,
    Craps::PlayerId& playerId,
    Gen::ErrorPass& ep)
{
    auto pBet = Gbl::pTable->getBet(betId);
    if (pBet == nullptr)
    {
        ep.diag = betDiag("betPlayerId", "retrieve playerId from bet", betId);
        return Gen::ReturnCode::Fail;
    }
    playerId = pBet->player().getPlayerId();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsInterface::betName(
    const Craps::BetId& betId,
    BetName& betName,
    Gen::ErrorPass& ep)
{
    auto pBet = Gbl::pTable->getBet(betId);
    if (pBet == nullptr)
    {
        ep.diag = betDiag("betName", "retrieve bet name", betId);
        return Gen::ReturnCode::Fail;
    }
    betName = pBet->betName();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsInterface::betContractAmount(
    const Craps::BetId& betId,
    Gen::Money& contractAmount,
    Gen::ErrorPass& ep)
{
    auto pBet = Gbl::pTable->getBet(betId);
    if (pBet == nullptr)
    {
        ep.diag = betDiag("betContractAmount", "retrieve contract amount", betId);
        return Gen::ReturnCode::Fail;
    }
    contractAmount = pBet->contractAmount();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsInterface::betOddsAmount(
    const Craps::BetId& betId,
    Gen::Money& oddsAmount,
    Gen::ErrorPass& ep)
{
    auto pBet = Gbl::pTable->getBet(betId);
    if (pBet == nullptr)
    {
        ep.diag = betDiag("betContractAmount", "retrieve odds amount", betId);
        return Gen::ReturnCode::Fail;
    }
    oddsAmount = pBet->oddsAmount();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsInterface::betOffComeOutRoll(
    const Craps::BetId& betId,
    bool& offComeOutRoll,
    Gen::ErrorPass& ep)
{
    auto pBet = Gbl::pTable->getBet(betId);
    if (pBet == nullptr)
    {
        ep.diag = betDiag("betOffComeOutRoll", "retrieve comeout roll flag", betId);
        return Gen::ReturnCode::Fail;
    }
    offComeOutRoll = pBet->offComeOutRoll();
    return Gen::ReturnCode::Success;
    
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsInterface::betHardwayWorking(
    const Craps::BetId& betId,
    bool& hardwayWorking,
    Gen::ErrorPass& ep)
{
    auto pBet = Gbl::pTable->getBet(betId);
    if (pBet == nullptr)
    {
        ep.diag = betDiag("betHardwayWorking", "retrieve hardway working flag", betId);
        return Gen::ReturnCode::Fail;
    }
    hardwayWorking = pBet->hardwayWorking();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsInterface::betDistance(
    const Craps::BetId& betId,
    unsigned& distance,
    Gen::ErrorPass& ep)
{
    auto pBet = Gbl::pTable->getBet(betId);
    if (pBet == nullptr)
    {
        ep.diag = betDiag("betDistance", "retrieve distance", betId);
        return Gen::ReturnCode::Fail;
    }
    distance = pBet->distance();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsInterface::betWhenCreated(
    const Craps::BetId& betId,
    Gen::Timepoint& whenCreated,
    Gen::ErrorPass& ep)
{
    auto pBet = Gbl::pTable->getBet(betId);
    if (pBet == nullptr)
    {
        ep.diag = betDiag("betWhenCreated", "retrieve when created", betId);
        return Gen::ReturnCode::Fail;
    }
    whenCreated = pBet->whenCreated();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsInterface::betWhenDecided(
    const Craps::BetId& betId,
    Gen::Timepoint& whenDecided,
    Gen::ErrorPass& ep)
{
    auto pBet = Gbl::pTable->getBet(betId);
    if (pBet == nullptr)
    {
        ep.diag = betDiag("betWhenDecided", "retrieve when decided", betId);
        return Gen::ReturnCode::Fail;
    }
    whenDecided = pBet->whenDecided();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsInterface::betSetOffComeOutRoll(
    const Craps::BetId& betId,
    Gen::ErrorPass& ep)
{
    auto pBet = Gbl::pTable->getBet(betId);
    if (pBet == nullptr)
    {
        ep.diag = betDiag("betSetOffComeOutRoll", "set offComeOutRoll flag", betId);
        return Gen::ReturnCode::Fail;
    }
    pBet->setOffComeOutRoll();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsInterface::betSetOnComeOutRoll(
    const Craps::BetId& betId,
    Gen::ErrorPass& ep)
{
    auto pBet = Gbl::pTable->getBet(betId);
    if (pBet == nullptr)
    {
        ep.diag = betDiag("betSetOnComeOutRoll", "set onComeOutRoll flag", betId);
        return Gen::ReturnCode::Fail;
    }
    pBet->setOnComeOutRoll();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsInterface::betSetHardwayOff(
    const Craps::BetId& betId,
    Gen::ErrorPass& ep)
{
    auto pBet = Gbl::pTable->getBet(betId);
    if (pBet == nullptr)
    {
        ep.diag = betDiag("betSetHardwayOff", "set hardway off flag", betId);
        return Gen::ReturnCode::Fail;
    }
    pBet->setHardwayOff();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsInterface::betSetHardwayOn(
    const Craps::BetId& betId,
    Gen::ErrorPass& ep)
{
    auto pBet = Gbl::pTable->getBet(betId);
    if (pBet == nullptr)
    {
        ep.diag = betDiag("betSetHardwayOn", "set hardway on flag", betId);
        return Gen::ReturnCode::Fail;
    }
    pBet->setHardwayOn();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

// Private helper
std::string
CrapsInterface::betDiag(
    const std::string& funcName,
    const std::string& unableToWhat,
    const Craps::BetId& betId)
{
    std::string d = "CrapsInterface::" + funcName + "(): unable to " +
                    unableToWhat + "; No such betId: " + 
                    std::to_string(betId) + ".";
    return d;
}

//----------------------------------------------------------------
//
// Table related
//
//----------------------------------------------------------------

Gen::ReturnCode
CrapsInterface::tableAmountOnTable(
    const Craps::TableId& tableId,
    Gen::Money& amount,
    Gen::ErrorPass& ep)
{
    Craps::CrapsTable* pTable = Gbl::pTableMgr->getTable(tableId, ep);
    if (pTable == nullptr)
    {
        return Gen::ReturnCode::Fail;
    }
    amount = pTable->getAmountOnTable();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsInterface::tableNumBetsOnTable(
    const Craps::TableId& tableId,
    unsigned& numBets,
    Gen::ErrorPass& ep)
{
    Craps::CrapsTable* pTable = Gbl::pTableMgr->getTable(tableId, ep);
    if (pTable == nullptr)
    {
        return Gen::ReturnCode::Fail;
    }
    numBets = pTable->getNumBetsOnTable();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------
