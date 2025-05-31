#include <controller/ConfigManager.h>
#include <iostream>
#include <stdexcept>
#include <controller/ConfigCommandLine.h>
#include <controller/ConfigDefaults.h>
#include <controller/ConfigEnv.h>
#include <controller/ConfigFiles.h>
#include <controller/Globals.h>

using namespace Ctrl;

ConfigManager::ConfigManager(int argc, char* argv[]) {
    try {
        populateLayerDefaults();
        populateLayerFiles();
        populateLayerEnv();
        populateLayerCmdLine(argc, argv);
        dumpConfig();
    }
    catch (const std::exception& e) {
        std::cerr << "ConfigManager initialization failed: " << e.what() << '\n';
        throw;
    }
}

void ConfigManager::populateLayerDefaults() {
    try {
        Gen::ConfigLayer defaultsLayer;
        ConfigDefaults::processDefaults(defaultsLayer);
        addLayer(LayerNameDefaults, defaultsLayer);
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to populate defaults layer: " << e.what() << '\n';
        throw;
    }
}

void ConfigManager::populateLayerFiles() {
    try {
        Gen::ConfigLayer filesLayer;
        ConfigFiles::processFiles(*this, filesLayer);
        addLayer(LayerNameFiles, filesLayer);
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to populate files layer: " << e.what() << '\n';
        throw;
    }
}

void ConfigManager::populateLayerEnv() {
    try {
        Gen::ConfigLayer envLayer;
        ConfigEnv::processEnv(envLayer);
        addLayer(LayerNameEnv, envLayer);
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to populate env layer: " << e.what() << '\n';
        throw;
    }
}

void ConfigManager::populateLayerCmdLine(int argc, char* argv[]) {
    try {
        Gen::ConfigLayer cmdLineLayer;
        ConfigCommandLine::processCmdLine(argc, argv, cmdLineLayer);
        addLayer(LayerNameCmdLine, cmdLineLayer);
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to populate cmdline layer: " << e.what() << '\n';
        throw;
    }
}

void ConfigManager::dumpConfig() {
    try {
        auto finalConfig = exportResolved();
        std::cout << "\nResolved Config:\n";
        for (const auto& [key, value] : finalConfig) {
            std::cout << key << " = " << value << '\n';
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to dump config: " << e.what() << '\n';
        throw;
    }
}