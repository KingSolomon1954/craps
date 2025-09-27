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
#include <craps/CrapsTypes.h>
#include <craps/Player.h>
#include <gen/ErrorPass.h>

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
    // TODO
    // auto rc = model->playerMakeOddsBet()
    
    // if rc == success
    //     ViewEventEmmitters::emitMakeOddsBetSuccess(pBase->correlationId)
    // else
    //     ViewEventEmmitters::emitDialogError(pBase->correlationId)
}
    
//----------------------------------------------------------------
//
// Process event: CmdMakeOddsBet
// Tell model to make an odds bet, convert event
//
void
CrapsEventHandlers::onCmdBetSetOddsAmount(GameEvent* pBase)
{
    // TODO  CmdBetSetOddsAmount
    // auto rc = model->playerMakeOddsBet()
    
    // if rc == success
    //     ViewEventEmmitters::emitMakeOddsBetSuccess(pBase->correlationId)
    // else
    //     ViewEventEmmitters::emitDialogError(pBase->correlationId)
}
    
//----------------------------------------------------------------
//
// Process event: CmdMakeOddsBetAuto
// Tell model to make an odds bet, convert event
//
void
CrapsEventHandlers::onCmdBetSetOddsAmountAuto(GameEvent* pBase)
{
    // TODO
    // auto rc = model->playerMakeOddsBet()
    
    // if rc == success
    //     ViewEventEmmitters::emitMakeOddsBetSuccess(pBase->correlationId)
    // else
    //     ViewEventEmmitters::emitDialogError(pBase->correlationId)
}
    
//----------------------------------------------------------------
//
// Process event: CmdRollDice
// Tell model to roll dice, convert event
//
void
CrapsEventHandlers::onCmdRollDice(GameEvent* pBase)
{
    // TODO
    // auto rc = Gbl::pTable->rollDice()
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
