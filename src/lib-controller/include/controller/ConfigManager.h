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
    static constexpr auto KeyStartTable     = "table.startTableId";
    static constexpr auto KeyTablePlayer1   = "table.playerId-1";
    static constexpr auto KeyTablePlayer2   = "table.playerId-2";
    static constexpr auto KeyTablePlayer3   = "table.playerId-3";
    static constexpr auto KeyTablePlayer4   = "table.playerId-4";
    static constexpr auto KeyTablePlayer5   = "table.playerId-5";
    static constexpr auto KeyTablePlayer6   = "table.playerId-6";
    static constexpr auto KeySoundEnabled   = "sound.enabled";
    static constexpr auto KeySoundVolume    = "sound.volume";
        
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
