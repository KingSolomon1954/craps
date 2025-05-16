//----------------------------------------------------------------
//
// File: CrapsGame.cpp
//
//----------------------------------------------------------------

#include "CrapsGame.h"
#include <iostream>
#include <rang.hpp>
#include "gen/BuildInfo.h"
#include "gen/StringUtils.h"
#include "CommandLine.h"
#include "CrapsTable.h"
#include "EventManager.h"
#include "PlayerManager.h"

#if 0
#include "codec/EnumChannelDirection.h"
#include "Properties.h"
#endif

using namespace App;

const std::string CrapsGame::appNameScreen = "Royal Craps";
const std::string CrapsGame::appNameExec   = "RoyalCraps";

/*-----------------------------------------------------------*//**

Constructor

@param[in] argc
    Number of items in argv array.

@param[in] argv
    Array of command line args.
*/
CrapsGame::CrapsGame(int argc, char* argv[])
{
    Gen::BuildInfo bld(App::CrapsGame::appNameScreen);
    CommandLine cmdline(argc, argv, bld);
    std::cout << bld.fullInfo() << std::endl;
    
    // Init globals and manage their lifetime
    std::unique_ptr<EventManager>  pEventMgr(initEventManager());   (void) pEventMgr;
    std::unique_ptr<PlayerManager> pPlayerMgr(initPlayerManager()); (void) pPlayerMgr;
    std::unique_ptr<CrapsTable>    pTable(initCrapsTable());        (void) pTable;

    Player alice("Alice", 1000u);
    Player john("John", 1000u);

    Gen::ErrorPass ep;
    Gbl::pTable->addPlayer(alice.getUuid(), ep);
    Gbl::pTable->addPlayer(john.getUuid(), ep);
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

CrapsTable*
CrapsGame::initCrapsTable()
{
    auto t = new CrapsTable();
    Gbl::pTable = t;
    return t;
}

//----------------------------------------------------------------

