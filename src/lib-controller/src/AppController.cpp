//----------------------------------------------------------------
//
// File: AppController.cpp
//
//----------------------------------------------------------------

#include <controller/AppController.h>
#include <iostream>

using namespace Ctrl;

//----------------------------------------------------------------

AppController::AppController()
    : view_(5)
// AppController::AppController(std::unique_ptr<IView> view)
//    : view_(std::move(view))
{
    // Empty
}

//----------------------------------------------------------------

// std::vector<CrapsTable>
std::vector<int>
AppController::loadAvailableTables()
{
    return {1, 2, 3};
    // Stubbed: load from file or build in memory
    // return {
    //     CrapsTable("Standard Table"),
    //     CrapsTable("No 7-Out Table"),
    //     CrapsTable("Fast-Paced Table")
    // };
}

//----------------------------------------------------------------

void
AppController::run()
{
#if 0    
    auto tables = loadAvailableTables();
    auto selectedTable = view_->promptUserToSelectTable(tables);

    auto allPlayers = playerManager_.loadPlayers();
    auto selectedPlayers = view_->promptUserToSelectPlayers(allPlayers);

    auto gameController = std::make_unique<GameController>(selectedTable, selectedPlayers);
    view_->setGameController(gameController.get());

    gameController->runGame();
#endif
    std::cout << "Todo: implement a read from stdin\n";
}

//----------------------------------------------------------------
