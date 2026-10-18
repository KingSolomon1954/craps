//----------------------------------------------------------------
//
// File: DialogAmount.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/bases/DialogBase.h>
#include <gen/MoneyUtils.h>
#include <craps/EnumBetName.h>
#include <array>
#include <functional>

namespace Cui {

class ConsoleView;  // fwd
    
class DialogAmount : public DialogBase
{
public:
    struct Config
    {
        std::string title;
        BetName     betName;
        unsigned    pivot;
        Gen::Money  preFillAmount = 0;
    };
    
    /// @name Lifecycle
    /// @{
   ~DialogAmount() = default;
    static DialogAmount& instance();
    DialogAmount(const DialogAmount&)            = delete;
    DialogAmount& operator=(const DialogAmount&) = delete;
    DialogAmount(DialogAmount&&)                 = delete;
    DialogAmount& operator=(DialogAmount&&)      = delete;
    /// @}

    /// @name Modifiers
    /// @{
    void draw()                          override;
    bool handleKey(int ch)               override;
    void setLocation(WindowPosition pos) override;
    /// @}

    /// @name Observers
    /// @{
    LocationRequest getLocationRequest() const override;
    /// @}
    
private:
    WindowPosition   winPos_;
    const WindowSize winSize_ = {12, 26};  // rows, cols, Fixed size
    Config           cfg_;

    static constexpr std::array<std::string_view, 7> Prompts
    {
        "Enter amount:",
        "",
        "[enter] Submit Bet",
        "[s] Save as AutoFill",
        "[q] Save as Quick Bet",
        "[?] Help",
        "[. or esc] Back"
    };

    std::string title_;
    BetName betName_;
    unsigned pivot_           = 0;
    Gen::Money preFillAmount_ = 0;
    Gen::Money inputAmount_   = 0;
    size_t cursorPos_         = 0;
    std::string lineBuffer_;

private:
    DialogAmount();  // Private ctor

    void clear();
        
    void drawStaticContent();
    void drawBorders();
    void drawPrompts();
    void drawTitle();
    std::string prepTitle(const std::string& title);
    
    bool handleLineInput(int ch);
    void back();
    void doEnter();
    void doAutoFill();
    void doQuickBet();
    void drawInput();
    void processAmount();
};

/*-----------------------------------------------------------*//**

@class DialogAmount

@brief Obtains bet amount from user

DialogAmount prompts the player to enter an amount for
a given bet. The dialog is first "configured" before pushing
to SurfaceManager, similar to other menus and dialogs.

DialogAmount has some special behaviors. Since it is already focused on
a particular bet and amount, it is natural to use it to help configure
some automatic behaviors.

DialogAmount will support two menu entries such that if selected it uses
the current entry value for:

@li Setting the auto fill value for the current bet type, to be used for
    default filling future bets of this type with the given amount.

@li Using the current bet type and amount to configure a Quick Bet. A
    quick bet is shortcut that is assigned to a particular bet and
    amount, and is given a fixed menu entry on the QuickBet menu.  So
    for example, the user presses 'q' (quick bet menu) followed by 'a'
    (the first configured quick bet) could mean make a Place Bet on the
    6 for 100.


Responsibilities:

@li Key bindings for the menu
@li Process input keys 
@li Takes action on input keys 
@li Renders the dialog on screen
@li Functions to establish defaults and fill values
*/

} // namespace Cui

//----------------------------------------------------------------
