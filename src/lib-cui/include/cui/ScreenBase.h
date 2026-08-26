//----------------------------------------------------------------
//
// File: ScreenBase.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/ViewSurface.h>

namespace Cui
{

class ScreenBase : public ViewSurface
{
public:
    /// @name Lifecycle
    /// @{
    ~ScreenBase() override = default;
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

