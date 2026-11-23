//----------------------------------------------------------------
//
// File: DialogPicker.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/bases/DialogBase.h>
#include <craps/CrapsTypes.h>
#include <string>
#include <string_view>
#include <vector>

//----------------------------------------------------------------

namespace Cui {

class DialogPicker : public DialogBase
{
public:
    enum class Mode
    {
        Pick,   ///< Choose one; Esc cancels
        Show    ///< Only look at the list; Enter or Esc closes
    };

    struct Item
    {
        Craps::PlayerId playerId;
        std::string     text;     ///< Usually the name
        std::string     note;     ///< Shown after the name, e.g. "(you)"
        std::string     description; ///< Shown after a vertical bar; clipped
        Craps::BetId    betId = 0;   ///< Set when the list is bets, not players
        std::size_t     tag   = 0;   ///< Caller's own meaning, handed back
    };

    /// @name Lifecycle
    /// @{
    static DialogPicker& instance();
    DialogPicker(const DialogPicker&)            = delete;
    DialogPicker& operator=(const DialogPicker&) = delete;
    DialogPicker(DialogPicker&&)                 = delete;
    DialogPicker& operator=(DialogPicker&&)      = delete;
    /// @}

    /// @name Modifiers
    /// @{
    void draw()                          override;
    bool handleKey(int ch)               override;
    void onAttach(SurfaceBase* pParent)  override;
    void onDetach()                      override;
    void setLocation(WindowPosition pos) override;
    void configure(std::string_view title, std::vector<Item> items,
                   Mode mode);
    /// @}

    /// @name Observers
    /// @{
    LocationRequest getLocationRequest() const override;
    /// @}

private:
    static constexpr std::size_t maxVisible_ = 8;
    static constexpr std::size_t maxInnerCols_ = 76;   // 80 column dialog

    DialogPicker();

    std::string hint() const;
    void calcSize();
    void drawFrame();
    void drawItems();
    void moveSelection(int delta);
    void choose(std::size_t index);
    void closeWithoutChoice();

private:
    WindowPosition    winPos_{};
    WindowSize        winSize_ = {8, 40};  // Arbitrary for initialization
    bool              configured_ = false;
    std::string       title_;
    std::vector<Item> items_;
    Mode              mode_ = Mode::Pick;
    std::size_t       selected_ = 0;
    std::size_t       top_ = 0;            // First visible item
    std::size_t       textWidth_ = 0;
    std::size_t       noteWidth_ = 0;
    std::size_t       descWidth_ = 0;      // 0 if no descriptions
};

/*-----------------------------------------------------------*//**

@class DialogPicker

@brief Shows a short list, usually to pick one

Each line is the name, an optional note, then a vertical bar and the
short description, aligned on the bar and clipped to keep the dialog
within 80 columns.

Pick mode: Up/Down move, Enter chooses, a letter chooses that line (a to
z, so up to 26 lines have a key), Esc cancels. Show mode: the same
list for looking only; Enter or Esc closes it.

The dialog pops itself and reports to OperationManager::onPickResult().
The list scrolls when there are more than eight lines. Used for
players, bets and any other short list the caller describes with Item.

*/

} // namespace Cui

//----------------------------------------------------------------
