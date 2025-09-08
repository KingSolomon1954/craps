//----------------------------------------------------------------
//
// File: MenuBetting.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/MenuBase.h>

namespace Cui {

class ConsoleView;       // fwd
class ScreenCrapsTable;  // fwd
    
class MenuBetting : public MenuBase
{
public:
    /// @name Lifecycle
    /// @{
    MenuBetting(ConsoleView& view);
   ~MenuBetting();
    /// @}

    /// @name Modifiers
    /// @{
    void setOwningScreen(ScreenCrapsTable* pOwning);
    /// @}

    /// @name Observers
    /// @{
    /// @}
    
protected:
    void drawMenu()            override;
    void handleMenuKey(int ch) override;

private:
    ScreenCrapsTable* pOwning_ = nullptr;
};

/*-----------------------------------------------------------*//**

@class MenuBetting

@brief Root menu for ScreeCrapsTable class

Responsibilities of MenuBetting:

@li Key bindings for the menu

@li Process input keys 

@li Takes action on input keys 

@li Renders the menu on screen

*/

} // namespace Cui

//----------------------------------------------------------------

