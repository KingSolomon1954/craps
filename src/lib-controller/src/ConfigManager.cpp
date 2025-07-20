//----------------------------------------------------------------
//
// File: ConfigManager.cpp
//
//----------------------------------------------------------------

#include <controller/ConfigManager.h>
#include <iostream>
#include <map>
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
        // Add layers first. Order matters. Lookup later is in 
        // reverse order of adding.
        Gen::ConfigLayer defaultsLayer;
        Gen::ConfigLayer filesLayer;
        Gen::ConfigLayer envLayer;
        Gen::ConfigLayer cmdLineLayer;
        
        addLayer(LayerNameDefaults, defaultsLayer);
        addLayer(LayerNameFiles,    filesLayer);
        addLayer(LayerNameEnv,      envLayer);
        addLayer(LayerNameCmdLine,  cmdLineLayer);

        // Be aware that addLayer() does a make_shared<> which creates a
        // copy, so can't use these four layers directly anymore.
        
        // OK to now populate the layers in different order 
        // since some layers depend on others having content.

        ConfigDefaults::processDefaults(*this);
        ConfigCommandLine::processCmdLine(argc, argv, *this);
        ConfigEnv::processEnv(*this);
        ConfigFiles::processFiles(*this); // last since file locations are overridable
        dumpConfig();
    }
    catch(const std::exception& e)
    {
        const std::string diag = "Unable to process configuration. ";
        throw std::runtime_error(diag  + e.what());
    }
}

//----------------------------------------------------------------

void ConfigManager::dumpConfig()
{
    auto finalConfig = exportResolved();

    // Copy to a std::map to get sorted keys
    std::map<std::string, std::string> sortedConfig(
        finalConfig.begin(), finalConfig.end());

    std::cout << "\nResolved Config:\n";
    for (const auto& [key, value] : sortedConfig)
    {
        std::cout << key << " = " << value << '\n';
    }
}

//----------------------------------------------------------------
