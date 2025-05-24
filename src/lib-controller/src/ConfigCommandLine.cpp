//----------------------------------------------------------------
//
// File: ConfigCommandLine.cpp
//
//----------------------------------------------------------------

#include <controller/ConfigCommandLine.h>
#include <iostream>
#include <string>
#include <cxxopts.hpp>
#include <rang.hpp>
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
ConfigCommandLine::processCmdLine(int argc, char* argv[],
                                  Gen::ConfigLayer& cfg)
{
    cxxopts::Options options(Gbl::appNameExec, Gbl::appNameScreen + ": A multiplayer dice game");

    options.add_options()
        ("cli", "run as command line program",
         cxxopts::value<bool>()->default_value("true"))
        ("con", "run as console program")
        ("gui", "run as GUI program")
        
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

    if (result.count("con"))
    {
        cfg.set("screen.viewType", "console");
    }

    if (result.count("cli"))
    {
        cfg.set("screen.viewType", "cmdline");
    }

    if (result.count("gui"))
    {
        cfg.set("screen.viewType", "graphical");
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
