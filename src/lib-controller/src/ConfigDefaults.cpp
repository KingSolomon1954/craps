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
#include <gen/Timepoint.h>

using namespace Ctrl;

/*-----------------------------------------------------------*//**

Process defaults

@param[in,out] pCfg
    The multi-layer configuration structure to populate.
*/
void
ConfigDefaults::processDefaults(Gen::MultiLayerConfig& multiConfig)
{
    // Work with layer directly
    auto& cfg = multiConfig.getLayer(ConfigManager::LayerNameDefaults);
    
    loadDefaultDirs(cfg);
    loadGameDefaults(cfg);
}

//----------------------------------------------------------------

void
ConfigDefaults::loadDefaultDirs(Gen::ConfigLayer& cfg)
{
    // /usr/share/royalcraps                   system-wide
    // /usr/share/royalcraps/config            system-wide base config
    // /usr/share/royalcraps/tables            system-wide table defs
    // /usr/share/royalcraps/players           system-wide player defs
    // /usr/share/royalcraps/audio             system-wide sound
    // /usr/share/royalcraps/images            system-wide images
    //
    // $HOME/.config/royalcraps                user config preferences
    // $HOME/.local/share/royalcraps           user assets base
    // $HOME/.local/share/royalcraps/tables    user table defs if any
    // $HOME/.local/share/royalcraps/players   user player defs if any
    // $HOME/.local/share/royalcraps/audio     user sound if any
    // $HOME/.local/share/royalcraps/images    user images if any
    // $HOME/.cache/royalcraps                 intermediate runtime files
    // $HOME/.local/state/royalcraps           logs/state
    //
    // /etc/royalcraps/                        admin settings if needed r/w 
    // /tmp                                    tmp
    
    const std::string home = getEnvOrDefault("HOME", "/tmp");
    namespace fs = std::filesystem;
    
    // { dirs.sysshared } System-wide, shared assets/configs, read-only)
    fs::path sysShared = "/usr/share/" + Gbl::appNameExec;
    cfg.set(ConfigManager::KeyDirsSysShared, sysShared.string());

    // { dirs.sysconfig } System-wide, shared configs, read-only)
    fs::path sysConfig = sysShared / "config";
    cfg.set(ConfigManager::KeyDirsSysConfig, sysConfig.string());

    // { dirs.systables } System-wide, shared table configs, read-only)
    fs::path sysTables = sysShared / "tables";
    cfg.set(ConfigManager::KeyDirsSysTables, sysTables.string());

    // { dirs.sysplayers } System-wide, shared player configs, read-only)
    fs::path sysPlayers = sysShared / "players";
    cfg.set(ConfigManager::KeyDirsSysPlayers, sysPlayers.string());

    // { dirs.sysaudio } System-wide, shared audio, read-only)
    fs::path sysAudio = sysShared / "audio";
    cfg.set(ConfigManager::KeyDirsSysAudio, sysAudio.string());

    // { dirs.sysaudio } System-wide, shared audio, read-only)
    fs::path sysImages = sysShared / "images";
    cfg.set(ConfigManager::KeyDirsSysImages, sysImages.string());
    
    // { dirs.usrdata } User data, equivalent to /usr/share/royalcraps read/write
    std::string usrDataStr = getEnvOrDefault("XDG_DATA_HOME", home + "/.local/share");
    fs::path usrData = usrDataStr + "/" + Gbl::appNameExec;
    cfg.set(ConfigManager::KeyDirsUsrData, usrData.string());
    
    // { dirs.usrconfig } User config, user read/write
    std::string usrConfigStr = getEnvOrDefault("XDG_CONFIG_HOME", home + "/.config");
    fs::path usrConfig = usrConfigStr + "/" + Gbl::appNameExec;
    cfg.set(ConfigManager::KeyDirsUsrConfig, usrConfig.string());

    // { dirs.usrtables } User table configs, if any read/write
    fs::path usrTables = usrData / "tables";
    cfg.set(ConfigManager::KeyDirsUsrTables, usrTables.string());

    // { dirs.usrplayers } User player configs, if any read/write
    fs::path usrPlayers = usrData / "players";
    cfg.set(ConfigManager::KeyDirsUsrPlayers, usrPlayers.string());

    // { dirs.usraudio } User audio, if any read/write
    fs::path usrAudio = usrData / "audio";
    cfg.set(ConfigManager::KeyDirsUsrAudio, usrAudio.string());

    // { dirs.usrimages } User images, if any read/write
    fs::path usrImages = usrData / "images";
    cfg.set(ConfigManager::KeyDirsUsrImages, usrImages.string());
    
    // { dirs.usrcache } Cache
    std::string userCacheStr = getEnvOrDefault("XDG_CACHE_HOME", home + "/.cache");
    fs::path userCache = userCacheStr + "/" + Gbl::appNameExec;
    cfg.set(ConfigManager::KeyDirsUsrCache, userCache.string());

    // { dirs.usrlog } Log
    std::string userStateStr = getEnvOrDefault("XDG_STATE_HOME", home + "/.local/state");
    fs::path userState = userStateStr + "/" + Gbl::appNameExec;
    cfg.set(ConfigManager::KeyDirsUsrLog, userState.string());
        
    // { dirs.usrstate } State  same location as logs
    cfg.set(ConfigManager::KeyDirsUsrState, userState.string());
    
    // { dirs.temp } Temp
    fs::path temp = "/tmp";
    cfg.set(ConfigManager::KeyDirsTemp, temp.string());

    // {dirs.admconfig} System-wide config, admin read/write
    fs::path admConfig = "/etc/" + Gbl::appNameExec;
    cfg.set(ConfigManager::KeyDirsAdmConfig, admConfig.string());
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
    cfg.set(ConfigManager::KeyViewType,     "console");
    cfg.set(ConfigManager::KeySoundEnabled, "true");
    cfg.set(ConfigManager::KeySoundVolume,  "50");
    cfg.set(ConfigManager::KeyDebugLogging,  "true");
    cfg.set(ConfigManager::KeyTraceLogging,  "false");

    Gen::Timepoint tp;
    cfg.set(ConfigManager::KeySessionStart,        tp.toString());
    cfg.set(ConfigManager::KeyTableStart,          "Table-1");
    cfg.set(ConfigManager::KeyTableStartLastPlyrs, "true");
    cfg.set(ConfigManager::KeyTableMaxSessions,    "50");
    cfg.set(ConfigManager::KeyTableMaxRecentRolls, "25");
    // TODO more ...
}

//----------------------------------------------------------------

#if 0

/usr/share/craps/
//    ├── tables/
//    │   ├── vegas_rules.yaml
//    │   ├── atlantic_city.yaml
//    ├── players/
//    │   ├── demo_player1.yaml
//    │   ├── demo_player2.yaml
//    ├── themes/
//    │   └── default_theme.yaml
//    

#endif
