//----------------------------------------------------------------
//
// File: CommandLine.cpp
//
//----------------------------------------------------------------

#include <iostream>
#include <string>
#include <cxxopts.hpp>
#include <rang.hpp>
#include <controller/CommandLine.h>
#include <controller/Globals.h>
#include <gen/BuildInfo.h>
#include <gen/MultiLayerConfig.h>

using namespace Ctrl;

/*-----------------------------------------------------------*//**

Constructor

@param[in] argc
    Number of items in argv array.

@param[in] argv
    Array of command line args.
*/
void
CommandLine::processCmdLine(int argc, char* argv[],
                            Gen::MultiLayerConfig* pCfg)
{
    cxxopts::Options options("RoyalCraps", "A multiplayer dice game");

    options.add_options()
        ("f,foo", "Param foo",
         cxxopts::value<int>()->default_value("10"))
        
        ("b,bar", "Param bar",
         cxxopts::value<std::string>())
        
        ("d,debug", "Enable debugging",
         cxxopts::value<bool>()->default_value("false"))
        
        ("v,version", "Print version info then exit")
        
        ("h,help", "Print usage")
    ;

    auto result = options.parse(argc, argv);

    if (result.count("help"))
    {
        std::cout << options.help() << std::endl;
        exit(0);
    }
    if (result.count("version"))
    {
        std::cout << rang::style::bold
                  << Gbl::pBuildInfo->fullInfo() << rang::style::reset;
        exit(0);
    }
    bool debug = result["debug"].as<bool>();
    (void)debug;  // suppress compiler warning
    
    int foo = result["foo"].as<int>();    
    (void)foo;  // suppress compiler warning
    
    std::string bar;
    if (result.count("bar"))
        bar = result["bar"].as<std::string>();
}

//----------------------------------------------------------------
