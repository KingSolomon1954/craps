//----------------------------------------------------------------
//
// File: CommandLine.h
//
//----------------------------------------------------------------

#pragma once

namespace Gen {
    class MultiLayerConfig;  // fwd
}
    
namespace Ctrl {

class CommandLine
{
public:
    /// @name Lifecycle
    /// @{
    static void processCmdLine(int argc, char* argv[],
                               Gen::MultiLayerConfig* pCfg);
    /// @}

    /// @name Modifiers
    /// @{
    /// @}

    /// @name Observers
    /// @{
    /// @}
};

/*-----------------------------------------------------------*//**

@class CommandLine

@brief Process command line arguments.

Supply class description.

*/

} // namespace Ctrl

//----------------------------------------------------------------
