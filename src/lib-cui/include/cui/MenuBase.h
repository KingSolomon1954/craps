//----------------------------------------------------------------
//
// File: MenuBase.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/ViewSurface.h>
#include <ncurses>

namespace Cui {

class WINDOW; // fwd

class MenuBase : public ViewSurface
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

private:
    
};

/*-----------------------------------------------------------*//**

@class MenuBase

@brief Specialized surface type for menus 

@li Implements virtual ViewSurface interface
@li Provides logic common for all menu UI classes

*/

} // namespace Cui

//----------------------------------------------------------------




#ifdef 0
    // From old MenuBase
    void toggle();
    void setRootMenu(bool root);
    Screen::ScreenType type() const override;
    bool visible_    = false;
    bool isRoot_     = false;
    bool isCanceled_ = false;
#endif
