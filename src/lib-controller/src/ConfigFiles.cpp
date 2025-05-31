//----------------------------------------------------------------
//
// File: ConfigFiles.cpp
//
//----------------------------------------------------------------

#include <controller/ConfigFiles.h>
#include <filesystem>
#include <controller/ConfigManager.h>
#include <gen/MultiLayerConfig.h>

using namespace Ctrl;
namespace fs = std::filesystem;

/*-----------------------------------------------------------*//**

Process startup config files.

@param[in] multiConfig
    The multi-layer configuration structure.

@param[in,out] cfg
    The configuration layer we populate.
*/
void
ConfigFiles::processFiles(const Gen::MultiLayerConfig& multiConfig,
                          Gen::ConfigLayer& cfg)
{
    createUserDirs(multiConfig);
    checkSystemDirs(multiConfig);
    loadSystemConfig(multiConfig, cfg);
    loadUserConfig(multiConfig, cfg);
}

//----------------------------------------------------------------

void
ConfigFiles::createUserDirs(const Gen::MultiLayerConfig& multiConfig)
{
    std::string configDir = multiConfig.getString(ConfigManager::KeyDirsUsrConfig).value();
    std::string dataDir   = multiConfig.getString(ConfigManager::KeyDirsUsrData).value();
    std::string cacheDir  = multiConfig.getString(ConfigManager::KeyDirsUsrCache).value();
    std::string logDir    = multiConfig.getString(ConfigManager::KeyDirsUsrLog).value();
    std::string stateDir  = multiConfig.getString(ConfigManager::KeyDirsUsrState).value();

    fs::create_directories(configDir);
    fs::create_directories(dataDir);
    fs::create_directories(cacheDir);
    fs::create_directories(logDir);
    fs::create_directories(stateDir);
}

//----------------------------------------------------------------

void
ConfigFiles::checkSystemDirs(const Gen::MultiLayerConfig& multiConfig)
{
    // Check system directories created by installer
    std::string sysConfigDir  = multiConfig.getString(ConfigManager::KeyDirsSysConfig).value();
    std::string sysSharedDir  = multiConfig.getString(ConfigManager::KeyDirsSysShared).value();
    std::string sysTablesDir  = multiConfig.getString(ConfigManager::KeyDirsSysTables).value();
    std::string sysPlayersDir = multiConfig.getString(ConfigManager::KeyDirsSysPlayers).value();

    // Can't continue if basic assets are not found
    existsOrThrow(sysConfigDir);
    existsOrThrow(sysSharedDir);
    existsOrThrow(sysTablesDir);
    existsOrThrow(sysPlayersDir);
}

//----------------------------------------------------------------

void
ConfigFiles::loadSystemConfig(
    const Gen::MultiLayerConfig& multiConfig,
    Gen::ConfigLayer& cfg)
{
    const std::string& filename =
        multiConfig.getString(ConfigManager::KeyDirsSysConfig).value() + "/config.yaml";
    existsOrThrow(filename);
    loadNamedConfig(filename, cfg);
}

//----------------------------------------------------------------

void
ConfigFiles::loadUserConfig(
    const Gen::MultiLayerConfig& multiConfig,
    Gen::ConfigLayer& cfg)
{
    const std::string& filename =
        multiConfig.getString(ConfigManager::KeyDirsUsrConfig).value() + "/config.yaml";
    if (fs::exists(filename))
    {
        loadNamedConfig(filename, cfg);
    }
}

//----------------------------------------------------------------

void
ConfigFiles::loadNamedConfig(
    const std::string& filename,
    Gen::ConfigLayer& cfg)
{
#if 0
    try
    {
        YAML::Node root = YAML::LoadFile(filename);

        std::function<void(const YAML::Node&, const std::string&)> recurse;
        recurse = [&](const YAML::Node& node, const std::string& prefix)
        {
            for (const auto& kv : node)
            {
                std::string key = prefix.empty() ? kv.first.as<std::string>() :
                    prefix + "." + kv.first.as<std::string>();
                if (key.empty())
                {
                     throw std::runtime_error("Empty key in YAML file: " + filename);
                }
                if (kv.second.IsMap())
                {
                    recurse(kv.second, key);
                }
                else
                {
                    cig.set(key, kv.second.as<std::string>());
                }
            }
        };

        recurse(root, "");
    }
    catch (const YAML::Exception& e)
    {
        throw std::runtime_error("Failed to parse YAML file " +
                                 filename + ": " + e.what());
    }
#endif    
}

//----------------------------------------------------------------

void
ConfigFiles::existsOrThrow(const std::string& pathStr)
{
    if (!fs::exists(pathStr))
    {
        throw std::runtime_error("Directory not found: " + pathStr);
    }
}

//----------------------------------------------------------------

#if 0

yaml-cpp

sudo apt install libyaml-cpp-dev  # Debian/Ubuntu
# or build from source via CMake

#include <yaml-cpp/yaml.h>
#include <iostream>
#include "MultiLayerConfig.h"

void loadYamlIntoConfig(const std::string& filename, MultiLayerConfig& config, const std::string& layerName) {
    config.addLayer(layerName);
    YAML::Node root = YAML::LoadFile(filename);

    std::function<void(const YAML::Node&, const std::string&)> recurse;
    recurse = [&](const YAML::Node& node, const std::string& prefix) {
        for (const auto& kv : node) {
            std::string key = prefix.empty() ? kv.first.as<std::string>() : prefix + "." + kv.first.as<std::string>();
            if (kv.second.IsMap()) {
                recurse(kv.second, key);
            } else {
                config.set(layerName, key, kv.second.as<std::string>());
            }
        }
    };

    recurse(root, "");
}

// Writng to YAML file

YAML::Node out;
out["dirs"]["data"] = "/opt/myapp/data";
out["server"]["port"] = 8080;

std::ofstream fout("output.yaml");
fout << out;


#endif
