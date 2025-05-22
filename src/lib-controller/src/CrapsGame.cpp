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
#include <controller/CommandLine.h>
#include <controller/ConfigFiles.h>
#include <controller/Env.h>
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
*/
CrapsGame::CrapsGame(int argc, char* argv[])
{
//  std::unique_ptr<Gen::Logger>           pLogger(initLogger());           (void) pLogger;
    std::unique_ptr<Gen::BuildInfo>        pBuildInfo(initBuildInfo());     (void) pBuildInfo;
    std::unique_ptr<Gen::MultiLayerConfig> pCfg(initCfg(argc, argv));       (void) pCfg;
    std::unique_ptr<Ctrl::EventManager>    pEventMgr(initEventManager());   (void) pEventMgr;
    std::unique_ptr<Ctrl::PlayerManager>   pPlayerMgr(initPlayerManager()); (void) pPlayerMgr;

    // Setup the chosen view IAW cmdline option.
    std::shared_ptr<ViewIntfc> pView = std::make_shared<Cui::ConsoleView>();
    
    // Bring in GameController, and associate the view with it.
    GameController controller(pView);

    // Ok now to interact with user via the gui/cui view.
    controller.userSelectsTableAndPlayers();
    pView->run();
}

//----------------------------------------------------------------

Gen::MultiLayerConfig* 
CrapsGame::initCfg(int argc, char* argv[])
{
    // TODO fix up later with real config processing
    Gen::MultiLayerConfig* pCfg = new Gen::MultiLayerConfig();
    CommandLine::processCmdLine(argc, argv, pCfg);
    ConfigFiles::processFiles(pCfg);
    Env::processEnv(pCfg);

    std::cout << Gbl::pBuildInfo->shortInfo() << std::endl;

    Gbl::pCfg = pCfg;
    dumpConfig(pCfg);
    return pCfg;
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

PlayerManager*
CrapsGame::initPlayerManager()
{
    auto p = new PlayerManager();
    Gbl::pPlayerMgr = p;
    return p;
}

//----------------------------------------------------------------

Craps::CrapsTable*
CrapsGame::initCrapsTable()
{
    auto t = new Craps::CrapsTable();
    Gbl::pTable = t;
    return t;
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
