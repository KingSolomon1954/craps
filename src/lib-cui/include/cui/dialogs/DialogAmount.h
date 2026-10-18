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
    void onAttach(SurfaceBase* pParent)  override;  // from Surface
    void onDetach()                      override;  // from Surface

    /// @}

    /// @name Observers
    /// @{
    LocationRequest getLocationRequest() const override;
    /// @}
    
private:
    WindowPosition winPos_;
    static constexpr WindowSize winSize_ = {12, 26};  // rows, cols, Fixed size

    static constexpr std::array<std::string_view, 7> Prompts
    {
        "Enter amount:",
        "",
        "[enter] Submit Bet",
        "[a] Save as AutoFill",
        "[q] Save as Quick Bet",
        "[?] Help",
        "[. or esc] Back"
    };

    // Line buffer management
    std::string lineBuffer_;
    Gen::Money inputAmount_ = 0;
    size_t cursorPos_       = 0;
    static constexpr int inputRow_ = 4;
    static constexpr int inputCol_ = 2 + Prompts[0].size() + 1;
    static constexpr int numInputBlanks_ = winSize_.cols - inputCol_ - 1;
    static constexpr int inputCharsMax_ = 8;
    const std::string eraseInput_ = std::string(numInputBlanks_, ' ');

private:
    DialogAmount();  // Private ctor

    // Drawing
    void drawStaticContent();
    void drawBorders();
    void drawPrompts();
    void drawTitle();
    void drawInput();
    std::string prepTitle(const std::string& title);

    // Input processing
    void prepLineInput();
    void clearLineInput();
    bool handleLineInput(int ch);
    void back();
    void doEnter();
    void doAutoFill();
    void doQuickBet();
    void doHelp();
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
