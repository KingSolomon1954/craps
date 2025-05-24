//----------------------------------------------------------------
//
// File: ConfigDefaults.h
//
//----------------------------------------------------------------

#pragma once

namespace Gen {
    class ConfigLayer;  // fwd
}
    
namespace Ctrl {

class ConfigDefaults
{
public:
    /// @name Lifecycle
    /// @{
    static void processDefaults(Gen::ConfigLayer& cfg);
    /// @}

    /// @name Modifiers
    /// @{
    /// @}

    /// @name Observers
    /// @{
    /// @}
};

/*-----------------------------------------------------------*//**

@class ConfigDefaults

@brief Process startup defaults

Populates the MultiLayerConfig "default" layer with values.

*/

} // namespace Ctrl

//----------------------------------------------------------------
