//----------------------------------------------------------------
//
// File: ConfigManager.h
//
//----------------------------------------------------------------

#pragma once

#include <controller/AnimationSpeed.h>
#include <controller/RollHistoryFormat.h>
#include <gen/MultiLayerConfig.h>
#include <optional>
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
    /// Changes the speed for the rest of this game (the session layer).
    void setAnimationSpeed(AnimationSpeed speed);
    void setRollHistoryFormat(RollHistoryFormat format);
    /// @}

    /// @name Observers
    /// @{
    void dumpConfig();

    /// The animation speed in effect now: what the person chose this
    /// game, else the command line, config.yaml, last game, or Normal.
    AnimationSpeed animationSpeed() const;

    /// The roll history format in effect now. Same precedence as the
    /// animation speed. Default is 1, the most compact.
    RollHistoryFormat rollHistoryFormat() const;
    std::optional<RollHistoryFormat> rollHistoryFormatToRemember() const;

    /// What state.yaml should remember: this game's choice if there
    /// was one, otherwise what the last game remembered.
    std::optional<AnimationSpeed> animationSpeedToRemember() const;
    /// @}

public:
    static constexpr auto LayerNameDefaults = "defaults";
    static constexpr auto LayerNameFiles    = "files";
    static constexpr auto LayerNameEnv      = "env";
    static constexpr auto LayerNameCmdLine  = "cmdline";
    static constexpr auto LayerNameSession  = "session";   // Changes made while playing

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

    // Dice roll animation: off | fast | normal | slow
    static constexpr auto KeyAnimationSpeed           = "animation.speed";
    // What state.yaml remembered; carried to the next state.yaml
    static constexpr auto KeyAnimationSpeedRemembered = "animation.remembered";

    // Roll history format: 1 to 4. Same key config.yaml has always listed.
    static constexpr auto KeyRollHistoryFormat           = "rollHistory.format";
    static constexpr auto KeyRollHistoryFormatRemembered = "rollHistory.remembered";

    // Startup. Set in the files layer when no user config.yaml existed,
    // see ConfigSeed.
    static constexpr auto KeyStartupFirstRun  = "startup.firstRun";
    
    // Table Setup
    static constexpr auto KeyTableStart          = "table.startTable";
    static constexpr auto KeyTablePlayer1        = "table.startPlayers[0]";
    static constexpr auto KeyTablePlayer2        = "table.startPlayers[1]";
    static constexpr auto KeyTablePlayer3        = "table.startPlayers[2]";
    static constexpr auto KeyTablePlayer4        = "table.startPlayers[3]";
    static constexpr auto KeyTablePlayer5        = "table.startPlayers[4]";
    static constexpr auto KeyTablePlayer6        = "table.startPlayers[5]";
    static constexpr auto KeyTableMaxSessions    = "table.maxSessions";
    static constexpr auto KeyTableMaxRecentRolls = "table.maxRecentRolls";
    static constexpr auto KeyTableUserIsPlayer   = "table.userIsPlayer";

    // Table Setup, as the user states it in config.yaml. All optional;
    // never given a default, since absent means "not stated". See
    // StartChoices.
    static constexpr auto KeyTableStartWithTable   = "table.startWithTable";
    static constexpr auto KeyTableStartWithUser    = "table.startWithUserPlayer";
    static constexpr auto KeyTableStartWithBuddies = "table.startWithBuddyPlayers";
    static constexpr auto KeyTableStartWithThese   = "table.startWithTheseBuddyPlayers";

    // Table Setup, packaged defaults from sys-config.yaml
    static constexpr auto KeyTableDefaultTable     = "table.defaultTable";
    static constexpr auto KeyTableDefaultUser      = "table.defaultUserPlayer";
    static constexpr auto KeyTableDefaultBuddies   = "table.defaultBuddyPlayers";

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
