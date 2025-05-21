//----------------------------------------------------------------
//
// File: CrapsGame.cpp
//
//----------------------------------------------------------------

#include "controller/CrapsGame.h"
#include <iostream>
#include <rang.hpp>
#include "gen/BuildInfo.h"
#include "gen/StringUtils.h"
#include "controller/CommandLine.h"
#include "controller/EventManager.h"
#include "controller/Globals.h"
#include "controller/PlayerManager.h"
#include "craps/CrapsTable.h"

#if 0
#include "controller/EnumChannelDirection.h"
#include "Properties.h"
#endif

using namespace Ctrl;

// const std::string CrapsGame::appNameScreen = "Royal Craps";
// const std::string CrapsGame::appNameExec   = "RoyalCraps";

/*-----------------------------------------------------------*//**

Constructor

@param[in] argc
    Number of items in argv array.

@param[in] argv
    Array of command line args.
*/
CrapsGame::CrapsGame(int argc, char* argv[])
{
    std::unique_ptr<Gen::BuildInfo>  pBuildInfo(initBuildInfo()); (void) pBuildInfo;
    // Process config files, cmdline, env vars, etc
    initConfig(argc, argv);
    
    // TODO bring in AppController, GameController
    
    // Init globals and manage their lifetime
    std::unique_ptr<Ctrl::EventManager>  pEventMgr(initEventManager());   (void) pEventMgr;
    std::unique_ptr<Ctrl::PlayerManager> pPlayerMgr(initPlayerManager()); (void) pPlayerMgr;
    std::unique_ptr<Craps::CrapsTable>   pTable(initCrapsTable());        (void) pTable;

    Craps::Player alice("Alice", 1000u);
    Craps::Player john("John", 1000u);

    Gen::ErrorPass ep;
    Gbl::pTable->addPlayer(alice.getUuid(), ep);
    Gbl::pTable->addPlayer(john.getUuid(), ep);
}

//----------------------------------------------------------------

void
CrapsGame::initConfig(int argc, char* argv[])
{
    // TODO fix up later with real config processing
    CommandLine cmdline(argc, argv);
    std::cout << Gbl::pBuildInfo->shortInfo() << std::endl;
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

