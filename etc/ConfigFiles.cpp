#include <controller/ConfigFiles.h>
#include <gen/MultiLayerConfig.h>
#include <yaml-cpp/yaml.h>
#include <filesystem>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <controller/Globals.h>
#include <controller/ConfigManager.h>

using namespace Ctrl;

namespace fs = std::filesystem;

void ConfigFiles::processFiles(const Gen::MultiLayerConfig& multiConfig, Gen::ConfigLayer& cfg) {
    try {
        // Create user-specific directories
        std::string config_dir = multiConfig.getString(ConfigManager::KeyDirsConfig).value_or("/tmp");
        std::string data_dir = multiConfig.getString(ConfigManager::KeyDirsData).value_or("/tmp");
        std::string cache_dir = multiConfig.getString(ConfigManager::KeyDirsCache).value_or("/tmp");
        std::string log_dir = multiConfig.getString(ConfigManager::KeyDirsLog).value_or("/tmp");
        std::string save_dir = multiConfig.getString("dirs.saves").value_or("/tmp");

        fs::create_directories(config_dir);
        fs::create_directories(data_dir);
        fs::create_directories(cache_dir);
        fs::create_directories(log_dir);
        fs::create_directories(save_dir);

        // Check system directories (created by installer)
        std::string sys_config_dir = multiConfig.getString("dirs.config_system").value_or("/etc/" + Gbl::appNameExec);
        std::string shared_dir = multiConfig.getString(ConfigManager::KeyDirsShared).value_or("/usr/share/" + Gbl::appNameExec);
        std::string tables_dir = multiConfig.getString(ConfigManager::KeyDirsTables).value_or("/usr/share/" + Gbl::appNameExec + "/tables");
        std::string players_dir = multiConfig.getString(ConfigManager::KeyDirsPlayers).value_or("/usr/share/" + Gbl::appNameExec + "/players");
        std::string highscores_dir = multiConfig.getString("dirs.highscores").value_or("/var/games/" + Gbl::appNameExec);

        // Log warnings if system directories are missing
        if (!fs::exists(sys_config_dir)) {
            std::cerr << "Warning: System config directory " << sys_config_dir << " does not exist. Check installer setup.\n";
        }
        if (!fs::exists(shared_dir)) {
            std::cerr << "Warning: Shared directory " << shared_dir << " does not exist. Check installer setup.\n";
        }
        if (!fs::exists(tables_dir)) {
            std::cerr << "Warning: Tables directory " << tables_dir << " does not exist. Check installer setup.\n";
        }
        if (!fs::exists(players_dir)) {
            std::cerr << "Warning: Players directory " << players_dir << " does not exist. Check installer setup.\n";
        }
        if (!fs::exists(highscores_dir)) {
            std::cerr << "Warning: High scores directory " << highscores_dir << " does not exist. Using /tmp as fallback.\n";
            highscores_dir = "/tmp";
        }

        // Load system-wide config
        std::string sys_config = sys_config_dir + "/config.yaml";
        if (fs::exists(sys_config)) {
            loadYamlIntoConfig(sys_config, cfg);
        }

        // Load user config
        std::string user_config = config_dir + "/config.yaml";
        if (fs::exists(user_config)) {
            loadYamlIntoConfig(user_config, cfg);
        }

        // Copy directory paths to files layer for save/load functions
        cfg.set(ConfigManager::KeyDirsConfig, config_dir);
        cfg.set(ConfigManager::KeyDirsData, data_dir);
        cfg.set(ConfigManager::KeyDirsCache, cache_dir);
        cfg.set(ConfigManager::KeyDirsLog, log_dir);
        cfg.set("dirs.saves", save_dir);
        cfg.set("dirs.highscores", highscores_dir);
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to process config files: " << e.what() << '\n';
        throw;
    }
}

void ConfigFiles::loadYamlIntoConfig(const std::string& filename, Gen::ConfigLayer& cfg) {
    try {
        YAML::Node root = YAML::LoadFile(filename);

        std::function<void(const YAML::Node&, const std::string&)> recurse;
        recurse = [&](const YAML::Node& node, const std::string& prefix) {
            for (const auto& kv : node) {
                std::string key = prefix.empty() ? kv.first.as<std::string>() : prefix + "." + kv.first.as<std::string>();
                if (key.empty()) {
                    std::cerr << "Warning: Empty key in YAML file " << filename << ". Skipping.\n";
                    continue;
                }
                if (kv.second.IsMap()) {
                    recurse(kv.second, key);
                } else if (kv.second.IsScalar()) {
                    cfg.set(key, kv.second.as<std::string>());
                }
            }
        };

        recurse(root, "");
    }
    catch (const YAML::Exception& e) {
        std::cerr << "Failed to parse YAML file " << filename << ": " << e.what() << '\n';
        throw;
    }
}

void ConfigFiles::saveGameState(const Gen::ConfigLayer& cfg, const std::string& state, const std::string& filename) {
    try {
        std::string save_dir = cfg.get("dirs.saves").value_or("/tmp");
        fs::create_directories(save_dir); // Ensure save directory exists
        fs::path save_path = save_dir + "/" + filename;
        YAML::Node out;
        out["game_state"] = state;
        std::ofstream fout(save_path.string());
        if (!fout) {
            throw std::runtime_error("Failed to save game state to " + save_path.string());
        }
        fout << out;
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to save game state: " << e.what() << '\n';
        throw;
    }
}

std::string ConfigFiles::loadGameState(const Gen::ConfigLayer& cfg, const std::string& filename) {
    try {
        std::string save_dir = cfg.get("dirs.saves").value_or("/tmp");
        fs::path save_path = save_dir + "/" + filename;
        if (!fs::exists(save_path)) {
            return "";
        }
        YAML::Node root = YAML::LoadFile(save_path.string());
        return root["game_state"].as<std::string>("");
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to load game state from " << filename << ": " << e.what() << '\n';
        throw;
    }
}

void ConfigFiles::saveHighScore(const Gen::ConfigLayer& cfg, const std::string& player, int score) {
    try {
        std::string highscores_dir = cfg.get("dirs.highscores").value_or("/tmp");
        fs::create_directories(highscores_dir); // Create if fallback to /tmp
        fs::path high_score_path = highscores_dir + "/highscores.yaml";
        YAML::Node out;
        if (fs::exists(high_score_path)) {
            try {
                out = YAML::LoadFile(high_score_path.string());
            } catch (const YAML::Exception& e) {
                std::cerr << "Warning: Corrupted highscores.yaml, starting fresh: " << e.what() << '\n';
            }
        }
        YAML::Node entry;
        entry["player"] = player;
        entry["score"] = score;
        out["scores"].push_back(entry);
        std::ofstream fout(high_score_path.string());
        if (!fout) {
            throw std::runtime_error("Failed to save high score to " + high_score_path.string());
        }
        fout << out;
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to save high score: " << e.what() << '\n';
        throw;
    }
}