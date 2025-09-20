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
ViewEventEmitters::emitViewErrorDialog(
    EventType orgEventType,
    uint64_t correlationId,
    const std::string& diag)
{
    // TODO
    // Notify menus/screens of an error, usually puts up an ErrorDialog
    // uiEvent.correlationId = pBase->correlationId;
    // uiEventMgr->publish(uiEvent);
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
