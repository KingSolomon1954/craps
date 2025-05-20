//----------------------------------------------------------------
//
// File: AppController.h
//
//----------------------------------------------------------------

#pragma once

#include <memory>
// #include "view/IView.h"
// #include "controller/GameController.h"
#include "craps/PlayerManager.h"
// #include "model/CrapsTable.h"

namespace Ctrl {

class AppController
{
public:
    AppController();
//  AppController(std::unique_ptr<IView> view);

    void run();

private:
    int view_;
//  std::unique_ptr<IView> view_;
    Craps::PlayerManager playerManager_;

    std::vector<int> loadAvailableTables();
//  std::vector<CrapsTable> loadAvailableTables();
};

/*-----------------------------------------------------------*//**

@class AppController

@brief A bet on the craps table.

Responsible for high-level orchestration logic: table/player selection,
game start.

*/

}  // namespace Ctrl

//----------------------------------------------------------------
