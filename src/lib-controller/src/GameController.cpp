//----------------------------------------------------------------
//
// File: GameController.cpp
//
//----------------------------------------------------------------

#include <controller/GameController.h>
#include <controller/TableManager.h>
#include <controller/ViewIntfc.h>
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

void
GameController::userSelectsTableAndPlayers()
{
    auto tds = TableManager::loadTableChoices();
    auto tid = pView_->promptUserToSelectTable(tds);

    auto pds = PlayerManager::loadPlayerChoices();
    auto playerIds = pView_->promptUserToSelectPlayers(pds);
    table_ = Craps::CrapsTable();

    Gen::ErrorPass ep;
    for (auto pid : playerIds)  // Players join table
    {
        // TODO: check error return
        table_.addPlayer(pid, ep);
    }
}

//----------------------------------------------------------------

void
GameController::rollDice()
{
    // TODO
    // table_.rollDice();
}

//----------------------------------------------------------------

void
GameController::onAboutCraps()
{
    // TODO
    // pView_->displayAbout();
}

//----------------------------------------------------------------
