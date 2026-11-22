//----------------------------------------------------------------
//
// File: DialogBetAmount.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/bases/DialogBase.h>
#include <cui/core/LineInput.h>
#include <gen/MoneyUtils.h>
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
    static constexpr WindowSize winSize_ = {24, 59};  // rows, cols, fixed size

    static constexpr std::array<std::string_view, 3> Prompts
    {
        "[enter] Submit Bet",
        "[?] Help",
        "[. or esc] Back"
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
    void drawAmountLimits();
    void drawReference();
    void drawStatistics();
    void drawPrompts();
    void drawTitle();
    void drawInput();
    std::string prepTitle(const std::string& title);
    Gen::Money getPrefillAmount() const;
    Gen::Money getLastFill() const;

    // Input processing
    void prepLineInput();
    bool readInputAmount();
    void back();
    void doEnter();
    void doHelp();
    void processAmount();
};

/*-----------------------------------------------------------*//**

@class DialogBetAmount

@brief Obtains bet amount from user

DialogBetAmount prompts the player to enter an amount for
a given bet. The dialog is first "configured" before pushing
to SurfaceManager, similar to other menus and dialogs.

The amount field starts with the amount last used for this bet (see
LastFill). Since the dialog is already focused on a particular bet and
amount, it is the natural place to offer QuickBets and AutoOdds; see
the Bet Automation section of the design document. Those entries are
not here yet.


Responsibilities:

@li Key bindings for the menu
@li Process input keys 
@li Takes action on input keys 
@li Renders the dialog on screen
@li Functions to establish defaults and fill values
*/

} // namespace Cui

//----------------------------------------------------------------
