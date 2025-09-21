//----------------------------------------------------------------
//
// File: Consoleview.h
//
//----------------------------------------------------------------

#pragma once

#include <controller/ViewInterface.h>
#include <atomic>
#include <unordered_map>
#include <string>
#include <thread>
#include <vector>
#include <mutex>
#include <ncurses.h>

namespace Cui {

class Screen;  // fwd
    
class ConsoleView : public Ctrl::ViewInterface
{
public:
    /// @name Lifecycle
    /// @{
    ConsoleView();
   ~ConsoleView();
    static ConsoleView* instance();
    void init()               override;
    void prepareForShutdown() override;
    /// @}

    /// @name StackOps
    /// @{
    void setScreen (Screen* pScreen);  // clear stack, push this (replace)
    void pushScreen(Screen* pScreen);  // overlay (pauses previous top)
    void popScreen();                  // remove top, resume new top if any
    void redrawUnlocked();
    /// @}

    /// @name InputHandling
    /// @{
    void inputThreadFunc();        // Input thread -> forward to top
    /// @}

    /// @name ViewHandlers
    /// @{
    void onViewErrorDialog(
        Ctrl::EventType    orgEventType,
        uint64_t           correlationId,
        const std::string& diag) override;
    void onViewMakeBetSuccess(
        Craps::BetId betId,
        uint64_t     correlationId) override;
    void onViewAutomationMakeBetSuccess(
        Craps::BetId betId,
        uint64_t     correlationId) override;
    void onViewAutomationMakeBetError(
        Ctrl::EventType        orgEventType,
        uint64_t               correlationId,
        const Craps::PlayerId& playerId,
        const std::string&     diag) override;
    void onViewMakeOddsBetSuccess() override;
    void onViewRollDice()           override;
    void onSignalProgramExit()      override;
    /// @}


    
    WINDOW* makeCenteredWindow(int h, int w);
         
    bool useUnicodePips = false;
    
private:
    std::vector<Screen*> stack_;                                     // non-owning stack
    std::mutex stackMx_;
    std::atomic<bool> running_{true};
    std::thread inputThread_;

    void redraw();
    bool utf8_enabled();
};

/*-----------------------------------------------------------*//**

@class ConsoleView

@brief Console interaction with the user.

Responsibilities of ConsoleView

UI Initialization & Screen hierarchy

@li ConsoleView is the main entry point for CUI ncurses
@li Initialize and shut down the ncurses environment safely
@li Manage window hierarchy (pushing/popping, ...)
@li Asynchronous input thread to obtain keys
@li Each key forwarded to the active screen
@li Delegates to next or previous screen for rendering/drawing
@li For pure event dispatch (no console-side logic), it just forwards to
    ViewEventEmitters.
@li For UI-local things (like pushing a new window on success), it acts
    directly.
*/

} // namespace Cui

//----------------------------------------------------------------
