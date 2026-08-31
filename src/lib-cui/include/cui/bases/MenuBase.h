//----------------------------------------------------------------
//
// File: MenuBase.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/bases/SurfaceBase.h>
#include <string_view>

namespace Cui {

class MenuBase : public SurfaceBase
{
public:
    /// @name Lifecycle
    /// @{
   ~MenuBase() = default;
    /// @}

    /// @name Modifiers
    /// @{
    /// @}

    /// @name Observers
    /// @{
    /// @}

protected:
    static constexpr std::string_view MenuHotKeys =
        "123456789abcdefghijklmnopqrstuvwxyz";

private:
    
};

/*-----------------------------------------------------------*//**

@class MenuBase

@brief Specialized surface type for menus 

@li Implements virtual Surface interface
@li Provides logic common for all menu UI classes

*/

} // namespace Cui

//----------------------------------------------------------------
