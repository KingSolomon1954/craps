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
    eventLoop_.enqueue(std::move(ev));
}

//----------------------------------------------------------------

void
GameController::dispatchEvent(GameEvent* pBase)
{
    switch(pBase->type())
    {
    case EventType::CmdPlayerJoinTable:      CrapsEventHandlers::onCmdPlayerJoinTable(pBase);      break;
    case EventType::CmdPlayerLeaveTable:     CrapsEventHandlers::onCmdPlayerLeaveTable(pBase);     break;
    case EventType::CmdMakeBet:              CrapsEventHandlers::onCmdMakeBet(pBase);              break;
    case EventType::CmdMakeBetAuto:          CrapsEventHandlers::onCmdMakeBetAuto(pBase);          break;
    case EventType::CmdBetSetContractAmount: CrapsEventHandlers::onCmdBetSetContractAmount(pBase); break;
    case EventType::CmdBetSetOddsAmount:     CrapsEventHandlers::onCmdBetSetOddsAmount(pBase);     break;
    case EventType::CmdBetSetOffComeOutRoll: CrapsEventHandlers::onCmdBetSetOffComeOutRoll(pBase); break;
    case EventType::CmdBetSetOnComeOutRoll:  CrapsEventHandlers::onCmdBetSetOnComeOutRoll(pBase);  break;
    case EventType::CmdRollDice:             CrapsEventHandlers::onCmdRollDice(pBase);             break;
    case EventType::ViewErrorDialog:         ViewEventHandlers::onViewErrorDialog(pBase);          break;
    case EventType::ViewMakeBetSuccess:      ViewEventHandlers::onViewMakeBetSuccess(pBase);       break;
    case EventType::ViewMakeOddsBetSuccess:  ViewEventHandlers::onViewMakeOddsBetSuccess(pBase);   break;
    case EventType::ViewRollDiceCountDown:   ViewEventHandlers::onViewRollDiceCountDown(pBase);    break;
    case EventType::ViewProgramExit:         ViewEventHandlers::onViewProgramExit(pBase);          break;
    }
}

//----------------------------------------------------------------

uint64_t
GameController::nextCorrelationId()
{
    return ++correlationId_;
}

//----------------------------------------------------------------
