//----------------------------------------------------------------
//
// File: ConfigCommandLine.cpp
//
//----------------------------------------------------------------

#include <controller/ConfigCommandLine.h>
#include <filesystem>
#include <iostream>
#include <string>
#include <cxxopts.hpp>
#include <rang.hpp>
#include <controller/Globals.h>
#include <controller/ConfigManager.h>
#include <gen/BuildInfo.h>
#include <gen/MultiLayerConfig.h>

using namespace Ctrl;

/*-----------------------------------------------------------*//**

Process the command line into configuration structure

@param[in] argc
    Number of items in argv array.

@param[in] argv
    Array of command line args.

@param[in] multiConfig
    The multi-layer configuration structure.

@param[in,out] cfg
    The configuration layer we populate.
*/
void
ConfigCommandLine::processCmdLine(int argc, char* argv[],
                                  Gen::MultiLayerConfig& multiConfig)
{
    // Work with layer directly
    auto& cfg = multiConfig.getLayer(ConfigManager::LayerNameCmdLine);
    
    setAppPath(argv, cfg);
    cxxopts::Options options(Gbl::appNameExec, Gbl::appNameScreen + ": A multiplayer dice game");

    std::string sysSharedDir = multiConfig.getString(
        ConfigManager::KeyDirsSysShared).value();
    std::string usrConfigDir = multiConfig.getString(
        ConfigManager::KeyDirsUsrConfig).value();
    
    options.add_options()
        ("cli", "run as command line program",
         cxxopts::value<bool>()->default_value("true"))
        ("con", "run as console program")
        ("gui", "run as GUI program")
        
        ("f,foo", "Param foo",
         cxxopts::value<int>()->default_value("10"))
        
        ("b,bar", "Param bar",
         cxxopts::value<std::string>())

        ("sys-config-dir", "System Config Directory instead of default",
         cxxopts::value<std::string>()->default_value(sysSharedDir))
        
        ("usr-config-dir", "User Config Directory instead of default",
         cxxopts::value<std::string>()->default_value(usrConfigDir))
        
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
        cfg.set(ConfigManager::KeyViewType, "console");
    }

    if (result.count("cli"))
    {
        cfg.set(ConfigManager::KeyViewType, "cmdline");
    }

    if (result.count("gui"))
    {
        cfg.set(ConfigManager::KeyViewType, "graphical");
    }

    if (result.count("sys-config-dir"))
    {
        std::string sysSharedStr = result["sys-config-dir"].as<std::string>();
        setSysConfigDir(sysSharedStr, cfg);
    }
        
    if (result.count("usr-config-dir"))
    {
        std::string usrConfigStr = result["usr-config-dir"].as<std::string>();
        cfg.set(ConfigManager::KeyDirsUsrConfig, usrConfigStr);
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

void
ConfigCommandLine::setAppPath(char* argv[], Gen::ConfigLayer& cfg)
{
    namespace fs = std::filesystem;
    fs::path relPath = argv[0];
    fs::path absPath = fs::absolute(relPath);
    cfg.set(ConfigManager::KeyAppPath, absPath);
}
    
//----------------------------------------------------------------

void
ConfigCommandLine::setSysConfigDir(const std::string& sysSharedStr,
                                   Gen::ConfigLayer& cfg)
{
    namespace fs = std::filesystem;
    fs::path sysShared  = sysSharedStr;
    fs::path sysConfig  = sysShared / "config";
    fs::path sysTables  = sysShared / "tables";
    fs::path sysPlayers = sysShared / "players";
    fs::path sysAudio   = sysShared / "audio";
    fs::path sysImages  = sysShared / "images";

    cfg.set(ConfigManager::KeyDirsSysShared,  sysShared.string());
    cfg.set(ConfigManager::KeyDirsSysConfig,  sysConfig.string());
    cfg.set(ConfigManager::KeyDirsSysTables,  sysTables.string());
    cfg.set(ConfigManager::KeyDirsSysPlayers, sysPlayers.string());
    cfg.set(ConfigManager::KeyDirsSysAudio,   sysAudio.string());
    cfg.set(ConfigManager::KeyDirsSysImages,  sysImages.string());
}

//----------------------------------------------------------------
