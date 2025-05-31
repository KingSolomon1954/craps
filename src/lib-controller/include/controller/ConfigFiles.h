//----------------------------------------------------------------
//
// File: ConfigFiles.h
//
//----------------------------------------------------------------

#pragma once

#include <string>

namespace Gen {
    class MultiLayerConfig; // fwd
    class ConfigLayer;      // fwd
}
    
namespace Ctrl {

class ConfigFiles
{
public:
    /// @name Lifecycle
    /// @{
    static void processFiles(const Gen::MultiLayerConfig& multiConfig,
                             Gen::ConfigLayer& cfg);
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
};

/*-----------------------------------------------------------*//**

@class ConfigFiles

@brief Process startup configuration files

Supply class description.

*/

} // namespace Ctrl

//----------------------------------------------------------------
