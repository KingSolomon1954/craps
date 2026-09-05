//----------------------------------------------------------------
//
// File: GameMain.cpp
//
//----------------------------------------------------------------

#include <controller/GameMain.h>
#include <cassert>
#include <iostream>
#include <rang.hpp>
#include <gen/BuildInfo.h>
#include <gen/Logger.h>
#include <gen/Debug.h>
#include <controller/ConfigManager.h>
#include <controller/GameController.h>
#include <controller/Globals.h>
#include <controller/PlayerManager.h>
#include <controller/TableManager.h>
#include <controller/ViewInterface.h>
#include <controller/UndoManager.h>
#include <craps/CrapsTable.h>
#include <craps/EventManager.h>
#include <cui/CuiMain.h>

using namespace Ctrl;

GameMain* GameMain::instancePtr_ = nullptr;

/*-----------------------------------------------------------*//**

Constructor

@param[in] argc
    Number of items in argv array.

@param[in] argv
    Array of command line args.

@internal

All these unique_ptr's on the stack manage the lifetime of globals.

*/
GameMain::GameMain(int argc, char* argv[])
{
    instancePtr_ = this;
    std::unique_ptr<Gen::BuildInfo>       pBuildInfo(initBuildInfo());         (void) pBuildInfo;
    std::unique_ptr<Ctrl::ConfigManager>  pCfg(initConfigManager(argc, argv)); (void) pCfg;
    setupLogging();                       // After config
    std::unique_ptr<Craps::EventManager>  pEventMgr(initEventManager());       (void) pEventMgr;
    std::unique_ptr<Ctrl::TableManager>   pTablerMgr(initTableManager());      (void) pTablerMgr;
    std::unique_ptr<Ctrl::PlayerManager>  pPlayerMgr(initPlayerManager());     (void) pPlayerMgr;
    std::unique_ptr<Ctrl::UndoManager>    pUndoMgr(initUndoManager());         (void) pUndoMgr;
    disableConsoleLogging();              // No more writing to screen
    std::unique_ptr<Ctrl::ViewInterface>  pView(initView());                   (void) pView;
    std::unique_ptr<Ctrl::GameController> pGameCtrl(initGameController());     (void) pGameCtrl;

    signalHandler_.waitForTerminate();  // Blocks until signal

    shutdownView();
    Gbl::pGameCtrl->prepareForShutdown();
    Gbl::pTable->prepareForShutdown();
}

//----------------------------------------------------------------

GameMain*
GameMain::instance()
{
    return instancePtr_;
}

//----------------------------------------------------------------

void
GameMain::terminateApp()
{
    signalHandler_.terminate();
}

//----------------------------------------------------------------

void
GameMain::setupLogging()
{
    assert(Gbl::pConfigMgr != nullptr);
    
    // Setup log file name.
    std::string d = Gbl::pConfigMgr->getString(ConfigManager::KeyDirsUsrLog).value();
    std::string f = "/" + Gbl::appNameExec + ".log";
    
    Gen::Logger::instance().setOutputFile(d + f);

    bool debug = Gbl::pConfigMgr->getBool(ConfigManager::KeyDebugLogging).value();
    Gen::Logger::instance().setDebugLevel(debug);
    
    bool trace = Gbl::pConfigMgr->getBool(ConfigManager::KeyTraceLogging).value();
    Gen::Logger::instance().setTraceLevel(trace);

    // First logging entry
    Gen::Logger::instance().logInfo("--------------------------------------");
    Gen::Logger::instance().logInfo("Starting " + Gbl::pBuildInfo->shortInfo());
    Gen::Logger::instance().logInfo("--------------------------------------");
}

//----------------------------------------------------------------

void
GameMain::disableConsoleLogging()
{
    Gen::Logger::instance().disableConsoleLogging();
}

//----------------------------------------------------------------

Gen::BuildInfo*
GameMain::initBuildInfo()
{
    auto p = new Gen::BuildInfo(Gbl::appNameScreen);
    Gbl::pBuildInfo = p;
    return p;
}

//----------------------------------------------------------------

Ctrl::ConfigManager*
GameMain::initConfigManager(int argc, char* argv[])
{
    auto* p = new Ctrl::ConfigManager(argc, argv);
    Gbl::pConfigMgr = p;
    return p;
}

//----------------------------------------------------------------

Craps::EventManager*
GameMain::initEventManager()
{
    auto p = new Craps::EventManager();
    Gbl::pEventMgr = p;
    return p;
}

//----------------------------------------------------------------

TableManager*
GameMain::initTableManager()
{
    auto p = new TableManager();  // And creates initial CrapsTable
    Gbl::pTableMgr = p;
    return p;
}

//----------------------------------------------------------------

PlayerManager*
GameMain::initPlayerManager()
{
    auto p = new PlayerManager();
    Gbl::pPlayerMgr = p;
    return p;
}

//----------------------------------------------------------------

UndoManager*
GameMain::initUndoManager()
{
    auto p = new UndoManager();
    Gbl::pUndoMgr = p;
    return p;
}

//----------------------------------------------------------------

ViewInterface*
GameMain::initView()
{
    auto p = getView();
    Gbl::pView = p;
    return p;
}

//----------------------------------------------------------------

GameController*
GameMain::initGameController()
{
    auto p = new GameController();
    Gbl::pGameCtrl = p;
    return p;
}

//----------------------------------------------------------------

ViewInterface*
GameMain::getView()
{
    std::string v = Gbl::pConfigMgr->getString(ConfigManager::KeyViewType).value();
    if (v == "console")
    {
        auto& cui = Cui::CuiMain::instance();
        return &cui.getView();
    }
    if (v == "graphical")
    {
        // TODO
        // TODO auto& gui = Gui::GuiMain::instance();
        // TODO return &gui.getView();
        return nullptr;
    }

    std::string diag = "Invalid value for config parameter:\"" +
        std::string(ConfigManager::KeyViewType) +
        "\". At this time only console (--con), the default, is available. "
        "Future options for GUI and CmdLine are not implemented yet.";
        
    throw std::invalid_argument(diag);
    return nullptr;
}

//----------------------------------------------------------------

void
GameMain::shutdownView()
{
    std::string v = Gbl::pConfigMgr->getString(ConfigManager::KeyViewType).value();
    if (v == "console")
    {
        Cui::CuiMain::instance().prepareForShutdown();
        return;
    }
    
    if (v == "graphical")
    {
        // TODO Gui::GuiMain::instance().prepareForShutdown();
        return;
    }
}

//----------------------------------------------------------------
