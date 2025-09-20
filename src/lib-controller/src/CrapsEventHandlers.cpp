//----------------------------------------------------------------
//
// File: CrapsEventHandlers.cpp
//
//----------------------------------------------------------------

#include <controller/CrapsEventHandlers.h>
#include <controller/Globals.h>
#include <controller/PlayerManager.h>
#include <controller/UndoManager.h>
#include <controller/ViewEventEmitters.h>
#include <craps/CrapsTypes.h>
#include <craps/Player.h>
#include <gen/ErrorPass.h>

using namespace Ctrl;

//----------------------------------------------------------------
//
// Process event: PlayerMakeBet
// Tells craps engine to make a bet
//
void
CrapsEventHandlers::onPlayerMakeBet(GameEvent* pBase)
{
    auto* ev = dynamic_cast<PlayerMakeBet*>(pBase);

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
        ViewEventEmitters::emitViewErrorDialog(
            EventType::PlayerMakeBet,
            ev->correlationId,
            ep.diag);
        return;
    }

    // Success path
    Gbl::pUndoMgr->push(std::make_unique<UndoBetAdded>(pBet));

    // Tell UI of success
    ViewEventEmitters::emitViewMakeBetSuccess(
        pBet->betId(),
        ev->correlationId);
}

//----------------------------------------------------------------
//
// Tell model to make an odds bet, convert event
//
void
CrapsEventHandlers::onPlayerMakeOddsBet(GameEvent* pBase)
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
// Tell model to roll dice, convert event
//
void
CrapsEventHandlers::onPlayerRollDice(GameEvent* pBase)
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


