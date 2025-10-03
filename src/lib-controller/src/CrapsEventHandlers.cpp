//----------------------------------------------------------------
//
// File: CrapsEventHandlers.cpp
//
//----------------------------------------------------------------

#include <controller/CrapsEventHandlers.h>
#include <controller/Globals.h>
#include <controller/PlayerManager.h>
#include <controller/TableManager.h>
#include <controller/UndoManager.h>
#include <controller/ViewCommands.h>
#include <craps/CrapsTable.h>
#include <craps/CrapsTypes.h>
#include <craps/Player.h>
#include <gen/ErrorPass.h>
#include <cassert>

using namespace Ctrl;

//----------------------------------------------------------------
//
// Process event: CmdPlayerJoinTable
// Tells craps engine that player wants to join a table
//
void
CrapsEventHandlers::onCmdPlayerJoinTable(GameEvent* pBase)
{
    auto* ev = dynamic_cast<CmdPlayerJoinTable*>(pBase);

    Gen::ErrorPass ep;
    Gen::ReturnCode rc;

    Craps::Player* p = Gbl::pPlayerMgr->getPlayer(ev->playerId, ep);
    if (p != nullptr)
    {
        auto pTable = Gbl::pTableMgr->getTable(ev->tableId, ep);
        if (pTable == nullptr)
        {
            rc = p->joinTable(pTable, ep);
        }
    }

    if (p == nullptr || rc == Gen::ReturnCode::Fail)
    {
        // Tell UI of fault
        ep.prepend(diagPrefix("onCmdPlayerJoinTable", "join table"));
        ViewCommands::emitViewErrorDialog(
            EventType::CmdPlayerJoinTable,
            ev->correlationId,
            ep.diag);
        return;
    }

    // Tell UI of success
    ViewCommands::emitViewSuccess(ev->correlationId);
}

//----------------------------------------------------------------
//
// Process event: CmdPlayerLeaveTable
// Tells craps engine that player wants to join a table
//
void
CrapsEventHandlers::onCmdPlayerLeaveTable(GameEvent* pBase)
{
    auto* ev = dynamic_cast<CmdPlayerLeaveTable*>(pBase);

    Gen::ErrorPass ep;
    Gen::ReturnCode rc;

    Craps::Player* p = Gbl::pPlayerMgr->getPlayer(ev->playerId, ep);
    if (p != nullptr)
    {
        rc = p->leaveTable(ep);
    }
    else
    {
        // Tell UI of fault
        ep.prepend(diagPrefix("onCmdPlayerLeaveTable", "join table"));
        ViewCommands::emitViewErrorDialog(
            EventType::CmdPlayerLeaveTable,
            ev->correlationId,
            ep.diag);
        return;
    }

    // Tell UI of success
    ViewCommands::emitViewSuccess(ev->correlationId);
}

//----------------------------------------------------------------
//
// Process event: CmdMakeBet
// Tells craps engine to make a bet
//
void
CrapsEventHandlers::onCmdMakeBet(GameEvent* pBase)
{
    auto* ev = dynamic_cast<CmdMakeBet*>(pBase);

    Gen::ErrorPass ep;
    Craps::BetPtr pBet;

    Craps::Player* p = Gbl::pPlayerMgr->getPlayer(ev->playerId, ep);
    if (p != nullptr)
    {
        pBet = p->makeBet(ev->betName,
                          ev->contractAmount,
                          ev->pivot,
                          ep);
    }

    if (p == nullptr || pBet == nullptr)
    {
        // Tell UI of fault
        ep.prepend(diagPrefix("onPlayerMakeBet", "make bet"));
        ViewCommands::emitViewErrorDialog(
            EventType::CmdMakeBet,
            ev->correlationId,
            ep.diag);
        return;
    }

    // Success path
    Gbl::pUndoMgr->push(std::make_unique<UndoBetAdded>(pBet));

    // Tell UI of success
    ViewCommands::emitViewMakeBetSuccess(ev->correlationId, pBet->betId());
}

