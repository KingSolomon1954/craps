//----------------------------------------------------------------
//
// File: ConfigCommandLine.h
//
//----------------------------------------------------------------

#pragma once

namespace Gen {
    class ConfigLayer;  // fwd
}
    
namespace Ctrl {

class ConfigCommandLine
{
public:
    /// @name Lifecycle
    /// @{
    static void processCmdLine(int argc, char* argv[],
                               Gen::ConfigLayer& cfg);
    /// @}

    /// @name Modifiers
    /// @{
    /// @}

    /// @name Observers
    /// @{
    /// @}
private:
    static void setAppPath(char* argv[], Gen::ConfigLayer& cfg);
};

/*-----------------------------------------------------------*//**

@class ConfigCommandLine

@brief Process command line arguments.

Populates the MultiLayerConfig "command line" layer.

*/

} // namespace Ctrl

//----------------------------------------------------------------
