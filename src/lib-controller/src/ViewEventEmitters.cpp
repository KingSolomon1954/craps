//----------------------------------------------------------------
//
// File: ViewEventEmitters.cpp
//
//----------------------------------------------------------------

#include <controller/ViewEventEmitters.h>

using namespace Ctrl;

//----------------------------------------------------------------
//
// Enqueue ViewErrorDialog event to GameController
//
void
ViewEventEmitters::emitViewErrorDialog()
{
    // TODO
    // Notify menus/screens of an error, usually puts up an ErrorDialog
    // uiEvent.correlationId = pBase->correlationId;
    // uiEventMgr->publish(uiEvent);
}
    
//----------------------------------------------------------------

void
ViewEventEmitters::emitViewMakeBetSuccess()
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
