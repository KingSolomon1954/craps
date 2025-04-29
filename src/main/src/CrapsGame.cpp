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
    std::unique_ptr<PlayerManager> pPlayerMgr(initPlayerManager()); (void) pPlayerMgr;
    std::unique_ptr<CrapsTable> pTable(initCrapsTable()); (void) pTable;
    
#if 0    
    ChannelDirection cd = ChannelDirection::Forward;
    std::cout << "Channel direction: " << rang::fg::yellow
              << cd << rang::fg::reset << std::endl;

    std::string x("QuestionEverything");
    std::string y = Gen::StringUtils::toLower(x);
    Properties props;
    (void)props;
#endif    
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

