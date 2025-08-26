//----------------------------------------------------------------
//
// File: Consoleview.h
//
//----------------------------------------------------------------

#pragma once

#include <controller/ViewIntfc.h>
#include <atomic>
#include <unordered_map>
#include <string>
#include <thread>
#include <vector>
#include <mutex>
#include <ncurses.h>
#include <cui/Screen.h>

namespace Cui {

class ConsoleView : public Ctrl::ViewIntfc
{
public:
    enum class InputMode
    {
        Menu,   // single-key
        Line    // line input with Enter
    };

    enum class ScreenId
    {
        CrapsTable,
        Stats,
        Help
        // ...
    };

    /// @name Lifecycle
    /// @{
    ConsoleView();
   ~ConsoleView();
    void init() override;
    void prepareForShutdown() override;
    void registerScreen(ScreenId id, std::unique_ptr<Screen> pScreen);
    /// @}

    /// @name StackOps
    /// @{
    void setScreen(ScreenId id);   // clear stack, push this (replace)
    void pushScreen(ScreenId id);  // overlay (pauses previous top)
    void popScreen();              // remove top, resume new top if any
    /// @}

    /// @name Rendering
    /// @{
    void redraw();                 // Optional explicit redraw (e.g., on resize)
    /// @}

    /// @name InputHandling
    /// @{
    void setInputMode(InputMode mode);
    void inputThreadFunc();        // Input thread -> forward to top
    /// @}

private:
    std::unordered_map<ScreenId, std::unique_ptr<Screen>> registry_; // owns screens
    std::vector<Screen*> stack_;                                     // non-owning stack
    std::mutex stackMx_;
    std::atomic<bool> running_{false};
    std::thread inputThread_;

    InputMode inputMode_{InputMode::Menu};
    std::string lineBuffer_;
    WINDOW* inputWin_{nullptr};

    Screen* topUnlocked();
    void redrawUnlocked();
    void setScreenUnlocked(Screen* s);
    void handleMenuInput(int ch);
    void handleLineInput(int ch);
};

/*-----------------------------------------------------------*//**

@class ConsoleView

@brief Console interaction with the user.

Responsibilities of ConsoleView

UI Initialization & Lifecycle
@li Initialize and shut down the ncurses environment safely.
@li Create, manage, and destroy ncurses windows/subwindows (main window, roll history window, player panels, etc.).
@li Handle resizing/refresh/redraw of windows.

Rendering / Drawing

@li Render the game state received from the controller:
@li Main table layout (dice, bets, point marker, etc.)
@li Player panels (name, balance, bets).
@li Roll history, session stats, messages.
@li Apply formatting: borders, highlights, bold/red for special conditions (e.g. seven-out).
@li Refresh subwindows efficiently (minimize flicker).
@li Animation. Simple dice roll animation, flashing text for a win/loss.

Input Handling

@li Capture keyboard input:
@li Single-key commands (e.g. q to quit, r to roll).
@li Multi-character input (typed names, bet amounts).
@li Capture pointer/mouse input (if enabled):
@li Translate clicks into logical events (select bet, select player, etc.).

Event Translation

@li Convert raw input into domain events (UIEvent / ViewEvent) to pass back to the controller.
@li Example: "k" → Event::RollDice
@li "b <amount>" → Event::PlaceBet
@li Mouse click on a player panel → Event::SelectPlayer

Window Layout Management

@li Decide how to split screen into panels/subwindows.
@li Owns the geometry (row/col sizes) but doesn’t interpret meaning of data.
@li Responsible for redrawing on terminal resize.

Not the responsibility of ConsoleView

@li Any game rules or logic (e.g., bet validation, dice rolling, stats updates).
@li Maintaining the actual game state (that’s the Model / CrapsGame).
@li Deciding what events mean — it only emits them, the controller interprets.

File organization

ConsoleView is the main entry point. It calls out to various
helper files/classes.

ConsoleView.cpp → owns all subwindows, input and lifecycle + high-level
glue (render()),

DrawTable.cpp → all functions for table layout (drawMainTable, dice,
point marker).

DrawPlayers.cpp → rendering players + their bets.

DrawHistory.cpp → roll history + log coloring.

DrawStats.cpp → stats panels.

Input.cpp → keyboard + mouse handling.

Layout.cpp → resize/layout recalculation logic.

*/

} // namespace Cui

//----------------------------------------------------------------