//----------------------------------------------------------------
//
// Process event: CmdMakeBetAuto
// Tells craps engine to make a bet
//
void
CrapsEventHandlers::onCmdMakeBetAuto(GameEvent* pBase)
{
    auto* ev = dynamic_cast<CmdMakeBetAuto*>(pBase);

    Gen::ErrorPass ep;
    Craps::Player* p = Gbl::pPlayerMgr->getPlayer(ev->playerId, ep);
    if (!p)
    {
        ViewCommands::emitViewMakeBetAutoError(
            EventType::CmdMakeBet,
            ev->correlationId,
            ev->playerId,
            "Player not found");
        return;
    }

    auto pBet = p->makeBet(ev->betName, ev->contractAmount, ev->pivot, ep);
    if (!pBet)
    {
        ViewCommands::emitViewMakeBetAutoError(
            EventType::CmdMakeBetAuto,
            ev->correlationId,
            ev->playerId,
            ep.diag);
        return;
    }

    ViewCommands::emitViewMakeBetAutoSuccess(
        pBet->betId(), ev->correlationId);
}

//----------------------------------------------------------------
//
// Process event: CmdBetSetContractAmount
// Tell model to set/change the contract amount of a bet
//
void
CrapsEventHandlers::onCmdBetSetContractAmount(GameEvent* pBase)
{
    auto* ev = dynamic_cast<CmdBetSetContractAmount*>(pBase);
    
    Gen::ErrorPass ep;
    Gen::ReturnCode rc = Gen::ReturnCode::Fail;
    
    auto pBet = Gbl::pTable->getBet(ev->betId, ep);
    if (pBet != nullptr)
    {
        rc = pBet->player().setContractAmount(ev->betId, ev->contractAmount, ep);
    }

    if (pBet == nullptr || rc == Gen::ReturnCode::Fail)
    {
        // Tell UI of fault
        ep.prepend(diagPrefix("onCmdBetSetContractAmount", "setContractAmount"));
        ViewCommands::emitViewErrorDialog(
            EventType::CmdBetSetContractAmount,
            ev->correlationId,
            ep.diag);
        return;
    }
    
    Gbl::pUndoMgr->push(std::make_unique<UndoBetModifiedAmount>(pBet));
    ViewCommands::emitViewSuccess(ev->correlationId);
}
    
//----------------------------------------------------------------
//
// Process event: CmdMakeOddsBet
// Tell model to make an odds bet, convert event
//
void
CrapsEventHandlers::onCmdBetSetOddsAmount(GameEvent* pBase)
{
    auto* ev = dynamic_cast<CmdBetSetOddsAmount*>(pBase);
    
    Gen::ErrorPass ep;
    Gen::ReturnCode rc = Gen::ReturnCode::Fail;
    
    auto pBet = Gbl::pTable->getBet(ev->betId, ep);
    if (pBet != nullptr)
    {
        rc = pBet->player().setOddsAmount(pBet, ev->oddsAmount, ep);
    }

    if (pBet == nullptr || rc == Gen::ReturnCode::Fail)
    {
        // Tell UI of fault
        ep.prepend(diagPrefix("onCmdBetSetOddsAmount", "setOddsAmount"));
        ViewCommands::emitViewErrorDialog(
            EventType::CmdBetSetOddsAmount,
            ev->correlationId,
            ep.diag);
        return;
    }
    
    Gbl::pUndoMgr->push(std::make_unique<UndoBetModifiedAmount>(pBet));
    ViewCommands::emitViewSuccess(ev->correlationId);
}
    
//----------------------------------------------------------------
//
// Process event: CmdBetSetOffComeOutRoll
// Tell model to make an odds bet, convert event
//
void
CrapsEventHandlers::onCmdBetSetOffComeOutRoll(GameEvent* pBase)
{
    auto* ev = dynamic_cast<CmdBetSetOffComeOutRoll*>(pBase);
    
    Gen::ErrorPass ep;
    
    auto pBet = Gbl::pTable->getBet(ev->betId, ep);
    if (pBet == nullptr)
    {
        ep.prepend(diagPrefix("onCmdBetSetOffComeOutRoll", "setOffComeOutRoll"));
        ViewCommands::emitViewErrorDialog(
            EventType::CmdBetSetOddsAmount,
            ev->correlationId,
            ep.diag);
        return;
    }
    
    pBet->setOnComeOutRoll();
    ViewCommands::emitViewSuccess(ev->correlationId);
    Gbl::pUndoMgr->push(std::make_unique<UndoBetModifiedFlags>(pBet));
}
    
