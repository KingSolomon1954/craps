//----------------------------------------------------------------
//
// File: MenuPlaceBet.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/bases/MenuBase.h>

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
    void draw()             override;
    void handleKey(int ch)  override;
    // bool shouldSkip() const override;  base class default is good
    /// @}

    /// @name Observers
    /// @{
    static MenuPlaceBet& instance();
    /// @}
    
private:
    struct Layout
    {
        static constexpr int height = 11;
        static constexpr int width  = 22;

        // TODO - get these from geometry manager
        static constexpr int winStartX = 10;        
        static constexpr int winStartY = 40;
    };

    MenuPlaceBet();  // Private ctor
    void createWindow();
    void registerWindow();
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

