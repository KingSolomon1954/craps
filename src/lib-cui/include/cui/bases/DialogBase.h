//----------------------------------------------------------------
//
// File: DialogBase.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/Surface.h>

namespace Cui {

class DialogBase : public Surface
{
public:
    /// @name Lifecycle
    /// @{
    DialogBase(std::string surfaceName)
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
