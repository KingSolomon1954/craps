//----------------------------------------------------------------
//
// File: DialogAckError.h
//
//----------------------------------------------------------------

#pragma once

#include <string>
#include <cui/MenuBase.h>

namespace Cui {

class ConsoleView;  // fwd
    
class DialogAckError : public MenuBase
{
public:
    /// @name Lifecycle
    /// @{
    static DialogAckError* instance();
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
    std::string message_;
    
    DialogAckError();  // Private ctor
    
    void clearState();
    void doAck();
};

/*-----------------------------------------------------------*//**

@class DialogAckError

@brief Display diagnostic in a pop up, required user to enter or ok

There is no result to obtain. When the user presses
a key the dialog is done.

Responsibilities of DialogAckError:

@li Key bindings for the dialog box

@li Process input keys 

@li Takes action on input keys 

@li Renders the dialog on screen

@li Function to establish message in dialog box

*/

} // namespace Cui

//----------------------------------------------------------------

