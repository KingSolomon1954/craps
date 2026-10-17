//----------------------------------------------------------------
//
// File: DialogError.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/bases/DialogBase.h>
#include <gen/ErrorPass.h>
#include <string>
#include <vector>

namespace Cui {

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
        Aos fmtdText;
        int nRows = 0;
        int nCols = 0;
        void clear()
        {
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

    const std::wstring titleStr_          = L" Error ";
    const std::wstring descriptionStr_    = L" Description:";
    const std::wstring probableCauseStr_  = L" Probable Cause:";
    const std::wstring proposedRepairStr_ = L" Proposed Repair:";
    const std::wstring additionalInfoStr_ = L" Additional Info:";
    const std::wstring traceStr_          = L" Trace:";

private:
    DialogError();

    // Creating dialog box entries
    void eraseFields();
    void buildDialogEntries  (const Gen::ErrorPass& ep);
    void formatTitle(const Gen::ErrorPass& ep);
    void formatDescription   (const std::string& description);
    void formatProbableCause (const std::string& probableCause);
    void formatProposedRepair(const std::string& proposedRepair);
    void formatAdditionalInfo(const std::string& additionalInfo);
    void formatTrace         (const Gen::ErrorPass& ep);
    void wrapString (const std::string& s, Aos& result);
    void wrapStrings(const Aos& source,    Aos& result);
    void calcSize();
    int wrapWidth();
    int maxNumCols();
    int numRows();
    int maxLength (const Aos& vos);
    int numSections();

    // Window creation and drawing
    int drawTitle       (int startRow);
    int drawDesc        (int startRow);
    int drawCause       (int startRow);
    int drawRepair      (int startRow);
    int drawAddinfo     (int startRow);
    int drawTrace       (int startRow);
    int drawPrompt      (int startRow);
    int drawIntroTitle  (int startRow);
    int drawIntroDesc   (int startRow);
    int drawIntroCause  (int startRow);
    int drawIntroRepair (int startRow);
    int drawIntroAddinfo(int startRow);
    int drawIntroTrace  (int startRow);
    int drawSectionSep  (int startRow);
    int drawFmtdText    (int startRow, const Aos& text);
    void attrTitle  (bool onOff);
    void attrDesc   (bool onOff);
    void attrCause  (bool onOff);
    void attrRepair (bool onOff);
    void attrAddinfo(bool onOff);
    void attrTrace  (bool onOff);

    // Processing hotKeys
    void back();
};

/*-----------------------------------------------------------*//**

@class DialogError

@brief Display diagnostic in a pop up, user must acknowledege

Caller must call configure before pushing dialog to
SurfaceManager. The dialog displays several sections
some of which are optional depending on the configuration.

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
