#include <controller/ConfigDefaults.h>
#include <cstdlib>
#include <filesystem>
#include <stdexcept>
#include <iostream>
#include <controller/ConfigManager.h>
#include <controller/Globals.h>

using namespace Ctrl;

namespace fs = std::filesystem;

void ConfigDefaults::processDefaults(Gen::ConfigLayer& cfg) {
    try {
        loadDefaultDirs(cfg);
        loadGameDefaults(cfg);
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to process defaults: " << e.what() << '\n';
        throw;
    }
}

void ConfigDefaults::loadDefaultDirs(Gen::ConfigLayer& cfg) {
    try {
        const std::string home = getEnvOrDefault("HOME", "/tmp");

        // 1. System-wide config
        fs::path sys_config = "/etc/" + Gbl::appNameExec;
        cfg.set("dirs.config_system", sys_config.string());

        // 2. Shared
        fs::path shared_dir = "/usr/share/" + Gbl::appNameExec;
        cfg.set(ConfigManager::KeyDirsShared, shared_dir.string());

        // 3. User config
        std::string config_dir = getEnvOrDefault("XDG_CONFIG_HOME", home + "/.config");
        fs::path user_config_dir = config_dir + "/" + Gbl::appNameExec;
        cfg.set(ConfigManager::KeyDirsConfig, user_config_dir.string());

        // 4. User data
        std::string data_dir = getEnvOrDefault("XDG_DATA_HOME", home + "/.local/share");
        fs::path user_data_dir = data_dir + "/" + Gbl::appNameExec;
        cfg.set(ConfigManager::KeyDirsData, user_data_dir.string());

        // 5. Cache
        std::string cache_dir = getEnvOrDefault("XDG_CACHE_HOME", home + "/.cache");
        fs::path user_cache_dir = cache_dir + "/" + Gbl::appNameExec;
        cfg.set(ConfigManager::KeyDirsCache, user_cache_dir.string());

        // 6. Temp
        fs::path temp_dir = "/tmp";
        cfg.set(ConfigManager::KeyDirsTemp, temp_dir.string());

        // 7. Log
        std::string state_dir = getEnvOrDefault("XDG_STATE_HOME", home + "/.local/state");
        fs::path log_dir = state_dir + "/" + Gbl::appNameExec;
        cfg.set(ConfigManager::KeyDirsLog, log_dir.string());

        // 8. Runtime
        std::string runtime_dir = getEnvOrDefault("XDG_RUNTIME_DIR", "/tmp");
        fs::path user_runtime_dir = runtime_dir + "/" + Gbl::appNameExec;
        cfg.set(ConfigManager::KeyDirsRuntime, user_runtime_dir.string());

        // 9. Tables
        fs::path tables_dir = shared_dir / "tables";
        cfg.set(ConfigManager::KeyDirsTables, tables_dir.string());

        // 10. Players
        fs::path players_dir = shared_dir / "players";
        cfg.set(ConfigManager::KeyDirsPlayers, players_dir.string());

        // 11. High scores
        fs::path high_score_dir = "/var/games/" + Gbl::appNameExec;
        cfg.set("dirs.highscores", high_score_dir.string());

        // 12. Saves
        fs::path save_dir = user_data_dir / "saves";
        cfg.set("dirs.saves", save_dir.string());
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to load default dirs: " << e.what() << '\n';
        throw;
    }
}

std::string ConfigDefaults::getEnvOrDefault(const std::string& envVar, const std::string& fallback) {
    const char* val = std::getenv(envVar.c_str());
    return val ? std::string(val) : fallback;
}

void ConfigDefaults::loadGameDefaults(Gen::ConfigLayer& cfg) {
    try {
        cfg.set(ConfigManager::KeyViewType, "console");
        cfg.set(ConfigManager::KeyStartTable, "vegas_rules");
        cfg.set("controls.mode", "Char");
        cfg.set("game.language", "en");
        cfg.set("game.dice_type", "standard");
        cfg.set("sound.enabled", "true");
        cfg.set("sound.volume", "50");
        cfg.set("display.dice_style", "ascii");
        cfg.set("display.colors", "true");
        cfg.set(ConfigManager::KeyTablePlayer1, "player1");
        cfg.set(ConfigManager::KeyTablePlayer2, "player2");
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to load game defaults: " << e.what() << '\n';
        throw;
    }
}