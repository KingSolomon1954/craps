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
ViewEventHandlers::onViewRollDice(GameEvent* pBase)
{
    // TODO
}
    
//----------------------------------------------------------------

void
ViewEventHandlers::onViewRollDiceCountDown(GameEvent* pBase)
{
    // TODO
}
    
//----------------------------------------------------------------

void
ViewEventHandlers::onViewProgramExit(GameEvent* pBase)
{
    // TODO
}

//----------------------------------------------------------------
