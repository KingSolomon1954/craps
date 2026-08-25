//----------------------------------------------------------------
//
// File: MenuPlaceBet.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/MenuBase.h>

namespace Cui {

class MenuPlaceBet : public MenuBase
{
public:
    /// @name Lifecycle
    /// @{
   ~MenuPlaceBet();
    /// @}

    /// @name Modifiers
    /// @{
    void draw()            override;
    void onAttach()        override;
    void onDetach()        override;
    void onPause()         override;
    void onResume()        override;
    void handleKey(int ch) override;
    /// @}

    /// @name Observers
    /// @{
    static MenuPlaceBet* instance();
    /// @}
    
protected:
    void draw()            override;
    void handleKey(int ch) override;

private:
    struct Layout
    {
        // 0  ┌────────────────────┐
        // 1  │ Place Which Number │
        // 2  ├────────────────────┤
        // 3  │ [4] Place 4        │
        // 4  │ [5] Place 5        │
        // 5  │ [6] Place 6        │
        // 6  │ [8] Place 8        │
        // 7  │ [9] Place 9        │
        // 8  │ [0] Place 10       │
        // 9  │ [esc] Back         │
        // 10 └────────────────────┘

        static constexpr int winStartX = 10;        
        static constexpr int winStartY = 40;
        static constexpr int winHeight = 11;
        static constexpr int winWidth  = 22;
    };

    MenuPlaceBet();  // Private ctor
    void fillWindow();
    
    void doPivot(int num);
    void clearState();
};

/*-----------------------------------------------------------*//**

@class MenuPivot

@brief Display choices for bet Pivots

Responsibilities of MenuPivot:

@li Key bindings for the menu

@li Process input keys 

@li Takes action on input keys 

@li Renders the menu on screen

@li Functions to establish defaults and fill values

*/

} // namespace Cui

//----------------------------------------------------------------

