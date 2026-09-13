//----------------------------------------------------------------
//
// File: MenuPlaceBet.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/bases/MenuBase.h>
#include <cui/CuiStructs.h>

namespace Cui {

class MenuPlaceBet : public MenuBase
{
public:
    /// @name Lifecycle
    /// @{
   ~MenuPlaceBet() = default;
    /// @}

    /// @name Modifiers
    /// @{
    void draw()                          override;
    bool handleKey(int ch)               override;
    void setLocation(WindowPosition pos) override;
    /// @}

    /// @name Observers
    /// @{
    static MenuPlaceBet& instance();
    LocationRequest getLocationRequest() const override;
    /// @}
    
private:
    WindowPosition  winPos_;
    WindowSize      winSize_ = {11, 22};  // rows, cols

private:
    MenuPlaceBet();  // Private ctor
    void fillWindow();
    void processSelection(int num);
    void populateCarrier (int pivot);
    void prepDialogAmount(int pivot);
    void activateDialogAmount();
    void back();
};

/*-----------------------------------------------------------*//**

@class MenuPlaceBet

@brief Display choices for Place Bets

Responsibilities of MenuPlaceBet

@li Key bindings for the menu
@li Process input keys 
@li Takes action on input keys 
@li Renders the menu on screen
@li Functions to establish defaults and fill values

*/

} // namespace Cui

//----------------------------------------------------------------

