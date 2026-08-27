//----------------------------------------------------------------
//
// File: MenuBase.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/Surface.h>

namespace Cui {

class WINDOW; // fwd

class MenuBase : public Surface
{
public:
    /// @name Lifecycle
    /// @{
   ~MenuBase() { if (pWin_) delwin(pWin_); }
    /// @}

    /// @name Modifiers
    /// @{
    /// @}

    /// @name Observers
    /// @{
    /// @}

protected:
    WINDOW* pWin_ = nullptr;
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
