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
GameController::handleAllEvents(const GameEvent::GameEventPtr& ev)
{
    switch(ev->type())
    {
        case EventType::UserInputLine: ;
        case EventType::UserInputChar: ;
        case EventType::Timer: ;
        case EventType::Signal: ;
    }
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
