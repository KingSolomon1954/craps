//----------------------------------------------------------------
//
// File: ConfigEnv.h
//
//----------------------------------------------------------------

#pragma once

namespace Gen {
    class MultiLayerConfig; // fwd
    class ConfigLayer;      // fwd
}
    
namespace Ctrl {

class ConfigEnv
{
public:
    /// @name Lifecycle
    /// @{
    static void processEnv(const Gen::MultiLayerConfig& multiConfig,
                           Gen::ConfigLayer& Cfg);
    /// @}

    /// @name Modifiers
    /// @{
    /// @}

    /// @name Observers
    /// @{
    /// @}
};

/*-----------------------------------------------------------*//**

@class ConfigEnv

@brief Process environment vars at startup

Populates MultiLayerConfig "env" layer with environment
variables.

*/

} // namespace Ctrl

//----------------------------------------------------------------
