//----------------------------------------------------------------
//
// File: ConfigFiles.h
//
//----------------------------------------------------------------

#pragma once

#include <string>

namespace Gen {
    class ConfigLayer;      // fwd
    class MultiLayerConfig; // fwd
}
    
namespace Ctrl {

class ConfigFiles
{
public:
    /// @name Lifecycle
    /// @{
    static void processFiles(Gen::MultiLayerConfig& multiConfig);
    /// @}

    /// @name Modifiers
    /// @{
    /// @}

    /// @name Observers
    /// @{
    /// @}
private:
    static void createUserDirs (const Gen::MultiLayerConfig& multiConfig);
    static void checkSystemDirs(const Gen::MultiLayerConfig& multiConfig);
    static void loadSystemConfig(
        const Gen::MultiLayerConfig& multiConfig, Gen::ConfigLayer& cfg);
    static void loadUserConfig(
        const Gen::MultiLayerConfig& multiConfig, Gen::ConfigLayer& cfg);
    static void existsOrThrow(const std::string& pathStr);
    static void loadNamedConfig(const std::string& filename,
                                Gen::ConfigLayer& cfg);
};

/*-----------------------------------------------------------*//**

@class ConfigFiles

@brief Process startup configuration files

Supply class description.

*/

} // namespace Ctrl

//----------------------------------------------------------------
