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

@brief Display diagnostic in a pop up, user must acknowledege

Caller must configure the diagnostic before pushing it on
SurfaceManager, by calling configure(). The dialog displays several
optional sections depending on the configuration.

There is no result to obtain. When the user presses
a key the dialog is done.

@li Title bar always shows "Error", followed by ErrorType, and ErrorName
@li Section Description, always shows supplied text
@li Section Probable cause appears only if non-empty
@li Section Proposed repair appears only if non-empty
@li Section additional info appears only if non-empty
@li Section Trace appears only if a programmatic type error

@verbatim
┌───────────────────────────────────────────────────────────┐
│ ❌ ERROR❗ Processing Error. Item Already Exists.         │
├───────────────────────────────────────────────────────────┤
│ 📜 Description:                                           │
│                                                           │
├───────────────────────────────────────────────────────────┤
│ 📂 Probable Cause:                                        │
│                                                           │
├───────────────────────────────────────────────────────────┤
│ 🔨 Proposed Repair:                                       │
│                                                           │
├───────────────────────────────────────────────────────────┤
│ 🔆 Additional Info:                                       │
│                                                           │
├───────────────────────────────────────────────────────────┤
│ 👇 Trace                                                  │
│                                                           │
├───────────────────────────────────────────────────────────┤
│ [. or esc] OK                                             │
└───────────────────────────────────────────────────────────┘
@endverbatim

Responsibilities of DialogAckError:

@li Key bindings for the dialog box
@li Process input keys 
@li Takes action on input keys 
@li Renders the dialog on screen
@li Function to establish message in dialog box

*/

} // namespace Cui

//----------------------------------------------------------------
