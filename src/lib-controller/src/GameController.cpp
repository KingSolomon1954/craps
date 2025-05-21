//----------------------------------------------------------------
//
// File: GameController.cpp
//
//----------------------------------------------------------------

#include <controller/GameController.h>
#include <cassert>
#include <controller/ViewIntfc.h>

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
GameController::chooseTableAndPlayers()
{
    // first orchestrate table and player selection
    // create craps table
    // have players join table
    table_ = Craps::CrapsTable();
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
