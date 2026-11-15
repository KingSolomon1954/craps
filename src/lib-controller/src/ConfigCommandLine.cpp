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
    std::string usrDataDir = multiConfig.getString(
        ConfigManager::KeyDirsUsrData).value();
    std::string usrStateDir = multiConfig.getString(
        ConfigManager::KeyDirsUsrState).value();
    
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

        ("usr-data-dir", "User Data Directory (tables, players, ...) "
         "instead of default",
         cxxopts::value<std::string>()->default_value(usrDataDir))

        ("usr-state-dir", "User State Directory (state.yaml) "
         "instead of default",
         cxxopts::value<std::string>()->default_value(usrStateDir))
        
        ("debug", "Enable debug logging",
         cxxopts::value<bool>()->default_value("true"))
        
        ("no-debug", "Disable debug logging",
         cxxopts::value<bool>()->default_value("false"))
        
        ("trace", "Enable trace logging",
         cxxopts::value<bool>()->default_value("false"))
        
        ("no-trace", "Disable trace logging",
         cxxopts::value<bool>()->default_value("true"))
        
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

    if (result.count("usr-data-dir"))
    {
        std::string usrDataStr = result["usr-data-dir"].as<std::string>();
        setUsrDataDir(usrDataStr, cfg);
    }

    if (result.count("usr-state-dir"))
    {
        std::string usrStateStr = result["usr-state-dir"].as<std::string>();
        cfg.set(ConfigManager::KeyDirsUsrState, usrStateStr);
        cfg.set(ConfigManager::KeyDirsUsrLog,   usrStateStr);  // Log lives with state
    }

    // Explicitly set these if given on command line even if default.
    // Allows overriding values if coming from env or file.
    //
    if (result.count("debug"))
    {
        cfg.set(ConfigManager::KeyDebugLogging, "true"); // Enable debug logging
    }
    if (result.count("no-debug"))
    {
        cfg.set(ConfigManager::KeyDebugLogging, "false"); // Disable debug logging
    }
    if (result.count("trace"))
    {
        cfg.set(ConfigManager::KeyTraceLogging, "true"); // Enable trace logging
    }
    if (result.count("no-trace"))
    {
        cfg.set(ConfigManager::KeyTraceLogging, "false"); // Disable trace logging
    }
    
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
//
// The user data directory holds the writable tables, players, audio and
// images directories beneath it. Moving it moves them all.
//
void
ConfigCommandLine::setUsrDataDir(const std::string& usrDataStr,
                                 Gen::ConfigLayer& cfg)
{
    namespace fs = std::filesystem;
    fs::path usrData = usrDataStr;

    cfg.set(ConfigManager::KeyDirsUsrData,    usrData.string());
    cfg.set(ConfigManager::KeyDirsUsrTables,  (usrData / "tables").string());
    cfg.set(ConfigManager::KeyDirsUsrPlayers, (usrData / "players").string());
    cfg.set(ConfigManager::KeyDirsUsrAudio,   (usrData / "audio").string());
    cfg.set(ConfigManager::KeyDirsUsrImages,  (usrData / "images").string());
}

//----------------------------------------------------------------
