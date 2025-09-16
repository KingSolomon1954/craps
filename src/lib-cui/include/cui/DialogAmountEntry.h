//----------------------------------------------------------------
//
// File: DialogAmountEntry.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/MenuBase.h>
#include <gen/MoneyUtils.h>
#include <functional>

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
    static DialogAmountEntry* instance();
   ~DialogAmountEntry();
    /// @}

    /// @name Modifiers
    /// @{
    void clearState();
    void setTitle(const std::string& title);
    void setFillAmount(Gen::Money fillAmount);
    
    using Callback = std::function<void(Gen::Money)>;
    void registerAutoFillCallback(Callback cb);
    void registerQuickBetCallback(Callback cb);
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
    Callback autoFillCb_ = nullptr;
    Callback quickBetCb_ = nullptr;

    DialogAmountEntry();  // Private ctor

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

