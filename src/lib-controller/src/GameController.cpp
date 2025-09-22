//----------------------------------------------------------------
//
// File: GameController.cpp
//
//----------------------------------------------------------------

#include <controller/GameController.h>
#include <cassert>
#include <iostream>
#include <controller/EventLoop.h>
#include <controller/CrapsEventHandlers.h>
#include <controller/ViewEventHandlers.h>

using namespace Ctrl;

/*-----------------------------------------------------------*//**

Constructor

*/
GameController::GameController()
    : eventLoop_([this](GameEvent* ev) { dispatchEvent(ev); })
{

}

//----------------------------------------------------------------

void
GameController::prepareForShutdown()
{
    eventLoop_.stop();
}

//----------------------------------------------------------------
//
// Enqueues an event to be processed.
//
// Called by both model and UI code.
//
void
GameController::enqueue(GameEvent::GameEventPtr ev)
{
    eventLoop_.enqueue(ev);
}

//----------------------------------------------------------------

void
GameController::dispatchEvent(GameEvent* pBase)
{
    switch(pBase->type())
    {
    case EventType::CmdMakeBet:             CrapsEventHandlers::onCmdMakeBet(pBase);            break;
    case EventType::CmdMakeBetAuto:         CrapsEventHandlers::onCmdMakeBetAuto(pBase);        break;
    case EventType::CmdMakeOddsBet:         CrapsEventHandlers::onCmdMakeOddsBet(pBase);        break;
    case EventType::CmdMakeOddsBetAuto:     CrapsEventHandlers::onCmdMakeOddsBetAuto(pBase);    break;
    case EventType::CmdRollDice:            CrapsEventHandlers::onCmdRollDice(pBase);           break;
    case EventType::ViewErrorDialog:        ViewEventHandlers::onViewErrorDialog(pBase);        break;
    case EventType::ViewMakeBetSuccess:     ViewEventHandlers::onViewMakeBetSuccess(pBase);     break;
    case EventType::ViewMakeOddsBetSuccess: ViewEventHandlers::onViewMakeOddsBetSuccess(pBase); break;
    case EventType::ViewRollDiceCountDown:  ViewEventHandlers::onViewRollDiceCountDown(pBase);  break;
    case EventType::ViewProgramExit:        ViewEventHandlers::onViewProgramExit(pBase);        break;
    }
}

//----------------------------------------------------------------

uint64_t
GameController::nextCorrelationId()
{
    return ++correlationId_;
}

//----------------------------------------------------------------
