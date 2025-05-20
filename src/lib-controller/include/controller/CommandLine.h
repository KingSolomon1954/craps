//----------------------------------------------------------------
//
// File: CommandLine.h
//
//----------------------------------------------------------------

#pragma once

namespace Gen
{
    class BuildInfo;  // fwd
}

namespace Craps {

class CommandLine
{
public:
    /// @name Lifecycle
    /// @{
    CommandLine(int argc, char* argv[], const Gen::BuildInfo& bld);
   ~CommandLine() = default;
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

} // namespace Craps

//----------------------------------------------------------------
