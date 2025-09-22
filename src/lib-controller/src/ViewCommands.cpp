//----------------------------------------------------------------
//
// File: ViewCommands.cpp
//
//----------------------------------------------------------------

#include <controller/ViewCommands.h>
#include <controller/GameController.h>
#include <controller/Globals.h>

using namespace Ctrl;

//----------------------------------------------------------------
//
// Enqueue ViewErrorDialog event to GameController
//
void
ViewCommands::emitViewErrorDialog(
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

void
ViewCommands::emitViewMakeBetSuccess(
    uint64_t correlationId,
    Craps::BetId betId)
{
    auto ev = std::make_shared<ViewMakeBetSuccess>();
    ev->correlationId = correlationId;
    ev->betId          = betId;

    Gbl::pGameCtrl->enqueue(ev);
}
    
//----------------------------------------------------------------

void
ViewCommands::emitViewMakeOddsBetSuccess(
    uint64_t correlationId,
    Craps::BetId betId)
{
    auto ev = std::make_shared<ViewMakeOddsBetSuccess>();
    ev->correlationId = correlationId;
    ev->betId          = betId;

    Gbl::pGameCtrl->enqueue(ev);
}
    
//----------------------------------------------------------------
//
// Enqueue ViewPlayerBetPlaced event to GameController
//
void
ViewCommands::emitViewMakeBetAutoSuccess(
    Craps::BetId betId,
    uint64_t correlationId)
{
    auto ev = std::make_shared<ViewMakeBetAutoSuccess>();
    ev->correlationId = correlationId;
    ev->betId  = betId;

    Gbl::pGameCtrl->enqueue(ev);
}

//----------------------------------------------------------------
//
// Enqueue ViewMakeBetAutoError event to GameController
//
void
ViewCommands::emitViewMakeBetAutoError(
    EventType              orgEventType,
    uint64_t               correlationId,
    const Craps::PlayerId& playerId,
    const std::string&     diag)
{
    auto ev = std::make_shared<ViewMakeBetAutoError>();
    ev->orgEventType  = orgEventType;
    ev->correlationId = correlationId;
    ev->playerId      = playerId;
    ev->diag          = diag;

    Gbl::pGameCtrl->enqueue(ev);
}

//----------------------------------------------------------------

void
ViewCommands::emitViewRollDice()
{
    // TODO
}
    
//----------------------------------------------------------------

void
ViewCommands::emitViewProgramExit()
{
    // TODO
}

//----------------------------------------------------------------
