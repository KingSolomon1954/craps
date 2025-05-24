//----------------------------------------------------------------
//
// File: ConfigDefaults.h
//
//----------------------------------------------------------------

#pragma once

#include <string>

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

private:
    static void loadDefaultDirs(Gen::ConfigLayer& cfg);
    static std::string getEnvOrDefault(const std::string& envVar,
                                       const std::string& fallback);
    static void loadGameDefaults(Gen::ConfigLayer& cfg);
};

/*-----------------------------------------------------------*//**

@class ConfigDefaults

@brief Process startup defaults

Populates the MultiLayerConfig "default" layer with values.

*/

} // namespace Ctrl

//----------------------------------------------------------------
