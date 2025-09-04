//----------------------------------------------------------------
//
// File: MenuBase.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/Screen.h>

namespace Cui {

class MenuBase : public Screen
{
public:
    /// @name Lifecycle
    /// @{
    MenuBase(ConsoleView& view, int h, int w);
   ~MenuBase();
    /// @}

    /// @name Modifiers
    /// @{
    void draw()            override;
    void handleKey(int ch) override;
    void toggle();
    void setRootMenu(bool root);
    void setOwningScreen(Screen* pOwning);
    /// @}

    /// @name Observers
    /// @{
    WINDOW* win() const;
    /// @}

protected:
    virtual void drawMenu()            = 0;
    virtual void handleMenuKey(int ch) = 0;

    WINDOW* w_             = nullptr;
    bool visible_          = false;
    bool isRoot_           = false;
    Screen* pOwningScreen_ = nullptr;

private:
};

/*-----------------------------------------------------------*//**

@class MenuBase

@brief Base class for menus

Responsibilities of MenuBase

@li knows if it’s visible or not,

@li toggles on spacebar,

@li can optionally handle ESC differently depending on whether it’s the
    root menu or a child menu.

@li draws itself to present menu options

@li processes key presses and takes action

*/

} // namespace Cui

//----------------------------------------------------------------
