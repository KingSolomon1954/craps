//----------------------------------------------------------------
//
// File: ConfigFiles.h
//
//----------------------------------------------------------------

#pragma once

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
};

/*-----------------------------------------------------------*//**

@class ConfigFiles

@brief Process startup configuration files

Supply class description.

*/

} // namespace Ctrl

//----------------------------------------------------------------
