#include <controller/ConfigEnv.h>
#include <gen/MultiLayerConfig.h>
#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <controller/Globals.h>
#include <controller/ConfigManager.h>

using namespace Ctrl;

void ConfigEnv::processEnv(Gen::ConfigLayer& cfg) {
    try {
        const std::map<std::string, std::string> env_to_key = {
            {"ROYALCRAPS_VIEW_TYPE", ConfigManager::KeyViewType},
            {"ROYALCRAPS_START_TABLE", ConfigManager::KeyStartTable},
            {"ROYALCRAPS_CONTROLS_MODE", "controls.mode"},
            {"ROYALCRAPS_LANGUAGE", "game.language"},
            {"ROYALCRAPS_DICE_TYPE", "game.dice_type"},
            {"ROYALCRAPS_SOUND_ENABLED", "sound.enabled"},
            {"ROYALCRAPS_SOUND_VOLUME", "sound.volume"},
            {"ROYALCRAPS_DICE_STYLE", "display.dice_style"},
            {"ROYALCRAPS_COLORS", "display.colors"}
        };

        for (const auto& [env, key] : env_to_key) {
            if (const char* val = std::getenv(env.c_str())) {
                cfg.set(key, val);
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to process environment variables: " << e.what() << '\n';
        throw;
    }
}