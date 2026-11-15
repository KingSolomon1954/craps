//----------------------------------------------------------------
//
// File: ConfigFiles.cpp
//
//----------------------------------------------------------------

#include <controller/ConfigFiles.h>
#include <filesystem>
#include <functional>
#include <controller/ConfigManager.h>
#include <controller/ConfigSeed.h>
#include <controller/UserState.h>
#include <gen/MultiLayerConfig.h>

using namespace Ctrl;
namespace fs = std::filesystem;

/*-----------------------------------------------------------*//**

Process startup config files.

1. Throws if system directories are missing and needed
2. Creates local user directories
3. Seeds the user directories from the packaged files if they are not
   populated yet, see ConfigSeed. Notes a first run in the config.
4. Finds config files based on MultiLayerConfig
5. Loads config files into MultiLayerConfig in files layer, each
   overriding the previous: packaged sys-config.yaml, the user's
   config.yaml, then state.yaml (what the last run remembered).

@param[in] multiConfig
    The multi-layer configuration structure.
*/
void
ConfigFiles::processFiles(Gen::MultiLayerConfig& multiConfig)
{
    checkSystemDirs(multiConfig);
    createUserDirs(multiConfig);
    const bool firstRun = ConfigSeed::seedUserFiles(multiConfig);

    // Work with layer directly from here on.
    auto& cfg = multiConfig.getLayer(ConfigManager::LayerNameFiles);
    cfg.set(ConfigManager::KeyStartupFirstRun, firstRun ? "true" : "false");

    loadSystemConfig(multiConfig, cfg);  // Load the packaged config
    loadUserConfig(multiConfig, cfg);    // Overrides packaged config
    UserState::loadInto(multiConfig, cfg); // Overrides user config
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
    // Check system directories created by installer. Packaged audio
    // and images are optional (nothing reads them yet, and an empty
    // directory does not survive some packaging steps).
    std::string sysSharedDir  = multiConfig.getString(ConfigManager::KeyDirsSysShared).value();
    std::string sysConfigDir  = multiConfig.getString(ConfigManager::KeyDirsSysConfig).value();
    std::string sysTablesDir  = multiConfig.getString(ConfigManager::KeyDirsSysTables).value();
    std::string sysPlayersDir = multiConfig.getString(ConfigManager::KeyDirsSysPlayers).value();

    // Can't continue if basic assets are not found
    if (!fs::exists(sysSharedDir))
    {
        throw std::runtime_error("Packaged game files not found: " +
            sysSharedDir + ". Searched <executable dir>/../share/" +
            "royalcraps, /usr/local/share/royalcraps and "
            "/usr/share/royalcraps. Use --sys-config-dir to name the "
            "directory holding config, tables and players.");
    }
    existsOrThrow(sysConfigDir);
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
        multiConfig.getString(ConfigManager::KeyDirsSysConfig).value() + "/sys-config.yaml";
    existsOrThrow(filename);
    yamlToMultiConfig(filename, cfg);
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
        yamlToMultiConfig(filename, cfg);
    }
}

//----------------------------------------------------------------

void
ConfigFiles::yamlToMultiConfig(const std::string& filename,
                               Gen::ConfigLayer& cfg)
{
    try
    {
        YAML::Node root = YAML::LoadFile(filename);

        std::function<void(const YAML::Node&, const std::string&)> recurse;
        recurse = [&](const YAML::Node& node, const std::string& prefix)
        {
            if (node.IsMap())
            {
                for (const auto& kv : node)
                {
                    std::string key = prefix.empty()
                        ? kv.first.as<std::string>()
                        : prefix + "." + kv.first.as<std::string>();

                    if (key.empty())
                        throw std::runtime_error("Empty key in YAML file: " + filename);

                    recurse(kv.second, key);
                }
            }
            else if (node.IsSequence())
            {
                for (size_t i = 0; i < node.size(); ++i)
                {
                    std::string seqKey = prefix + "[" + std::to_string(i) + "]";
                    recurse(node[i], seqKey);
                }
            }
            else if (node.IsScalar())
            {
                cfg.set(prefix, node.as<std::string>());
            }
            else
            {
                throw std::runtime_error("Unsupported YAML node type in file: " + filename);
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
        throw std::runtime_error("Directory/file not found: " + pathStr);
    }
}

//----------------------------------------------------------------





#if 0

// TODO: incorporate, need to save back to user's config.yaml

#include <yaml-cpp/yaml.h>
#include <string>
#include <map>
#include <regex>
#include <sstream>
#include <iostream>

// Example: flattenKey = "table.startPlayers[2]", value = "paige.yaml"
void insertKeyValue(YAML::Node& root, const std::string& flatKey, const std::string& value) {
    std::regex arrayRegex(R"((\w+)\[(\d+)\])"); // matches name[index]
    std::stringstream ss(flatKey);
    std::string segment;
    YAML::Node* current = &root;

    while (std::getline(ss, segment, '.')) {
        std::smatch match;
        if (std::regex_match(segment, match, arrayRegex)) {
            std::string arrayName = match[1];
            int index = std::stoi(match[2]);

            (*current)[arrayName] = (*current)[arrayName] ? (*current)[arrayName] : YAML::Node(YAML::NodeType::Sequence);

            // Resize if needed
            if ((*current)[arrayName].size() <= (size_t)index) {
                (*current)[arrayName].resize(index + 1);
            }
            current = &((*current)[arrayName][index]);
        } else {
            // Regular map key
            (*current)[segment] = (*current)[segment] ? (*current)[segment] : YAML::Node(YAML::NodeType::Map);
            current = &((*current)[segment]);
        }
    }

    // Finally, assign value
    *current = value;
}

YAML::Node multiConfigToYaml(const std::map<std::string, std::string>& flatMap) {
    YAML::Node root;
    for (const auto& [key, value] : flatMap) {
        insertKeyValue(root, key, value);
    }
    return root;
}

int main() {
    std::map<std::string, std::string> flatConfig = {
        {"table.startTable", "las-vegas.yaml"},
        {"table.startWithLastPlayers", "true"},
        {"table.startPlayers[0]", "nathan.yaml"},
        {"table.startPlayers[1]", "arthur.yaml"},
        {"table.startPlayers[2]", "paige.yaml"},
        {"table.maxSessions", "44"},
        {"table.maxRecentRolls", "33"},
        {"rollHistory.format", "1"},
        {"rollHistory.scroll", "up"},
        {"log.debug", "true"},
        {"log.trace", "false"},
        {"player.maxSessions", "22"}
    };

    YAML::Node root = multiConfigToYaml(flatConfig);

    YAML::Emitter out;
    out << root;

    std::cout << out.c_str() << "\n";
}

#endif

#if 0
// Writng to YAML file

YAML::Node out;
out["dirs"]["data"] = "/opt/myapp/data";
out["server"]["port"] = 8080;

std::ofstream fout("output.yaml");
fout << out;


#endif

//----------------------------------------------------------------

