//----------------------------------------------------------------
//
// File: CrapsEventHandlers.cpp
//
//----------------------------------------------------------------

#include <controller/CrapsEventHandlers.h>
#include <controller/Globals.h>
#include <controller/PlayerManager.h>
#include <controller/UndoManager.h>
#include <controller/ViewCommands.h>
#include <craps/CrapsTypes.h>
#include <craps/Player.h>
#include <gen/ErrorPass.h>

using namespace Ctrl;

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
// Process event: CmdMakeOddsBet
// Tell model to make an odds bet, convert event
//
void
CrapsEventHandlers::onCmdMakeOddsBet(GameEvent* pBase)
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
// Process event: CmdMakeOddsBetAuto
// Tell model to make an odds bet, convert event
//
void
CrapsEventHandlers::onCmdMakeOddsBetAuto(GameEvent* pBase)
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
