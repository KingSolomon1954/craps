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
    static constexpr auto KeyAppPath     = "app.execAbsPath";
    static constexpr auto KeyDirsShared  = "dirs.shared";
    static constexpr auto KeyDirsConfig  = "dirs.config";
    static constexpr auto KeyDirsData    = "dirs.data";
    static constexpr auto KeyDirsCache   = "dirs.cache";
    static constexpr auto KeyDirsTemp    = "dirs.temp";
    static constexpr auto KeyDirsLog     = "dirs.log";
    static constexpr auto KeyDirsRuntime = "dirs.runtime";
    static constexpr auto KeyDirsTables  = "dirs.tables";
    static constexpr auto KeyDirsPlayers = "dirs.players";
    static constexpr auto KeyViewType    = "screen.viewType";
        
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
