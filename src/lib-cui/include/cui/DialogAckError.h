//----------------------------------------------------------------
//
// File: DialogAckError.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/MenuBase.h>

namespace Cui {

class ConsoleView;  // fwd
    
class DialogAckError : public MenuBase
{
public:
    /// @name Lifecycle
    /// @{
    DialogAckError(ConsoleView& view);
   ~DialogAckError();
    /// @}

    /// @name Modifiers
    /// @{
    void setMessage(const std::string& msg);
    /// @}

    /// @name Observers
    /// @{
    /// @}
    
protected:
    void drawMenu()            override;
    void handleMenuKey(int ch) override;

private:
};

/*-----------------------------------------------------------*//**

@class DialogAckError

@brief Display diagnostic in a pop up, required user to enter or ok

Responsibilities of DialogAckError:

@li Key bindings for the dialog box

@li Process input keys 

@li Takes action on input keys 

@li Renders the dialog on screen

@li Functions to establish defaults and fill values

*/

} // namespace Cui

//----------------------------------------------------------------

