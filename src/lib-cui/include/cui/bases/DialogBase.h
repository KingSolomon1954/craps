//----------------------------------------------------------------
//
// File: DialogBase.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/bases/SurfaceBase.h>

namespace Cui {

class DialogBase : public SurfaceBase
{
public:
    /// @name Lifecycle
    /// @{
    explicit DialogBase(std::string surfaceName)
        : SurfaceBase(std::move(surfaceName))
    {
    }
    ~DialogBase() = default;
    /// @}

    /// @name Modifiers
    /// @{
    /// @}

    /// @name Observers
    /// @{
    /// @}

protected:

private:
    
};

/*-----------------------------------------------------------*//**

@class DialogBase

@brief Specialized surface type for dialog boxes

@li Implements virtual Surface interface
@li Provides logic common for all dialog classes

*/

} // namespace Cui

//----------------------------------------------------------------