//----------------------------------------------------------------
//
// Process event: CmdBetSetOnComeOutRoll
// Tell model to make an odds bet, convert event
//
void
CrapsEventHandlers::onCmdBetSetOnComeOutRoll(GameEvent* pBase)
{
    auto* ev = dynamic_cast<CmdBetSetOnComeOutRoll*>(pBase);
    
    Gen::ErrorPass ep;
    
    auto pBet = Gbl::pTable->getBet(ev->betId, ep);
    if (pBet == nullptr)
    {
        ep.prepend(diagPrefix("onCmdBetSetOnComeOutRoll", "setOnComeOutRoll"));
        ViewCommands::emitViewErrorDialog(
            EventType::CmdBetSetOddsAmount,
            ev->correlationId,
            ep.diag);
        return;
    }
    
    pBet->setOffComeOutRoll();
    ViewCommands::emitViewSuccess(ev->correlationId);
    Gbl::pUndoMgr->push(std::make_unique<UndoBetModifiedFlags>(pBet));
}
    
//----------------------------------------------------------------
//
// Process event: CmdBetSetHardwayOff
// Tell model to make an odds bet, convert event
//
void
CrapsEventHandlers::onCmdBetSetHardwayOff(GameEvent* pBase)
{
    auto* ev = dynamic_cast<CmdBetSetHardwayOff*>(pBase);
    
    Gen::ErrorPass ep;
    
    auto pBet = Gbl::pTable->getBet(ev->betId, ep);
    if (pBet == nullptr)
    {
        ep.prepend(diagPrefix("onCmdBetSetHardwayOff", "setHardwayOff"));
        ViewCommands::emitViewErrorDialog(
            EventType::CmdBetSetHardwayOff,
            ev->correlationId,
            ep.diag);
        return;
    }
    
    pBet->setHardwayOff();
    ViewCommands::emitViewSuccess(ev->correlationId);
    Gbl::pUndoMgr->push(std::make_unique<UndoBetModifiedFlags>(pBet));
}
    
//----------------------------------------------------------------
//
// Process event: CmdBetSetHardwayOn
// Tell model to make an odds bet, convert event
//
void
CrapsEventHandlers::onCmdBetSetHardwayOn(GameEvent* pBase)
{
    auto* ev = dynamic_cast<CmdBetSetHardwayOn*>(pBase);
    
    Gen::ErrorPass ep;
    
    auto pBet = Gbl::pTable->getBet(ev->betId, ep);
    if (pBet == nullptr)
    {
        ep.prepend(diagPrefix("onCmdBetSetHardwayOn", "setHardwayOn"));
        ViewCommands::emitViewErrorDialog(
            EventType::CmdBetSetHardwayOn,
            ev->correlationId,
            ep.diag);
        return;
    }
    
    pBet->setHardwayOff();
    ViewCommands::emitViewSuccess(ev->correlationId);
    Gbl::pUndoMgr->push(std::make_unique<UndoBetModifiedFlags>(pBet));
}
    
//----------------------------------------------------------------
//
// Process event: CmdRollDice
// Tell model to roll dice, convert event
//
void
CrapsEventHandlers::onCmdRollDice(GameEvent* pBase)
{
    auto* ev = dynamic_cast<CmdRollDice*>(pBase);
    
    Gen::ErrorPass ep;
    auto pTable = Gbl::pTableMgr->getTable(ev->tableId, ep);
    if (pTable == nullptr)
    {
        ep.prepend(diagPrefix("onCmdRollDice", "roll dice"));
        ViewCommands::emitViewErrorDialog(
            EventType::CmdRollDice,
            ev->correlationId,
            ep.diag);
        return;
    }
    pTable->rollDice();
    Gbl::pUndoMgr->clear();
    ViewCommands::emitViewSuccess(ev->correlationId);
}
    
//----------------------------------------------------------------
//
// Private helper
//
std::string
CrapsEventHandlers::diagPrefix(
    const std::string& funcName,
    const std::string& unableToWhat)
{
    std::string d = "CrapsEventHandler::" + funcName + 
                    "(): unable to " + unableToWhat + "; ";
    return d;
}

//----------------------------------------------------------------
