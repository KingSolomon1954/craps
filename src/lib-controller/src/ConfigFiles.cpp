//----------------------------------------------------------------
//
// File: ConfigFiles.cpp
//
//----------------------------------------------------------------

#include <controller/ConfigFiles.h>
#include <gen/MultiLayerConfig.h>

using namespace Ctrl;

/*-----------------------------------------------------------*//**

Process startup config files.

@param[in,out] pCfg
    The multi-layer configuration structure to populate.
*/
void
ConfigFiles::processFiles(Gen::ConfigLayer& cfg)
{
    (void) cfg;
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
