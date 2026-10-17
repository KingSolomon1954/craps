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
    enum class MessageLabel
    {
        Unset,     // Unset means user failed to configure
        None,      // Caller doesn't want a label at all
        Custom,    // Caller is providing their own label
        Info,      // Use precanned info label 
        Warning,   // Use precanned warning label
        Error      // Use precanned error label
    };
    
    struct MessageText
    {
        MessageLabel label = MessageLabel::Unset;
        std::string customLabel;
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
    int buildDialogEntries();
    int formatPrompt();
    int maxColsTitle();
    int maxColsDetail();
    void calcSize(int maxCols);
    void eraseFields();
    void formatText();

    // Window creation and drawing
    void drawBorders();
    void drawPrompt();
    void drawTitle();
    void drawText();
    void drawCustomLabel();
    void drawInfoLabel();
    void drawWarnLabel();
    void drawErrorLabel();
    void attrOn    (bool onOff);
    void attrCustom(bool onOff);
    void attrInfo  (bool onOff);
    void attrWarn  (bool onOff);
    void attrError (bool onOff);
    
    // Processing hotKeys
    void back();
    
private:
    WindowPosition winPos_;
    WindowSize     winSize_ = {5, 50};  // Arbitrary for initializaiton
    MessageText    msg_;

    struct MessageFormatted
    {
        std::string title;
        std::vector<std::string> detail;
        std::string prompt;
    };
    MessageFormatted msgFmtd_;

    const std::string InfoLabel_  = "Info: ";
    const std::string WarnLabel_  = "Warning: ";
    const std::string ErrorLabel_ = "Error: ";
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

DialogMessage::MessageText m;
m.type = DialogMessage::MessageType::Info;
m.brief = "No qualifying odds capable bets";
m.detail = "Of the 4 bets you have, none are capable of betting odds."
           "Only Pass, Dont Pass, Come and Dont Come bets can have odds";
DialogMessage::instance().configure(m);

@endcode

When the message is acknowledged by the user, the result is propagated
and available from SurfaceBase::getOperationResult() and has the value
of "Acknowledged".

*/

} // namespace Cui

//----------------------------------------------------------------
