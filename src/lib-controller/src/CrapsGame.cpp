//----------------------------------------------------------------
//
// File: CrapsGame.cpp
//
//----------------------------------------------------------------

#include <controller/CrapsGame.h>
#include <iostream>
#include <rang.hpp>
#include <gen/BuildInfo.h>
#include <gen/Logger.h>
#include <gen/Debug.h>
#include <controller/ConfigManager.h>
#include <controller/EventLoop.h>
#include <controller/EventManager.h>
#include <controller/GameController.h>
#include <controller/Globals.h>
#include <controller/PlayerManager.h>
#include <controller/ViewIntfc.h>
#include <craps/CrapsTable.h>
#include <cui/ConsoleView.h>

using namespace Ctrl;

/*-----------------------------------------------------------*//**

Constructor

@param[in] argc
    Number of items in argv array.

@param[in] argv
    Array of command line args.

@internal

All these unique_ptr's on the stack manage the lifetime of globals.

*/
CrapsGame::CrapsGame(int argc, char* argv[])
{
    std::unique_ptr<Gen::BuildInfo>       pBuildInfo(initBuildInfo());         (void) pBuildInfo;
    std::unique_ptr<Ctrl::ConfigManager>  pCfg(initConfigManager(argc, argv)); (void) pCfg;
    enableFileLogging();                  // Only after config manager.
    std::unique_ptr<Ctrl::EventManager>   pEventMgr(initEventManager());       (void) pEventMgr;
    std::unique_ptr<Ctrl::TableManager>   pTablerMgr(initTableManager());      (void) pTablerMgr;
    std::unique_ptr<Ctrl::PlayerManager>  pPlayerMgr(initPlayerManager());     (void) pPlayerMgr;
    std::unique_ptr<Ctrl::ViewIntfc>      pView(initView());                   (void) pView;
    std::unique_ptr<Ctrl::GameController> pGameCtrl(initGameController());     (void) pGameCtrl;
    std::unique_ptr<Ctrl::EventLoop>      pEventLoop(initEventLoop());         (void) pEventLoop;
    
    pView->run();  // Doesn't return until exiting
    
    Gbl::pTable->prepareForShutdown();
}

//----------------------------------------------------------------

void
CrapsGame::enableFileLogging()
{
    assert(Gbl::pConfigMgr != nullptr);
    
    // Setup log file name.
    std::string d = Gbl::pConfigMgr->getString(ConfigManager::KeyDirsUsrLog).value();
    std::string f = "/" + Gbl::appNameExec + ".log";
    
    // After next statement, all logging will be seen only in file.
    Gen::Logger::instance().setOutputFile(d + f);

    // Set up logging filters IAW config.
    bool debug = Gbl::pConfigMgr->getBool(ConfigManager::KeyDebugLogging).value();
    if (!debug)
    {
        Gen::Logger::instance().setDebugLevel(false);
    }
    
    bool trace = Gbl::pConfigMgr->getBool(ConfigManager::KeyTraceLogging).value();
    if (trace)
    {
        Gen::Logger::instance().setTraceLevel(true);
    }

    // First logging entry in file.
    Gen::Logger::instance().logInfo("Starting " + Gbl::pBuildInfo->shortInfo());
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

Ctrl::ConfigManager*
CrapsGame::initConfigManager(int argc, char* argv[])
{
    auto* p = new Ctrl::ConfigManager(argc, argv);
    Gbl::pConfigMgr = p;
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
    auto p = new TableManager();  // And creates initial CrapsTable
    Gbl::pTableMgr = p;
    return p;
}

//----------------------------------------------------------------

PlayerManager*
CrapsGame::initPlayerManager()
{
    auto p = new PlayerManager();
    p->loadStartingPlayers();
    Gbl::pPlayerMgr = p;
    return p;
}

//----------------------------------------------------------------

ViewIntfc*
CrapsGame::initView()
{
    auto p = getView();
    Gbl::pView = p;
    return p;
}

//----------------------------------------------------------------

GameController*
CrapsGame::initGameController()
{
    auto p = new GameController();
    Gbl::pGameCtrl = p;
    return p;
}

//----------------------------------------------------------------

EventLoop*
CrapsGame::initEventLoop()
{
    auto p = new EventLoop();
    Gbl::pEventLoop = p;
    return p;
}

//----------------------------------------------------------------

ViewIntfc*
CrapsGame::getView()
{
    std::string v = Gbl::pConfigMgr->getString(ConfigManager::KeyViewType).value();
    if (v == "console") return new Cui::ConsoleView();
//  if (v == "cmdline") return new Cli::CmdLineView();
//  if (v == "graphical") return Gui::GuiView();

    std::string diag = "Invalid value for config parameter:\"" +
        std::string(ConfigManager::KeyViewType) +
        "\". At this time only console (--con), the default, is available. "
        "Future options for GUI and CmdLine are not implemented yet.";
        
    throw std::invalid_argument(diag);
    return nullptr;
}

//----------------------------------------------------------------
