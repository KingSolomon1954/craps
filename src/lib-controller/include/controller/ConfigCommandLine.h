//----------------------------------------------------------------
//
// File: ConfigCommandLine.h
//
//----------------------------------------------------------------

#pragma once

#include <string>

namespace Gen {
    class ConfigLayer;       // fwd
    class MultiLayerConfig;  // fwd
}
    
namespace Ctrl {

class ConfigCommandLine
{
public:
    /// @name Lifecycle
    /// @{
    static void processCmdLine(int argc, char* argv[],
                               Gen::MultiLayerConfig& multiConfig);
    /// @}

    /// @name Modifiers
    /// @{
    /// @}

    /// @name Observers
    /// @{
    /// @}
private:
    static void setAppPath(char* argv[], Gen::ConfigLayer& cfg);
    static void setSysConfigDir(const std::string& sysSharedStr,
                                Gen::ConfigLayer& cfg);
};

/*-----------------------------------------------------------*//**

@class ConfigCommandLine

@brief Process command line arguments.

Populates the MultiLayerConfig "command line" layer.

*/

} // namespace Ctrl

//----------------------------------------------------------------
