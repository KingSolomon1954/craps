//----------------------------------------------------------------
//
// File: ViewEventHandlers.cpp
//
//----------------------------------------------------------------

#include <controller/ViewEventHandlers.h>
#include <controller/GameEvent.h>

using namespace Ctrl;

//----------------------------------------------------------------
//
// Tell UI to display ViewErrorDialog
//
void
ViewEventHandlers::onViewErrorDialog(GameEvent* pBase)
{
    auto* ev = dynamic_cast<ViewErrorDialog*>(pBase);
    // TODO
    // Notify menus/screens of an error, usually puts up an ErrorDialog
    // ev->orgEventType;
    // ev->correlationId;
    // ev->diag;

    // uiEvent.correlationId = ev->correlationId;
    // uiEvent.orgEventType = ev->orgEventType;
    // uiEvent.diag = ev->diag;
    // uiEventMgr->publish(uiEvent);
}
    
//----------------------------------------------------------------

void
ViewEventHandlers::onViewMakeBetSuccess(GameEvent* pBase)
{
    // TODO
    // Gbl::pView->
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
ViewEventHandlers::onSignalProgramExit(GameEvent* pBase)
{
    // TODO
}

//----------------------------------------------------------------
