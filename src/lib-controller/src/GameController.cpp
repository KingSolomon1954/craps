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
#include <controller/TableManager.h>
#include <controller/ViewIntfc.h>
#include <craps/CrapsTable.h>

using namespace Ctrl;

/*-----------------------------------------------------------*//**

Constructor

*/
GameController::GameController()
{
    assert(Gbl::pView != nullptr);
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
    if (pEvent->input == "quit") Gbl::pEventLoop->stop();
}

//----------------------------------------------------------------
//
// Not used.
// Pursue different design for choosing table
//
void
GameController::userSelectsTable()
{
    auto tds = Gbl::pTableMgr->loadTableChoices();
    auto tid = Gbl::pView->promptUserToSelectTable(tds);
}

//----------------------------------------------------------------
//
// Not used.
// Pursue different design for choosing players
//
void
GameController::userSelectsPlayers()
{
    auto pds = Gbl::pPlayerMgr->loadPlayerChoices();
    auto playerIds = Gbl::pView->promptUserToSelectPlayers(pds);
    
    Gen::ErrorPass ep;
    for (auto pid : playerIds)  // Players join table
    {
        // TODO: check error return
        Gbl::pTable->addPlayer(pid, ep);
    }
}

//----------------------------------------------------------------
