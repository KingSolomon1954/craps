//----------------------------------------------------------------
//
// File: ViewEventHandlers.cpp
//
//----------------------------------------------------------------

#include <controller/ViewEventHandlers.h>
#include <controller/GameEvent.h>
#include <controller/Globals.h>
#include <controller/ViewInterface.h>

using namespace Ctrl;

//----------------------------------------------------------------
//
// Tell UI that a general Craps command was successful
//
void
ViewEventHandlers::onViewSuccess(GameEvent* pBase)
{
    auto* ev = dynamic_cast<ViewErrorDialog*>(pBase);
    Gbl::pView->showSuccess(ev->orgEventType, ev->correlationId);
}
    
//----------------------------------------------------------------
//
// Tell UI to display ErrorDialog
//
void
ViewEventHandlers::onViewErrorDialog(GameEvent* pBase)
{
    auto* ev = dynamic_cast<ViewErrorDialog*>(pBase);
    Gbl::pView->showErrorDialog(ev->orgEventType,
                                ev->correlationId,
                                ev->diag);
}
    
//----------------------------------------------------------------

void
ViewEventHandlers::onViewMakeBetSuccess(GameEvent* pBase)
{
    auto* ev = dynamic_cast<ViewMakeBetSuccess*>(pBase);
    Gbl::pView->showMakeBetSuccess(ev->betId, ev->correlationId);
}
    
//----------------------------------------------------------------

void
ViewEventHandlers::onViewMakeBetAutoSuccess(GameEvent* pBase)
{
    auto* ev = dynamic_cast<ViewMakeBetSuccess*>(pBase);
    Gbl::pView->showMakeBetSuccess(ev->betId, ev->correlationId);
}
    
//----------------------------------------------------------------

void
ViewEventHandlers::onViewMakeBetAutoError(GameEvent* pBase)
{
    auto* ev = dynamic_cast<ViewMakeBetAutoError*>(pBase);
    Gbl::pView->showMakeBetAutoError(
        ev->orgEventType,
        ev->correlationId,
        ev->playerId,
        ev->diag);
}
    
//----------------------------------------------------------------

void
ViewEventHandlers::onViewMakeOddsBetSuccess(GameEvent* pBase)
{
    // TODO
}
    
//----------------------------------------------------------------

void
ViewEventHandlers::onViewRollDiceCountDown(GameEvent* pBase)
{
    auto* ev = dynamic_cast<ViewRollDiceCountDown*>(pBase);
    Gbl::pView->showRollDiceCountDown(ev->correlationId, ev->numSeconds);
}
    
//----------------------------------------------------------------

void
ViewEventHandlers::onViewRollDiceAnimation(GameEvent* pBase)
{
    auto* ev = dynamic_cast<ViewRollDiceAnimation*>(pBase);
    Gbl::pView->showRollDiceAnimation(ev->correlationId);
}
    
//----------------------------------------------------------------

void
ViewEventHandlers::onViewProgramExit(GameEvent* pBase)
{
    // TODO
}

//----------------------------------------------------------------
