//----------------------------------------------------------------
//
// File: DialogMessage.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/bases/DialogBase.h>
#include <string>
#include <vector>

//----------------------------------------------------------------

namespace Cui {

class DialogMessage : public DialogBase
{
public:
    enum class MessageType
    {
        Unset,     // Unset means user failed to configure
        None,      // Caller doesn't want anything displayed for type
        Custom,    // Caller wants their own text displayed for type
        Info,      // Precanned formatting for info
        Warning,   // Precanned formatting for warning
        Error      // Precanned formatting for error
    };
    
    struct MessageText
    {
        MessageType type = MessageType::Unset;
        std::string customType;
        std::string brief;
        std::string detail;
    };

    /// @name Lifecycle
    /// @{
   ~DialogMessage() = default;
    static DialogMessage& instance();
    DialogMessage(const DialogMessage&)            = delete;
    DialogMessage& operator=(const DialogMessage&) = delete;
    DialogMessage(DialogMessage&&)                 = delete;
    DialogMessage& operator=(DialogMessage&&)      = delete;
    /// @}

    /// @name Modifiers
    /// @{
    void draw()                          override;
    bool handleKey(int ch)               override;
    void onAttach(SurfaceBase* pParent)  override;
    void onDetach()                      override;
    void setLocation(WindowPosition pos) override;
    void configure(const MessageText& msg);
    /// @}

    /// @name Observers
    /// @{
    LocationRequest getLocationRequest() const override;
    /// @}
    
private:
    DialogMessage();

    // Creating dialog box entries
    void buildDialogEntries();
    void eraseFields();
    void formatTitle();
    void formatDetail();
    void formatPrompt();
    void calcSize();

    // Window creation and drawing
    void drawBorders();
    void drawStaticContent();
    void populate();

    // Processing hotKeys
    void back();
    
private:
    WindowPosition  winPos_;
    WindowSize      winSize_ = {5, 50};  // Arbitrary for initializaiton
    MessageText     msg_;

    struct MessageFormatted
    {
        std::string title;
        std::vector<std::string> detail;
        std::string prompt;
    };
    MessageFormatted msgFmtd_;
};

/*-----------------------------------------------------------*//**

@class DialogMessage

@brief Present a message that the user must acknowledge.

This is used to display errors, warnings, and important info, typically
the result of improper user interactions, but also programming
errors. For example entering an incorrect odds bet amount, or making an
odds bet during a comeout roll.

The class offers a configure() function to set up
a caller-supplied message, consisting of three fields:

@code

DialogMessage::instance().configure(
    MessageType::Warning,             // message type
    "Invalid bet",                    // message brief
    "The bet amount is not valid.");  // description
@endcode

When the message is acknowledged by the user, the result is propagated
and available from SurfaceBase::getOperationResult() and has the value
of "Acknowledged".

*/

} // namespace Cui

//----------------------------------------------------------------
