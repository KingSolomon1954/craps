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
#include <gen/Logger.h>

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
        Gen::ConfigLayer sessionLayer;
        
        addLayer(LayerNameDefaults, defaultsLayer);
        addLayer(LayerNameFiles,    filesLayer);
        addLayer(LayerNameEnv,      envLayer);
        addLayer(LayerNameCmdLine,  cmdLineLayer);
        addLayer(LayerNameSession,  sessionLayer);   // Highest priority

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

AnimationSpeed
ConfigManager::animationSpeed() const
{
    const auto v = getString(KeyAnimationSpeed);
    if (!v) return AnimationSpeed::Normal;

    const auto speed = parseAnimationSpeed(*v);
    if (!speed)
    {
        LOG_WARN("Ignoring " + std::string(KeyAnimationSpeed) + ": \"" + *v +
                 "\" is not one of off, fast, normal or slow");
        return AnimationSpeed::Normal;
    }
    return *speed;
}

//----------------------------------------------------------------

void
ConfigManager::setAnimationSpeed(AnimationSpeed speed)
{
    set(LayerNameSession, KeyAnimationSpeed, toString(speed));
}

//----------------------------------------------------------------

std::optional<AnimationSpeed>
ConfigManager::animationSpeedToRemember() const
{
    // Chosen while playing?
    if (auto v = const_cast<ConfigManager*>(this)->getLayer(LayerNameSession)
                     .get(KeyAnimationSpeed))
    {
        return parseAnimationSpeed(*v);
    }
    // Otherwise keep what the last game remembered
    if (auto v = getString(KeyAnimationSpeedRemembered))
    {
        return parseAnimationSpeed(*v);
    }
    return std::nullopt;
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
