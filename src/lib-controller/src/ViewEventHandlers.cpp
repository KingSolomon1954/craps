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

    Gbl::pView->onViewErrorDialog(ev->orgEventType,
                                  ev->correlationId,
                                  ev->diag);
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
