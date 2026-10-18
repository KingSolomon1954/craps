//----------------------------------------------------------------
//
// File: MenuPivot.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/bases/MenuBase.h>
#include <cui/core/CuiStructs.h>
#include <craps/EnumBetName.h>
#include <string_view>
#include <array>

namespace Cui {

class MenuPivot : public MenuBase
{
public:
    /// @name Lifecycle
    /// @{
   ~MenuPivot() = default;
    static MenuPivot& instance();
    /// @}

    /// @name Modifiers
    /// @{
    void draw()                          override;
    bool handleKey(int ch)               override;
    void setLocation(WindowPosition pos) override;
    void onResume()                      override;
    /// @}

    /// @name Observers
    /// @{
    LocationRequest getLocationRequest() const override;
    /// @}

private:
    WindowPosition   winPos_;
    const WindowSize winSize_ = {11, 24};  // rows, cols
    BetName betName_ = BetName::Invalid;
    
    using NumberPrompts = std::array<std::string_view, 6>;
    
    static constexpr NumberPrompts promptsPlace_
    {
        "[4] Place 4  ",
        "[5] Place 5  ",
        "[6] Place 6  ",
        "[8] Place 8  ",
        "[9] Place 9  ",
        "[0] Place 10 "
    };

    static constexpr NumberPrompts promptsBuy_
    {
        "[4] Buy 4    ",
        "[5] Buy 5    ",
        "[6] Buy 6    ",
        "[8] Buy 8    ",
        "[9] Buy 9    ",
        "[0] Buy 10   "
    };

    static constexpr NumberPrompts promptsLay_
    {
        "[4] Lay 4    ",
        "[5] Lay 5    ",
        "[6] Lay 6    ",
        "[8] Lay 8    ",
        "[9] Lay 9    ",
        "[0] Lay 10   "
    };

    static constexpr NumberPrompts promptsHardway_
    {
        "[4] Hard 4   ",
        "[6] Hard 6   ",
        "[8] Hard 8   ",
        "[0] Hard 10  ",
        "             ",
        "             "
    };

private:
    std::string_view activeTitle_;
    const NumberPrompts* pNumberPrompts_ = nullptr;

private:
    MenuPivot();  // Private ctor

    // Drawing
    void drawStaticContent();
    void drawTitle();
    void drawPrompts();
    void selectTitle();
    void selectPrompts();

    // Input processing
    bool handleKeyHardways(int ch);
    bool handleKeyAllNumbers(int ch);
    void processSelection(int num);
    void populateCarrier (int pivot);
    void activateDialogAmount();
    void back();
};

/*-----------------------------------------------------------*//**

@class MenuPivot

@brief Display choices for Place Bets

Responsibilities of MenuPivot

@li Key bindings for the menu
@li Process input keys 
@li Takes action on input keys 
@li Renders the menu on screen
@li Functions to establish defaults and fill values

*/

} // namespace Cui

//----------------------------------------------------------------

