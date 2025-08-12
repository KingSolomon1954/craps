//----------------------------------------------------------------
//
// File: ConfigManager.h
//
//----------------------------------------------------------------

#pragma once

#include <gen/MultiLayerConfig.h>
#include <string>

namespace Gen {
    class ConfigLayer;  // fwd
}
    
namespace Ctrl {

class ConfigManager : public Gen::MultiLayerConfig
{
public:
    /// @name Lifecycle
    /// @{
    ConfigManager(int argc, char* argv[]);
    /// @}

    /// @name Modifiers
    /// @{
    /// @}

    /// @name Observers
    /// @{
    void dumpConfig();
    /// @}

public:
    static constexpr auto LayerNameDefaults = "defaults";
    static constexpr auto LayerNameFiles    = "files";
    static constexpr auto LayerNameEnv      = "env";
    static constexpr auto LayerNameCmdLine  = "cmdline";

    // Keys to retrieve/deposit config values
    static constexpr auto KeyAppPath        = "app.execAbsPath";

    // Directory/Folder keys
    static constexpr auto KeyDirsSysShared  = "dirs.sysshared";
    static constexpr auto KeyDirsSysConfig  = "dirs.sysconfig";
    static constexpr auto KeyDirsSysTables  = "dirs.systables";
    static constexpr auto KeyDirsSysPlayers = "dirs.sysplayers";
    static constexpr auto KeyDirsSysAudio   = "dirs.sysaudio";
    static constexpr auto KeyDirsSysImages  = "dirs.sysimages";
    static constexpr auto KeyDirsUsrData    = "dirs.usrdata";
    static constexpr auto KeyDirsUsrConfig  = "dirs.usrconfig";
    static constexpr auto KeyDirsUsrTables  = "dirs.usrtables";
    static constexpr auto KeyDirsUsrPlayers = "dirs.usrplayers";
    static constexpr auto KeyDirsUsrAudio   = "dirs.usraudio";
    static constexpr auto KeyDirsUsrImages  = "dirs.usrimages";
    static constexpr auto KeyDirsUsrCache   = "dirs.usrcache";
    static constexpr auto KeyDirsUsrLog     = "dirs.usrlog";
    static constexpr auto KeyDirsUsrState   = "dirs.usrstate";
    static constexpr auto KeyDirsTemp       = "dirs.temp";
    static constexpr auto KeyDirsAdmConfig  = "dirs.admconfig";
    
    // General Config    
    static constexpr auto KeySessionStartTime = "session.startTime";
    static constexpr auto KeyViewType         = "screen.viewType";
    static constexpr auto KeySoundEnabled     = "sound.enabled";
    static constexpr auto KeySoundVolume      = "sound.volume";
    static constexpr auto KeyDebugLogging     = "log.debug";
    static constexpr auto KeyTraceLogging     = "log.trace";
    
    // Table Setup
    static constexpr auto KeyTableStart          = "table.startTable";
    static constexpr auto KeyTableStartLastPlyrs = "table.startWithLastPlayers";
    static constexpr auto KeyTablePlayer1        = "table.startPlayers[0]";
    static constexpr auto KeyTablePlayer2        = "table.startPlayers[1]";
    static constexpr auto KeyTablePlayer3        = "table.startPlayers[2]";
    static constexpr auto KeyTablePlayer4        = "table.startPlayers[3]";
    static constexpr auto KeyTablePlayer5        = "table.startPlayers[4]";
    static constexpr auto KeyTablePlayer6        = "table.startPlayers[5]";
    static constexpr auto KeyTableMaxSessions    = "table.maxSessions";
    static constexpr auto KeyTableMaxRecentRolls = "table.maxRecentRolls";
    static constexpr auto KeyTableUserIsPlayer   = "table.userIsPlayer";

    // Player
    static constexpr auto KeyPlayerMaxSessions  = "player.maxSessions";

private:
};

/*-----------------------------------------------------------*//**

@class ConfigManager

@brief Manage Craps configuraiton

Layered on top of MultiLayerConfig, this ConfigManager
provides configuration specialized to craps game.

*/

} // namespace Ctrl

//----------------------------------------------------------------
