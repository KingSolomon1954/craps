//----------------------------------------------------------------
//
// File: ConfigEnv.cpp
//
//----------------------------------------------------------------

#include <controller/ConfigEnv.h>
#include <cstdlib>
#include <map>
#include <stdexcept>
#include <controller/ConfigManager.h>
#include <gen/MultiLayerConfig.h>

using namespace Ctrl;

/*-----------------------------------------------------------*//**

Process environment variables.

@param[in,out] multiConfig
    The multi-layer configuration structure.

@param[in,out] cfg
    The configuration layer we populate.
*/
void
ConfigEnv::processEnv(Gen::MultiLayerConfig& multiConfig)
{
    // Work with layer directly
    auto& cfg = multiConfig.getLayer(ConfigManager::LayerNameEnv);
    
    try
    {
        const std::map<std::string, std::string> envToKey =
        {
            {"ROYALCRAPS_VIEW_TYPE",     ConfigManager::KeyViewType},
            {"ROYALCRAPS_START_TABLE",   ConfigManager::KeyTableStart},
            {"ROYALCRAPS_SOUND_ENABLED", ConfigManager::KeySoundEnabled},
            {"ROYALCRAPS_SOUND_VOLUME",  ConfigManager::KeySoundVolume}
        };

        for (const auto& [env, key] : envToKey)
        {
            if (const char* val = std::getenv(env.c_str()))
            {
                cfg.set(key, val);
            }
        }
    }
    catch (const std::exception& e)
    {
        std::string diag = "Failed to process environment variables: ";
        throw std::runtime_error(diag  + e.what());
    }
}

//----------------------------------------------------------------
