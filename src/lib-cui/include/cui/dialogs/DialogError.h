//----------------------------------------------------------------
//
// File: DialogError.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/bases/DialogBase.h>
#include <gen/ErrorPass.h>
#include <array>
#include <string>
#include <vector>

namespace Cui {

class ConsoleView;  // fwd
    
class DialogError : public DialogBase
{
public:
    /// @name Lifecycle
    /// @{
   ~DialogError() = default;
    static DialogError& instance();
    DialogError(const DialogError&)            = delete;
    DialogError& operator=(const DialogError&) = delete;
    DialogError(DialogError&&)                 = delete;
    DialogError& operator=(DialogError&&)      = delete;
    /// @}

    /// @name Modifiers
    /// @{
    void draw()                          override;
    bool handleKey(int ch)               override;
    void setLocation(WindowPosition pos) override;
    void configure(const Gen::ErrorPass& ep);
    /// @}

    /// @name Observers
    /// @{
    LocationRequest getLocationRequest() const override;
    /// @}
    
private:
    using Aos = std::vector<std::string>;  // Array of strings
    
    struct Section
    {
        std::string introText;
        Aos fmtdText;
        int nRows = 0;
        int nCols = 0;
        void clear()
        {
            introText.clear();
            fmtdText.clear();
            nRows = nCols = 0;
        }
    };
    
    struct ErrorText
    {
        Section title;
        Section desc;
        Section cause;
        Section repair;
        Section addinfo;
        Section trace;
    };

    WindowPosition winPos_;
    WindowSize     winSize_ = {5, 50};  // Arbitrary for initializaiton
    ErrorText      et_;

private:
    DialogError();

    void doAck();
    void eraseFields();
    void buildDialogEntries  (const Gen::ErrorPass& ep);
    void formatTitle         (const std::string& errorName,
                              const std::string& errorType);
    void formatDescription   (const std::string& description);
    void formatProbableCause (const std::string& probableCause);
    void formatProposedRepair(const std::string& proposedRepair);
    void formatAdditionalInfo(const std::string& additionalInfo);
    void formatTrace         (const Aos& stackTrace);
    void calcSize();
    int maxNumCols();
    int numRows();
    int numSections();
    int drawDesc   (int startingRow);
    int drawTitle  (int startingRow);
    int drawCause  (int startingRow);
    int drawRepair (int startingRow);
    int drawAddinfo(int startingRow);
    int drawTrace  (int startingRow);
    int drawPrompt (int startingRow);
    void back();
};

/*-----------------------------------------------------------*//**

@class DialogError

@brief Display diagnostic in a pop up, user must acknowledege

Caller must call configure before pushing dialog to
SurfaceManager. The dialog displays several sections
some of which are optional depending on the configuration.

There is no result to obtain. When the user presses
a key the dialog is done.

@li Title bar always shows "Error", followed by ErrorType, and 
    ErrorName. ErrorType and ErrorName can never be empty.
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

Responsibilities of DialogError:

@li Key bindings for the dialog box
@li Process input keys 
@li Takes action on input keys 
@li Renders the dialog on screen

*/

} // namespace Cui

//----------------------------------------------------------------
