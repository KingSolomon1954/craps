//----------------------------------------------------------------
//
// File: CrapsGame.cpp
//
//----------------------------------------------------------------

#include <controller/CrapsGame.h>
#include <iostream>
#include <rang.hpp>
#include <gen/BuildInfo.h>
#include <controller/ConfigManager.h>
#include <controller/EventManager.h>
#include <controller/GameController.h>
#include <controller/Globals.h>
#include <controller/PlayerManager.h>
#include <craps/CrapsTable.h>
#include <cui/ConsoleView.h>

using namespace Ctrl;

/*-----------------------------------------------------------*//**

Constructor

@param[in] argc
    Number of items in argv array.

@param[in] argv
    Array of command line args.
*/
CrapsGame::CrapsGame(int argc, char* argv[])
{
//  std::unique_ptr<Gen::Logger>           pLogger(initLogger());               (void) pLogger;
    std::unique_ptr<Gen::BuildInfo>        pBuildInfo(initBuildInfo());         (void) pBuildInfo;
    std::unique_ptr<Ctrl::ConfigManager>   pCfg(initConfigManager(argc, argv)); (void) pCfg;
    std::unique_ptr<Ctrl::EventManager>    pEventMgr(initEventManager());       (void) pEventMgr;
    std::unique_ptr<Ctrl::TableManager>    pTablerMgr(initTableManager());      (void) pTablerMgr;
    std::unique_ptr<Ctrl::PlayerManager>   pPlayerMgr(initPlayerManager());     (void) pPlayerMgr;

    // Setup the chosen view IAW cmdline option.
//  std::shared_ptr<ViewIntfc> pView = std::make_shared<Cui::ConsoleView>();
    std::shared_ptr<ViewIntfc> pView = getView();

    // Bring in GameController, and associate the view with it.
    GameController controller(pView);

    // Ok now to interact with user via the gui/cui view.
    std::unique_ptr<Craps::CrapsTable> pTable(controller.userSelectsTable()); (void) pTable;
    controller.userSelectsPlayers();
    pView->run();
}

//----------------------------------------------------------------

Ctrl::ConfigManager*
CrapsGame::initConfigManager(int argc, char* argv[])
{
    auto* p = new Ctrl::ConfigManager(argc, argv);
    Gbl::pConfigMgr = p;
    return p;
}

//----------------------------------------------------------------

Gen::BuildInfo*
CrapsGame::initBuildInfo()
{
    auto p = new Gen::BuildInfo(Gbl::appNameScreen);
    Gbl::pBuildInfo = p;
    return p;
}

//----------------------------------------------------------------

EventManager*
CrapsGame::initEventManager()
{
    auto p = new EventManager();
    Gbl::pEventMgr = p;
    return p;
}

//----------------------------------------------------------------

TableManager*
CrapsGame::initTableManager()
{
    auto p = new TableManager();
    Gbl::pTableMgr = p;
    return p;
}

//----------------------------------------------------------------

PlayerManager*
CrapsGame::initPlayerManager()
{
    auto p = new PlayerManager();
    Gbl::pPlayerMgr = p;
    return p;
}

//----------------------------------------------------------------

std::shared_ptr<ViewIntfc>
CrapsGame::getView()
{
    std::string v = Gbl::pConfigMgr->getString("screen.viewType").value();
    if (v == "console") return std::make_shared<Cui::ConsoleView>();
//  if (v == "cmdline") return std::make_shared<Cli::CmdLineView>();
//  if (v == "graphical") return std::make_shared<Gui::GuiView>();

    std::string diag = "Invalid value for config parameter:\"" +
        std::string(ConfigManager::KeyViewType) +
        "\". Options for GUI and CmdLine view not implemented yet";
        
    throw std::invalid_argument(diag);
    return nullptr;
}

//----------------------------------------------------------------

#if 0    
    // Init globals and manage their lifetime
    std::unique_ptr<Ctrl::EventManager>  pEventMgr(initEventManager());   (void) pEventMgr;
    std::unique_ptr<Ctrl::PlayerManager> pPlayerMgr(initPlayerManager()); (void) pPlayerMgr;
    std::unique_ptr<Craps::CrapsTable>   pTable(initCrapsTable());        (void) pTable;

    Craps::Player alice("Alice", 1000u);
    Craps::Player john("John", 1000u);

    Gen::ErrorPass ep;
    Gbl::pTable->addPlayer(alice.getUuid(), ep);
    Gbl::pTable->addPlayer(john.getUuid(), ep);
#endif    
