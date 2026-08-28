//----------------------------------------------------------------
//
// File: ConsoleManager.h
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

class Surface;  // fwd

class ConsoleManager : public Ctrl::ViewInterface
{
public:
    /// @name Lifecycle
    /// @{
    ConsoleManager();
   ~ConsoleManager();
    void init()               override;  // from ViewInterface
    void prepareForShutdown() override;  // from ViewInterface
    static ConsoleManager& instance();
    /// @}

    /// @name StackOps
    /// @{
    void setSurface (Surface* pSurface);  // clear stack, push this (replace)
    void pushSurface(Surface* pSurface);  // overlay (pauses previous top)
    void popSurface ();                   // remove top, resume new top if any
    void popSurfaces();                   // remove top until menu claim control
    void draw(Surface* pSurface);
    /// @}


    /// @name InputHandling
    /// @{
    void inputThreadFunc(); // Input thread -> forward to top
    /// @}

    /// @name ViewHandlers
    /// @{
    void showSuccess(
        Ctrl::EventType    orgEventType,
        uint64_t           correlationId) override;
    void showErrorDialog(
        Ctrl::EventType    orgEventType,
        uint64_t           correlationId,
        const std::string& diag) override;
    void showMakeBetSuccess(
        Craps::BetId betId,
        uint64_t     correlationId) override;
    void showMakeOddsBetSuccess(
        Craps::BetId betId,
        uint64_t     correlationId) override;
    void showMakeBetAutoSuccess(
        Craps::BetId betId,
        uint64_t     correlationId) override;
    void showMakeBetAutoError(
        Ctrl::EventType        orgEventType,
        uint64_t               correlationId,
        const Craps::PlayerId& playerId,
        const std::string&     diag) override;
    void showRollDiceCountDown(
        uint64_t               correlationId,
        int                    numSeconds) override;
    void showRollDiceAnimation(
        uint64_t               correlationId) override;
    void showProgramExit() override;
    /// @}

    bool useUnicodePips_ = false;

private:
    std::vector<Surface*> stack_;    // non-owning stack
    std::mutex stackMx_;
    std::atomic<bool> running_{true};
    std::thread inputThread_;
    
    bool utf8_enabled();
};

/*-----------------------------------------------------------*//**

@class ConsoleManager

@brief Console interaction with the user.

Responsibilities of ConsoleView:

@li UI Runtime
@li UI Initialization & Screen hierarchy
@li The main entry point for CUI ncurses
@li Implements the UI run loop
@li Implements the UI ViewInterface
@li Owns/orchestrates the active surface view stack and ncurses lifecycle
@li Initialize and shut down the ncurses environment safely
@li Manages view surfaces, pushing/popping, unaware of whether
    it's dealing with a screen, menu, or dialog box.
@li Asynchronous input thread to obtain keys
@li Each key forwarded to the active screen
@li Delegates to next or previous screen for rendering/drawing
@li Dispatches events (no console-side logic). Just forwards them to
    Controller::ViewCommands.
*/

} // namespace Cui

//----------------------------------------------------------------
