//----------------------------------------------------------------
//
// File: MenuPivot.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/MenuBase.h>
#include <craps/EnumBetName.h>

namespace Cui {

class ConsoleView;  // fwd
    
class MenuPivot : public MenuBase
{
public:
    struct Results
    {
        bool canceled;
        size_t pivot;
    };
    
    /// @name Lifecycle
    /// @{
    MenuPivot(ConsoleView& view);
   ~MenuPivot();
    /// @}

    /// @name Modifiers
    /// @{
    void setUpFor(BetName betName);
    /// @}

    /// @name Observers
    /// @{
    Results getResults() const;
    /// @}
    
protected:
    void drawMenu()            override;
    void handleMenuKey(int ch) override;

private:
    BetName     betName_;
    size_t      pivot_;
    std::string title_;
    
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

