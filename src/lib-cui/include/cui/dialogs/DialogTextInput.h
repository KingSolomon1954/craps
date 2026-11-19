//----------------------------------------------------------------
//
// File: DialogTextInput.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/bases/DialogBase.h>
#include <cui/core/LineInput.h>
#include <functional>
#include <string>
#include <string_view>

//----------------------------------------------------------------

namespace Cui {

class DialogTextInput : public DialogBase
{
public:
    /// Returns "" if the text is acceptable, otherwise a short sentence
    /// telling the person what is wrong with it.
    using Validator = std::function<std::string(const std::string&)>;

    /// @name Lifecycle
    /// @{
    static DialogTextInput& instance();
    DialogTextInput(const DialogTextInput&)            = delete;
    DialogTextInput& operator=(const DialogTextInput&) = delete;
    DialogTextInput(DialogTextInput&&)                 = delete;
    DialogTextInput& operator=(DialogTextInput&&)      = delete;
    /// @}

    /// @name Modifiers
    /// @{
    void draw()                          override;
    bool handleKey(int ch)               override;
    void onAttach(SurfaceBase* pParent)  override;
    void onDetach()                      override;
    void setLocation(WindowPosition pos) override;
    void configure(std::string_view title,
                   std::string_view prompt,
                   std::string_view initial,
                   std::size_t      maxChars,
                   Validator        validator);
    /// @}

    /// @name Observers
    /// @{
    LocationRequest getLocationRequest() const override;
    /// @}

private:
    static constexpr int rows_ = 7;

    DialogTextInput();

    void calcSize();
    void drawFrame();
    void drawInput();
    void doEnter();
    void doCancel();

private:
    WindowPosition  winPos_{};
    WindowSize      winSize_ = {rows_, 40};  // Arbitrary for initialization
    bool            configured_ = false;
    std::string     title_;
    std::string     prompt_;
    std::string     message_;                // Why the text was refused
    std::size_t     maxChars_ = 16;
    Validator       validator_;
    LineInput       lineInput_{16, LineInput::Charset::PlayerName};
    int             inputRow_ = 3;
    int             inputCol_ = 2;
};

/*-----------------------------------------------------------*//**

@class DialogTextInput

@brief Asks the person to type one line of text, such as a name

@code
DialogTextInput::instance().configure(
    "Rename Player", "Name: ", "Nathan", 16,
    [](const std::string& s) { return s.empty() ? "Needs a name." : ""; });
SurfaceManager::instance().pushSurface(&DialogTextInput::instance());
@endcode

Enter accepts and Esc cancels. Enter with text the validator refuses
keeps the dialog open and shows the reason. The dialog pops itself and
reports to OperationManager::onTextResult().

*/

} // namespace Cui

//----------------------------------------------------------------
