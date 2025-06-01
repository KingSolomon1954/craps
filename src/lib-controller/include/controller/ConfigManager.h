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
    static constexpr auto KeyDirsSysShared  = "dirs.sysshared";
    static constexpr auto KeyDirsSysTables  = "dirs.systables";
    static constexpr auto KeyDirsSysPlayers = "dirs.sysplayers";
    static constexpr auto KeyDirsSysConfig  = "dirs.sysconfig";
    static constexpr auto KeyDirsUsrConfig  = "dirs.usrconfig";
    static constexpr auto KeyDirsUsrData    = "dirs.usrdata";
    static constexpr auto KeyDirsUsrCache   = "dirs.usrcache";
    static constexpr auto KeyDirsUsrLog     = "dirs.usrlog";
    static constexpr auto KeyDirsUsrState   = "dirs.usrstate";
    static constexpr auto KeyDirsTemp       = "dirs.temp";
    static constexpr auto KeyViewType       = "screen.viewType";
    static constexpr auto KeySoundEnabled   = "sound.enabled";
    static constexpr auto KeySoundVolume    = "sound.volume";
    
    static constexpr auto KeyTableStart          = "tbl.startId";
    static constexpr auto KeyTableStartLastPlyrs = "tbl.startLastPlyrs";
    static constexpr auto KeyTablePlayer1        = "tbl.plyr1";
    static constexpr auto KeyTablePlayer2        = "tbl.plyr2";
    static constexpr auto KeyTablePlayer3        = "tbl.plyr3";
    static constexpr auto KeyTablePlayer4        = "tbl.plyr4";
    static constexpr auto KeyTablePlayer5        = "tbl.plyr5";
    static constexpr auto KeyTablePlayer6        = "tbl.plyr6";
        
private:
    void populateLayerDefaults();
    void populateLayerFiles();
    void populateLayerEnv();
    void populateLayerCmdLine(int argc, char* argv[]);
};

/*-----------------------------------------------------------*//**

@class ConfigManager

@brief Manage Craps configuraiton

Layered on top of MultiLayerConfig, this ConfigManager
provides configuration specialized to craps game.

*/

} // namespace Ctrl

//----------------------------------------------------------------
