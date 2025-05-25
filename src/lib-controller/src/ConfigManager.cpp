//----------------------------------------------------------------
//
// File: ConfigManager.cpp
//
//----------------------------------------------------------------

#include <controller/ConfigManager.h>
#include <iostream>
#include <controller/ConfigCommandLine.h>
#include <controller/ConfigDefaults.h>
#include <controller/ConfigEnv.h>
#include <controller/ConfigFiles.h>
#include <controller/Globals.h>

using namespace Ctrl;

//----------------------------------------------------------------

ConfigManager::ConfigManager(int argc, char* argv[])
{
    // Order matters. Lookup later is in reverse order
    populateLayerDefaults();
    populateLayerFiles();
    populateLayerEnv();
    populateLayerCmdLine(argc, argv);
    dumpConfig();
}

//----------------------------------------------------------------

void
ConfigManager::populateLayerDefaults()
{
    Gen::ConfigLayer defaultsLayer;
    ConfigDefaults::processDefaults(defaultsLayer);
    addLayer(LayerNameDefaults, defaultsLayer);
}
    
//----------------------------------------------------------------

void
ConfigManager::populateLayerFiles()
{
    Gen::ConfigLayer filesLayer;
    ConfigFiles::processFiles(filesLayer);
    addLayer(LayerNameFiles, filesLayer);
}
    
//----------------------------------------------------------------

void
ConfigManager::populateLayerEnv()
{
    Gen::ConfigLayer envLayer;
    ConfigEnv::processEnv(envLayer);
    addLayer(LayerNameEnv, envLayer);
}
    
//----------------------------------------------------------------

void
ConfigManager::populateLayerCmdLine(int argc, char* argv[])
{
    Gen::ConfigLayer cmdLineLayer;
    ConfigCommandLine::processCmdLine(argc, argv, cmdLineLayer);
    addLayer(LayerNameCmdLine, cmdLineLayer);
}
    
//----------------------------------------------------------------

void
ConfigManager::dumpConfig()
{
    auto finalConfig = exportResolved();
    std::cout << "\nResolved Config:\n";
    for (const auto& [key, value] : finalConfig)
    {
        std::cout << key << " = " << value << '\n';
    }
}
    
//----------------------------------------------------------------
