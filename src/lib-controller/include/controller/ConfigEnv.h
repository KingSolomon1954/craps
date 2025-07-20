//----------------------------------------------------------------
//
// File: ConfigEnv.h
//
//----------------------------------------------------------------

#pragma once

namespace Gen {
    class ConfigLayer;      // fwd
    class MultiLayerConfig; // fwd
}
    
namespace Ctrl {

class ConfigEnv
{
public:
    /// @name Lifecycle
    /// @{
    static void processEnv(Gen::MultiLayerConfig& multiConfig);
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
