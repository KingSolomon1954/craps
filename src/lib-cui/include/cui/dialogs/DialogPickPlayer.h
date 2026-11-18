//----------------------------------------------------------------
//
// File: DialogPickPlayer.h
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

class DialogPickPlayer : public DialogBase
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
        std::string     note;     ///< Shown at the right, e.g. "(you)"
    };

    /// @name Lifecycle
    /// @{
    static DialogPickPlayer& instance();
    DialogPickPlayer(const DialogPickPlayer&)            = delete;
    DialogPickPlayer& operator=(const DialogPickPlayer&) = delete;
    DialogPickPlayer(DialogPickPlayer&&)                 = delete;
    DialogPickPlayer& operator=(DialogPickPlayer&&)      = delete;
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

    DialogPickPlayer();

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
};

/*-----------------------------------------------------------*//**

@class DialogPickPlayer

@brief Shows a short list of players, optionally to pick one

Pick mode: Up/Down move, Enter chooses, 1-9 chooses that line, Esc
cancels. Show mode: the same list for looking only; Enter or Esc
closes it.

The dialog pops itself and reports to OperationManager::onPickResult().
The list scrolls when there are more than eight players.

*/

} // namespace Cui

//----------------------------------------------------------------
