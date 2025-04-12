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
#include "codec/EnumChannelDirection.h"
#include "CommandLine.h"
#include "Properties.h"

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
    
    ChannelDirection cd = ChannelDirection::Forward;
    std::cout << "Channel direction: " << rang::fg::yellow
              << cd << rang::fg::reset << std::endl;

    std::string x("QuestionEverything");
    std::string y = Gen::StringUtils::toLower(x);
    Properties props;
    (void)props;
}

//----------------------------------------------------------------
