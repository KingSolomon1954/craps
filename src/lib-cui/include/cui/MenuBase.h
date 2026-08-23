//----------------------------------------------------------------
//
// File: MenuBase.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/ViewSurface>

namespace Cui {

class WINDOW; // fwd

class MenuBase : public Screen
{
public:
    /// @name Lifecycle
    /// @{
   ~MenuBase() override = default;
    /// @}

    /// @name Modifiers
    /// @{
    void draw()            override;
    void handleKey(int ch) override;
    /// @}

    /// @name Observers
    /// @{
    /// @}

protected:
    virtual void drawMenu()            = 0;
    virtual void handleMenuKey(int ch) = 0;

    WINDOW* w_ = nullptr;

private:
};

/*-----------------------------------------------------------*//**

@class MenuBase

@brief Specialized surface type for menu screens

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
