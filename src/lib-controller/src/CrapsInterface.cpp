//----------------------------------------------------------------
//
// File: CrapsInterface.cpp
//
//----------------------------------------------------------------

#include <controller/CrapsInterface.h>
#include <controller/Globals.h>
#include <controller/PlayerManager.h>
#include <craps/BankStats.h>
#include <craps/CrapsTable.h>
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
