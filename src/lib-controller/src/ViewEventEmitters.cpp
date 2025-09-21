//----------------------------------------------------------------
//
// File: ViewEventEmitters.cpp
//
//----------------------------------------------------------------

#include <controller/ViewEventEmitters.h>
#include <controller/GameController.h>
#include <controller/Globals.h>

using namespace Ctrl;

//----------------------------------------------------------------
//
// Enqueue ViewErrorDialog event to GameController
//
void
ViewEventEmitters::emitViewErrorDialog(
    EventType          orgEventType,
    uint64_t           correlationId,
    const std::string& diag)
{
    auto ev = std::make_shared<ViewErrorDialog>();
    ev->orgEventType  = orgEventType;
    ev->correlationId = correlationId;
    ev->diag          = diag;

    Gbl::pGameCtrl->enqueue(ev);
}

//----------------------------------------------------------------
//
// Enqueue ViewAutomationBetError event to GameController
//
void
ViewEventEmitters::emitViewAutomationMakeBetError(
    EventType              orgEventType,
    uint64_t               correlationId,
    const Craps::PlayerId& playerId,
    const std::string&     diag)
{
    auto ev = std::make_shared<ViewAutomationMakeBetError>();
    ev->orgEventType  = orgEventType;
    ev->correlationId = correlationId;
    ev->playerId      = playerId;
    ev->diag          = diag;

    Gbl::pGameCtrl->enqueue(ev);
}

//----------------------------------------------------------------

void
ViewEventEmitters::emitViewMakeBetSuccess(
    Craps::BetId betId,
    uint64_t correlationId)
{
    // TODO
}
    
//----------------------------------------------------------------

void
ViewEventEmitters::emitViewMakeOddsBetSuccess()
{
    // TODO
}
    
//----------------------------------------------------------------
//
// Enqueue ViewPlayerBetPlaced event to GameController
//
void
ViewEventEmitters::emitViewAutomationMakeBetSuccess(
    Craps::BetId betId,
    uint64_t correlationId)
{
    auto ev = std::make_shared<ViewAutomationMakeBetSuccess>();
    ev->orgEventType  = EventType::AutomationMakeBet;
    ev->correlationId = correlationId;

    Gbl::pGameCtrl->enqueue(ev);
}

//----------------------------------------------------------------

void
ViewEventEmitters::emitViewRollDice()
{
    // TODO
}
    
//----------------------------------------------------------------

void
ViewEventEmitters::emitSignalProgramExit()
{
    // TODO
}

//----------------------------------------------------------------
