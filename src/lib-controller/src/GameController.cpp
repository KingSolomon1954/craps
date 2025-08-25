//----------------------------------------------------------------
//
// File: GameController.cpp
//
//----------------------------------------------------------------

#include <controller/GameController.h>
#include <cassert>
#include <iostream>
#include <controller/EventLoop.h>
#include <controller/Globals.h>
#include <controller/ViewIntfc.h>

using namespace Ctrl;

/*-----------------------------------------------------------*//**

Constructor

*/
GameController::GameController()
    : eventLoop_([this](GameEvent* ev) { dispatchEvent(ev); })
{
    assert(Gbl::pView != nullptr);
}

//----------------------------------------------------------------

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
    case EventType::UserInputLine: onUserInputLine(pBase); break;
    case EventType::UserInputChar: ; break;
    case EventType::Timer:         ; break;
    case EventType::Signal:        ; break;
    }
}

//----------------------------------------------------------------

void
GameController::onUserInputLine(GameEvent* pBase)
{
    auto pEvent = static_cast<UserInputLineEvent*>(pBase);
    std::cout << "[InputLine] " << pEvent->input << '\n';
    if (pEvent->input == "quit") eventLoop_.stop();
}

//----------------------------------------------------------------

void
GameController::onUserInputChar(GameEvent* pBase)
{
    auto pEvent = static_cast<UserInputCharEvent*>(pBase);
    std::cout << "[InputChar] " << pEvent->input << '\n';
    if (pEvent->input == 'q') eventLoop_.stop();
}

//----------------------------------------------------------------
//
// Not used.
// Pursue different design for choosing players
//
#if 0
void
GameController::userSelectsPlayers()
{
    auto playerIds = Gbl::pView->promptUserToSelectPlayers();
    
    Gen::ErrorPass ep;
    for (auto pid : playerIds)  // Players join table
    {
        // TODO: check error return
        // Gbl::pTable->addPlayer(pid, ep);
    }
}
#endif

//----------------------------------------------------------------
