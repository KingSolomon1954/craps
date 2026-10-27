//----------------------------------------------------------------
//
// File: DialogBetAmount.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/bases/DialogBase.h>
#include <cui/core/LineInput.h>
#include <gen/MoneyUtils.h>
#include <craps/EnumBetName.h>
#include <array>
#include <string>
#include <string_view>

namespace Cui {

class DialogBetAmount : public DialogBase
{
public:
    /// @name Lifecycle
    /// @{
   ~DialogBetAmount() = default;
    static DialogBetAmount& instance();
    DialogBetAmount(const DialogBetAmount&)            = delete;
    DialogBetAmount& operator=(const DialogBetAmount&) = delete;
    DialogBetAmount(DialogBetAmount&&)                 = delete;
    DialogBetAmount& operator=(DialogBetAmount&&)      = delete;
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
    WindowPosition winPos_{};
    // "DontCome bet Odds on the 10"  (27)
    static constexpr WindowSize winSize_ = {24, 59};  // rows, cols, fixed size

    static constexpr std::array<std::string_view, 5> Prompts
    {
        "[enter] Submit Bet",
        "[a] Save as AutoFill",
        "[q] Save as Quick Bet",
        "[?] Help",
        "[. or esc] Back"
    };

    using Hints = std::array<std::string_view, 4>;
    static constexpr Hints defaultHints{
        "Pays 1:1                  ",
        "Pays 1:1                  ",
        "Pays 1:1                  ",
        "Pays 1:1                  "
    };

    static constexpr Hints passCome{
        "Pays 2:1 use multiple of 1",
        "Pays 3:2 use multiple of 2",
        "Pays 6:5 use multiple of 5",
        "Pays 1:1                  "
    };
    static constexpr Hints dontPassCome{
        "Pays 1:2 use multiple of 2",
        "Pays 2:3 use multiple of 3",
        "Pays 5:6 use multiple of 6",
        "Pays 1:1                  "
    };
    static constexpr Hints place{
        "Pays 9:5 use multiple of 5",
        "Pays 7:5 use multiple of 5",
        "Pays 7:6 use multiple of 6",
        "Pays 1:1                  "
    };
    static constexpr Hints buy{
        "Pays 2:1 use multiple of 1",
        "Pays 3:2 use multiple of 2",
        "Pays 6:5 use multiple of 6",
        "Pays 1:1                  "
    };
    static constexpr Hints lay{
        "Pays 1:2 use multiple of 2",
        "Pays 2:3 use multiple of 3",
        "Pays 5:6 use multiple of 6",
        "Pays 1:1                  "
    };

    // Input field layout and editing
    static constexpr int inputRow_ = 5;
    static constexpr int inputCol_ = 2 + std::string_view("Enter amount:").size() + 1;
    static constexpr int numInputBlanks_ = winSize_.cols - inputCol_ - 1;
    static constexpr int inputCharsMax_ = 8;
    Gen::Money inputAmount_ = 0;
    LineInput lineInput_{inputCharsMax_};
    const std::string eraseInput_ = std::string(numInputBlanks_, ' ');

private:
    DialogBetAmount();  // Private ctor

    // Drawing
    void drawStaticContent();
    void drawBorders();
    void drawHintMultiples();
    void drawReference();
    void drawStatistics();
    void drawPrompts();
    void drawTitle();
    void drawInput();
    std::string prepTitle(const std::string& title);
    Gen::Money getPrefillAmount() const;
    Gen::Money getAutoFill() const;
    Gen::Money getLastFill() const;

    // Input processing
    void prepLineInput();
    bool readInputAmount();
    void back();
    void doEnter();
    void doAutoFill();
    void doQuickBet();
    void doHelp();
    void processAmount();
};

/*-----------------------------------------------------------*//**

@class DialogBetAmount

@brief Obtains bet amount from user

DialogBetAmount prompts the player to enter an amount for
a given bet. The dialog is first "configured" before pushing
to SurfaceManager, similar to other menus and dialogs.

DialogBetAmount has some special behaviors. Since it is already focused
on a particular bet and amount, it is natural to use it to help 
configure some automatic behaviors.

DialogBetAmount will support two menu entries such that if selected it 
uses the current entry value for:

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
