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
// Enqueue generic ViewSuccess event to the EventLoop
//
void
ViewCommands::emitViewSuccess(
    uint64_t correlationId)
{
    auto ev = std::make_shared<ViewSuccess>();
    ev->correlationId = correlationId;

    Gbl::pGameCtrl->enqueue(ev);
}

//----------------------------------------------------------------
//
// Enqueue ViewErrorDialog event to the EventLoop
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
//
// Enqueue ViewMakeBetSuccess event to the EventLoop
//
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
//
// Enqueue ViewMakeOddsBetSuccess event to the EventLoop
//
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
// Enqueue ViewMakeBetAutoSuccess event to the EventLoop
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
// Enqueue ViewMakeBetAutoError event to the EventLoop
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
//
// Enqueue ViewRollDiceCountDown event to the EventLoop
//
void
ViewCommands::emitViewRollDiceCountDown(
    uint64_t correlationId,
    const Craps::TableId& tableId,
    int numSeconds)
{
    auto ev = std::make_shared<ViewRollDiceCountDown>();
    ev->correlationId = correlationId;
    ev->tableId       = tableId;
    ev->numSeconds    = numSeconds;

    Gbl::pGameCtrl->enqueue(ev);
}

//----------------------------------------------------------------
//
// Enqueue ViewRollDiceAnimation event to the EventLoop
//
void
ViewCommands::emitViewRollDiceAnimation()
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
