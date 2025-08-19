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

Gen::ReturnCode
CrapsInterface::joinTable(const Craps::PlayerId& playerId,
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
CrapsInterface::leaveTable(const Craps::PlayerId& playerId,
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
CrapsInterface::makeBet(const Craps::PlayerId& playerId,
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

Gen::ReturnCode
CrapsInterface::setOddsAmount(const Craps::PlayerId& playerId,
                              Craps::BetId betId,
                              Gen::Money oddsAmount,
                              Gen::ErrorPass& ep)
{
    Craps::Player* p = Gbl::pPlayerMgr->getPlayer(playerId, ep);
    if (p == nullptr)
    {
        ep.prepend("CrapsInterface::setOddsAmount(): unable to setOddsAmount; ");
        return Gen::ReturnCode::Fail;
    }
    Craps::BetPtr pBet = p->getBet(betId);
    return p->setOddsAmount(pBet, oddsAmount, ep);
}

//----------------------------------------------------------------

bool
CrapsInterface::isBetRemovable(const Craps::PlayerId& playerId,
                               Craps::BetId betId)
{
    Gen::ErrorPass ep;
    Craps::Player* p = Gbl::pPlayerMgr->getPlayer(playerId, ep);
    if (p == nullptr)
    {
        return false;
    }
    Craps::BetPtr b = p->getBet(betId);
    return Gbl::pTable->isBetRemovable(b);
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsInterface::removeBet(const Craps::PlayerId& playerId,
                          Craps::BetId betId,
                          Gen::ErrorPass& ep)
{
    Craps::Player* p = Gbl::pPlayerMgr->getPlayer(playerId, ep);
    if (p == nullptr)
    {
        ep.prepend("CrapsInterface::removeBet(): unable to remove bet; ");
        return Gen::ReturnCode::Fail;
    }
    return p->removeBet(betId, ep);
}

//----------------------------------------------------------------

const std::string&
CrapsInterface::getName(const Craps::PlayerId& playerId)
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
CrapsInterface::getAmountOnTablePlayer(
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
CrapsInterface::getNumBetsOnTablePlayer(
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

const Craps::LastRollStats&
CrapsInterface::getLastRollStats(const Craps::PlayerId& playerId)
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
CrapsInterface::getCurrentStatsPlayer(const Craps::PlayerId& playerId)
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
CrapsInterface::getAlltimeStatsPlayer(const Craps::PlayerId& playerId)
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
CrapsInterface::getBankCurrentStatsPlayer(const Craps::PlayerId& playerId)
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
CrapsInterface::getBankAlltimeStatsPlayer(const Craps::PlayerId& playerId)
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
CrapsInterface::getSessionHistoryPlayer(const Craps::PlayerId& playerId)
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
// Table related
//
//----------------------------------------------------------------

Gen::ReturnCode
CrapsInterface::getAmountOnTable(const Craps::TableId& tableId,
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
CrapsInterface::getNumBetsOnTable(const Craps::TableId& tableId,
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
