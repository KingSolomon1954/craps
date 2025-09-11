//----------------------------------------------------------------
//
// File: DialogAmountEntry.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/MenuBase.h>
#include <gen/MoneyUtils.h>

namespace Cui {

class ConsoleView;  // fwd
    
class DialogAmountEntry : public MenuBase
{
public:
    struct Results
    {
        bool canceled     = false;
        Gen::Money amount = 0;
    };
    
    /// @name Lifecycle
    /// @{
    DialogAmountEntry(ConsoleView& view);
   ~DialogAmountEntry();
    /// @}

    /// @name Modifiers
    /// @{
    void clearState();
    void setTitle(const std::string& title);
    void setFillAmount(Gen::Money fillAmount);
    /// @}

    /// @name Observers
    /// @{
    Results getResults() const;
    /// @}
    
protected:
    void drawMenu()            override;
    void handleMenuKey(int ch) override;

private:
    std::string title_;
    Gen::Money fillAmount_ = 0;
    Gen::Money amount_     = 0;
    size_t cursorPos_      = 0;
    std::string lineBuffer_;

    void doEnter();
    void doAutoFill();
    void doQuickBet();
    void handleLineInput(int ch);
    void drawInput();
};

/*-----------------------------------------------------------*//**

@class DialogAmountEntry

@brief Obtains bet amount from user

Responsibilities of DialogAmountEntry:

@li Key bindings for the menu

@li Process input keys 

@li Takes action on input keys 

@li Renders the dialog on screen

@li Functions to establish defaults and fill values

@li Returns entered values in results structure

*/

} // namespace Cui

//----------------------------------------------------------------

