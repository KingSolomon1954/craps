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
    void processSelection();
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

@brief Dialog box showing a message. User must acknowledge.

The dialog accepts:

@li [. or esc or enter]

The dialog is configured with a caller-supplied message,
consisting of three fields:

@code

DialogMessage::instance().configure(
    MessageType::Warning,             // message type
    "Invalid bet",                    // message brief
    "The bet amount is not valid.");  // description
@endcode

The result is available through getResult() and is always
set to "acknowledged" which is propagated up the chain of
surfaces.

*/

} // namespace Cui

//----------------------------------------------------------------
