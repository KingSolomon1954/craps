//----------------------------------------------------------------
//
// File: ViewEventHandlers.cpp
//
//----------------------------------------------------------------

#include <controller/ViewEventHandlers.h>

using namespace Ctrl;

//----------------------------------------------------------------
//
// Tell UI to display ViewErrorDialog
//
void
ViewEventHandlers::onViewErrorDialog(GameEvent* pBase)
{
    // TODO
    // Notify menus/screens of an error, usually puts up an ErrorDialog
    // uiEvent.correlationId = pBase->correlationId;
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
