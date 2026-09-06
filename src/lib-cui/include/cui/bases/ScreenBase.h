//----------------------------------------------------------------
//
// File: ScreenBase.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/bases/SurfaceBase.h>

namespace Cui
{

class ScreenBase : public SurfaceBase
{
public:
    /// @name Lifecycle
    /// @{
    explicit ScreenBase(std::string surfaceName)
        : SurfaceBase(std::move(surfaceName))
    {
    }
   ~ScreenBase() = default;
    /// @}
    
    /// @name Modifiers
    /// @{
    /// @}

    /// @name Observers
    /// @{
    /// @}

    // Screen-specific behaviors used by all screens...
};

/*-----------------------------------------------------------*//**

@class ScreenBase

@brief Specialized surface type for full screens

@li Provides the generic UI surface contract for full screens.

*/

} // namespace Cui

//----------------------------------------------------------------

