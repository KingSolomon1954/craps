//----------------------------------------------------------------
//
// File: CrapsGame.cpp
//
//----------------------------------------------------------------

#include <controller/CrapsGame.h>
#include <iostream>
#include <rang.hpp>
#include <gen/BuildInfo.h>
#include <gen/MultiLayerConfig.h>
#include <controller/ConfigCommandLine.h>
#include <controller/ConfigDefaults.h>
#include <controller/ConfigEnv.h>
#include <controller/ConfigFiles.h>
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
//  std::unique_ptr<Gen::Logger>           pLogger(initLogger());           (void) pLogger;
    std::unique_ptr<Gen::BuildInfo>        pBuildInfo(initBuildInfo());     (void) pBuildInfo;
    std::unique_ptr<Gen::MultiLayerConfig> pCfg(initCfg(argc, argv));       (void) pCfg;
    std::unique_ptr<Ctrl::EventManager>    pEventMgr(initEventManager());   (void) pEventMgr;
    std::unique_ptr<Ctrl::TableManager>    pTablerMgr(initTableManager());  (void) pTablerMgr;
    std::unique_ptr<Ctrl::PlayerManager>   pPlayerMgr(initPlayerManager()); (void) pPlayerMgr;

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

Gen::MultiLayerConfig* 
CrapsGame::initCfg(int argc, char* argv[])
{
    Gen::MultiLayerConfig* pCfg = initMultiLayerConfig(argc, argv);
    std::cout << Gbl::pBuildInfo->shortInfo() << std::endl;
    Gbl::pCfg = pCfg;
    return pCfg;
}

//----------------------------------------------------------------

Gen::MultiLayerConfig*
CrapsGame::initMultiLayerConfig(int argc, char* argv[])
{
    auto* pCfg = new Gen::MultiLayerConfig();
    // Order matters. Lookup later is in reverse order
    populateLayerDefaults(pCfg);
    populateLayerFiles(pCfg);
    populateLayerEnv(pCfg);
    populateLayerCmdLine(argc, argv, pCfg);
    dumpConfig(pCfg);
    return pCfg;
}

//----------------------------------------------------------------

void
CrapsGame::populateLayerDefaults(Gen::MultiLayerConfig* pCfg)
{
    Gen::ConfigLayer defaultsLayer;
    ConfigDefaults::processDefaults(defaultsLayer);
    pCfg->addLayer(Gen::MultiLayerConfig::LayerDefaults, defaultsLayer);
}
    
//----------------------------------------------------------------

void
CrapsGame::populateLayerFiles(Gen::MultiLayerConfig* pCfg)
{
    Gen::ConfigLayer filesLayer;
    ConfigFiles::processFiles(filesLayer);
    pCfg->addLayer(Gen::MultiLayerConfig::LayerFiles, filesLayer);
}
    
//----------------------------------------------------------------

void
CrapsGame::populateLayerEnv(Gen::MultiLayerConfig* pCfg)
{
    Gen::ConfigLayer envLayer;
    ConfigEnv::processEnv(envLayer);
    pCfg->addLayer(Gen::MultiLayerConfig::LayerEnv, envLayer);
}
    
//----------------------------------------------------------------

void
CrapsGame::populateLayerCmdLine(int argc, char* argv[],
                                Gen::MultiLayerConfig* pCfg)
{
    Gen::ConfigLayer cmdLineLayer;
    ConfigCommandLine::processCmdLine(argc, argv, cmdLineLayer);
    pCfg->addLayer(Gen::MultiLayerConfig::LayerCmdLine, cmdLineLayer);
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

void
CrapsGame::dumpConfig(Gen::MultiLayerConfig* pCfg)
{
    auto finalConfig = pCfg->exportResolved();
    std::cout << "\nResolved Config:\n";
    for (const auto& [key, value] : finalConfig)
    {
        std::cout << key << " = " << value << '\n';
    }
}
    
//----------------------------------------------------------------

std::shared_ptr<ViewIntfc>
CrapsGame::getView()
{
    std::string v = Gbl::pCfg->getString("viewType").value_or("console");
    if (v == "console") return std::make_shared<Cui::ConsoleView>();
//  if (v == "cmdline") return std::make_shared<Cli::CmdLineView>();
//  if (v == "graphical") return std::make_shared<Gui::GuiView>();

    throw std::invalid_argument("Invalid value for config parameter: viewType"
        "GUI and CmdLine view not implemented yet");
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
