//----------------------------------------------------------------
//
// File: ConfigDefaults.cpp
//
//----------------------------------------------------------------

#include <controller/ConfigDefaults.h>
#include <cstdlib>
#include <filesystem>
#include <controller/ConfigManager.h>
#include <controller/Globals.h>

using namespace Ctrl;

/*-----------------------------------------------------------*//**

Process defaults

@param[in,out] pCfg
    The multi-layer configuration structure to populate.
*/
void
ConfigDefaults::processDefaults(Gen::ConfigLayer& cfg)
{
    loadDefaultDirs(cfg);
    loadGameDefaults(cfg);
}

//----------------------------------------------------------------

void
ConfigDefaults::loadDefaultDirs(Gen::ConfigLayer& cfg)
{
    const std::string home = getEnvOrDefault("HOME", "/tmp");
    namespace fs = std::filesystem;
    
    // 1. { dirs.sysshared } System-wide, shared configs, read-only)
    fs::path sysShared = "/usr/share/" + Gbl::appNameExec;
    cfg.set(ConfigManager::KeyDirsSysShared, sysShared.string());

    // 2. { dirs.systables } System-wide, shared table configs, read-only)
    fs::path tables = sysShared / "tables";
    cfg.set(ConfigManager::KeyDirsSysTables, tables.string());

    // 3. { dirs.sysplayers } System-wide, shared player configs, read-only)
    fs::path players = sysShared / "players";
    cfg.set(ConfigManager::KeyDirsSysPlayers, players.string());

    // 4. {dirs.sysconfig} System-wide config, admin read/write
    fs::path sysConfig = "/etc/" + Gbl::appNameExec;
    cfg.set(ConfigManager::KeyDirsSysConfig, sysConfig.string());

    // 5. { dirs.usrconfig } User config, user read/write
    std::string userConfigStr = getEnvOrDefault("XDG_CONFIG_HOME", home + "/.config");
    fs::path userConfig = userConfigStr + "/" + Gbl::appNameExec;
    cfg.set(ConfigManager::KeyDirsUsrConfig, userConfig.string());

    // 6. { dirs.usrdata } User data, user read/write
    std::string userDataStr = getEnvOrDefault("XDG_DATA_HOME", home + "/.local/share");
    fs::path userData = userDataStr + "/" + Gbl::appNameExec;
    cfg.set(ConfigManager::KeyDirsUsrData, userData.string());

    // 7. { dirs.usrcache } Cache
    std::string userCacheStr = getEnvOrDefault("XDG_CACHE_HOME", home + "/.cache");
    fs::path userCache = userCacheStr + "/" + Gbl::appNameExec;
    cfg.set(ConfigManager::KeyDirsUsrCache, userCache.string());

    // 8. { dirs.usrlog } Log
    std::string userStateStr = getEnvOrDefault("XDG_STATE_HOME", home + "/.local/state");
    fs::path userState = userStateStr + "/" + Gbl::appNameExec;
    cfg.set(ConfigManager::KeyDirsUsrLog, userState.string());
        
    // 9. { dirs.usrstate } State  same location as logs
    cfg.set(ConfigManager::KeyDirsUsrState, userState.string());

    // 10. { dirs.temp } Temp
    fs::path temp = "/tmp";
    cfg.set(ConfigManager::KeyDirsTemp, temp.string());
}
    
//----------------------------------------------------------------

std::string
ConfigDefaults::getEnvOrDefault(const std::string& envVar,
                                const std::string& fallback)
{
    const char* val = std::getenv(envVar.c_str());
    return val ? std::string(val) : fallback;
}

//----------------------------------------------------------------

void
ConfigDefaults::loadGameDefaults(Gen::ConfigLayer& cfg)
{
    cfg.set(ConfigManager::KeyViewType, "console");
    cfg.set(ConfigManager::KeyStartTable, "lasvegas");
    cfg.set(ConfigManager::KeySoundEnabled, "true");
    cfg.set(ConfigManager::KeySoundVolume, "50");
    // TODO more ...
}

//----------------------------------------------------------------

#if 0

/usr/share/craps/
//    ├── tables/
//    │   ├── vegas_rules.json
//    │   ├── atlantic_city.json
//    ├── players/
//    │   ├── demo_player1.json
//    │   ├── demo_player2.json
//    ├── themes/
//    │   └── default_theme.json
//    

#endif
