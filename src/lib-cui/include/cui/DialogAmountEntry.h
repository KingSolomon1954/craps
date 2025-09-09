//----------------------------------------------------------------
//
// File: DialogAmountEntry.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/MenuBase.h>
#include <gen/ErrorPass.h>
#include <gen/MoneyUtils.h>
#include <gen/ReturnCode.h>

namespace Cui {

class ConsoleView;  // fwd
    
class DialogAmountEntry : public MenuBase
{
public:
    struct Results
    {
        bool canceled;
        Gen::Money amount;
    };
    
    /// @name Lifecycle
    /// @{
    DialogAmountEntry(ConsoleView& view);
   ~DialogAmountEntry();
    /// @}

    /// @name Modifiers
    /// @{
    void setPrompt(const std::string& prompt);
    void setFillAmount(Gen::Money fillAmount);
    
    // Howie don't think this is needed
    // void setOwningScreen(ScreenCrapsTable* pOwning);
    /// @}

    /// @name Observers
    /// @{
    Results getResults() const;
    /// @}
    
protected:
    void drawMenu()            override;
    void handleMenuKey(int ch) override;

private:
//  Howie don't think this is needed
//  ScreenCrapsTable* pOwning_ = nullptr;
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

