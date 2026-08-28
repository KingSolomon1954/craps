//----------------------------------------------------------------
//
// File: ScreenBase.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/Surface.h>

namespace Cui
{

class ScreenBase : public Surface
{
public:
    /// @name Lifecycle
    /// @{
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

