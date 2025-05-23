//----------------------------------------------------------------
//
// File: GameController.cpp
//
//----------------------------------------------------------------

#include <controller/GameController.h>
#include <controller/TableManager.h>
#include <controller/ViewIntfc.h>
#include <craps/CrapsTable.h>
#include <iostream>

using namespace Ctrl;

/*-----------------------------------------------------------*//**

Constructor

*/
GameController::GameController(std::shared_ptr<ViewIntfc> pView)
    : pView_(std::move(pView))
{
    pView_->setGameController(this);
}

//----------------------------------------------------------------

Craps::CrapsTable*
GameController::userSelectsTable()
{
    auto tds = Gbl::pTableMgr->loadTableChoices();
    auto tid = pView_->promptUserToSelectTable(tds);
    
    auto pTable = Gbl::pTableMgr->loadCrapsTable(tid);
    Gbl::pTable = pTable;
    return pTable;
}

//----------------------------------------------------------------

void
GameController::userSelectsPlayers()
{
    auto pds = Gbl::pPlayerMgr->loadPlayerChoices();
    auto playerIds = pView_->promptUserToSelectPlayers(pds);
    
    Gen::ErrorPass ep;
    for (auto pid : playerIds)  // Players join table
    {
        // TODO: check error return
        Gbl::pTable->addPlayer(pid, ep);
    }
}

//----------------------------------------------------------------

void
GameController::rollDice()
{
    Gbl::pTable->rollDice();
}

//----------------------------------------------------------------

void
GameController::onAboutCraps()
{
    // TODO
    // pView_->displayAbout();
}

//----------------------------------------------------------------
