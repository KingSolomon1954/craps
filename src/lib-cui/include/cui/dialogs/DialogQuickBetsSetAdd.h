//----------------------------------------------------------------
//
// File: DialogQuickBetsSetAdd.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/bases/DialogBase.h>
#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

//----------------------------------------------------------------

namespace Cui {

class DialogQuickBetsSetAdd : public DialogBase
{
public:
    /// An existing QuickBets entry to offer
    struct Entry
    {
        std::string name;
        std::string description;   ///< The bets in it, clipped to fit
    };

    /// Result index meaning "make a new entry"
    static constexpr std::size_t NewEntry = static_cast<std::size_t>(-1);

    /// @name Lifecycle
    /// @{
    static DialogQuickBetsSetAdd& instance();
    DialogQuickBetsSetAdd(const DialogQuickBetsSetAdd&)            = delete;
    DialogQuickBetsSetAdd& operator=(const DialogQuickBetsSetAdd&) = delete;
    DialogQuickBetsSetAdd(DialogQuickBetsSetAdd&&)                 = delete;
    DialogQuickBetsSetAdd& operator=(DialogQuickBetsSetAdd&&)      = delete;
    /// @}

    /// @name Modifiers
    /// @{
    void draw()                          override;
    bool handleKey(int ch)               override;
    void onAttach(SurfaceBase* pParent)  override;
    void onDetach()                      override;
    void setLocation(WindowPosition pos) override;

    /// betText is the bet being saved, e.g. "Place bet 6 $24"
    void configure(std::string_view betText, std::vector<Entry> entries);
    /// @}

    /// @name Observers
    /// @{
    LocationRequest getLocationRequest() const override;
    /// @}

private:
    static constexpr std::size_t maxVisible_   = 12;  // Entries shown at once
    static constexpr std::size_t maxInnerCols_ = 70;
    static constexpr std::size_t maxEntries_   = 26;  // Letters a-z

    DialogQuickBetsSetAdd();

    void calcSize();
    void drawAll();
    void scrollList(int delta);
    void choose(std::size_t index);
    void cancel();

private:
    WindowPosition     winPos_{};
    WindowSize         winSize_ = {10, 44};  // Arbitrary for initialization
    bool               configured_ = false;
    std::string        title_;
    std::vector<Entry> entries_;
    std::size_t        top_ = 0;             // First entry shown
};

/*-----------------------------------------------------------*//**

@class DialogQuickBetsSetAdd

@brief Saves the bet being entered into a QuickBets entry

Offered from DialogBetAmount. The first choice makes a new entry
holding this bet. The others add the bet to one of the player's existing
entries, each with a letter. When there are more entries than fit, the
Up and Down keys scroll the list; the letters always pick by position in
the whole list.

@code
┌──────────────────────────────────────────┐
│ QuickBets Set or Add   (Place bet 6 $24) │
├──────────────────────────────────────────┤
│ [1] Set as new QuickBets entry           │
├──────────────────────────────────────────┤
│ Add to an existing entry:                │
│ [a] Place Hi  | Place 8 $24, Place 9 ... │
│ [b] Place Lo  | Place 4 $20, Place 5 ... │
│ [. or esc] Back                          │
└──────────────────────────────────────────┘
@endcode

The dialog pops itself and reports to OperationManager::onQuickBetsChoice().

*/

} // namespace Cui

//----------------------------------------------------------------
