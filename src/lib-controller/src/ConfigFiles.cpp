//----------------------------------------------------------------
//
// File: ConfigFiles.cpp
//
//----------------------------------------------------------------

#include <controller/ConfigFiles.h>
#include <filesystem>
#include <functional>
#include <controller/ConfigManager.h>
#include <gen/MultiLayerConfig.h>
#include <yaml-cpp/yaml.h>

using namespace Ctrl;
namespace fs = std::filesystem;

/*-----------------------------------------------------------*//**

Process startup config files.

1. Creates local user directories
2. Throws if system directories are missing and needed
3. Finds config files based on MultiLayerConfig
4. Loads config files into MultiLayerConfig in files layer

@param[in] multiConfig
    The multi-layer configuration structure.
*/
void
ConfigFiles::processFiles(Gen::MultiLayerConfig& multiConfig)
{
    createUserDirs(multiConfig);
    checkSystemDirs(multiConfig);

    // Work with layer directly
    auto& cfg = multiConfig.getLayer(ConfigManager::LayerNameFiles);
    
    loadSystemConfig(multiConfig, cfg);
    loadUserConfig(multiConfig, cfg);
}

//----------------------------------------------------------------

void
ConfigFiles::createUserDirs(const Gen::MultiLayerConfig& multiConfig)
{
    std::string usrDataDir    = multiConfig.getString(ConfigManager::KeyDirsUsrData).value();
    std::string usrConfigDir  = multiConfig.getString(ConfigManager::KeyDirsUsrConfig).value();
    std::string usrTablesDir  = multiConfig.getString(ConfigManager::KeyDirsUsrTables).value();
    std::string usrPlayersDir = multiConfig.getString(ConfigManager::KeyDirsUsrPlayers).value();
    std::string usrAudioDir   = multiConfig.getString(ConfigManager::KeyDirsUsrAudio).value();
    std::string usrImagesDir  = multiConfig.getString(ConfigManager::KeyDirsUsrImages).value();
    std::string usrCacheDir   = multiConfig.getString(ConfigManager::KeyDirsUsrCache).value();
    std::string usrLogDir     = multiConfig.getString(ConfigManager::KeyDirsUsrLog).value();
    std::string usrStateDir   = multiConfig.getString(ConfigManager::KeyDirsUsrState).value();

    fs::create_directories(usrDataDir);
    fs::create_directories(usrConfigDir);
    fs::create_directories(usrTablesDir);
    fs::create_directories(usrPlayersDir);
    fs::create_directories(usrAudioDir);
    fs::create_directories(usrImagesDir);
    fs::create_directories(usrCacheDir);
    fs::create_directories(usrLogDir);
    fs::create_directories(usrStateDir);
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
                    cfg.set(key, kv.second.as<std::string>());
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
