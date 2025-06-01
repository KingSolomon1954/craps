//----------------------------------------------------------------
//
// File: ConfigManager.cpp
//
//----------------------------------------------------------------

#include <controller/ConfigManager.h>
#include <iostream>
#include <stdexcept>
#include <controller/ConfigCommandLine.h>
#include <controller/ConfigDefaults.h>
#include <controller/ConfigEnv.h>
#include <controller/ConfigFiles.h>
#include <controller/Globals.h>

using namespace Ctrl;

//----------------------------------------------------------------

ConfigManager::ConfigManager(int argc, char* argv[])
{
    try
    {
        // Order matters. Lookup later is in reverse order
        populateLayerDefaults();
        populateLayerFiles();
        populateLayerEnv();
        populateLayerCmdLine(argc, argv);
        dumpConfig();
    }
    catch(const std::exception& e)
    {
        const std::string diag = "Unable to process configuration. ";
        throw std::runtime_error(diag  + e.what());
    }
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
    ConfigFiles::processFiles(*this, filesLayer);
    addLayer(LayerNameFiles, filesLayer);
}
    
//----------------------------------------------------------------

void
ConfigManager::populateLayerEnv()
{
    Gen::ConfigLayer envLayer;
    ConfigEnv::processEnv(*this, envLayer);
    addLayer(LayerNameEnv, envLayer);
}
    
//----------------------------------------------------------------

void
ConfigManager::populateLayerCmdLine(int argc, char* argv[])
{
    Gen::ConfigLayer cmdLineLayer;
    ConfigCommandLine::processCmdLine(argc, argv, *this, cmdLineLayer);
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
