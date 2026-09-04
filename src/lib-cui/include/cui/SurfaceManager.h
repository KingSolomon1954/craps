//----------------------------------------------------------------
//
// File: SurfaceManager.h
//
//----------------------------------------------------------------

#pragma once

#include <atomic>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace Cui {

class SurfaceBase;  // fwd

class SurfaceManager
{
public:
    /// @name Lifecycle
    /// @{
   ~SurfaceManager() = default;
    static SurfaceManager& instance();
    void prepareForShutdown();
    void registerForShutdown(SurfaceBase* pSurface);
    /// @}

    /// @name StackOps
    /// @{
    void setSurface (SurfaceBase* pSurface);  // clear stack, push this (replace)
    void pushSurface(SurfaceBase* pSurface);  // overlay (pauses previous top)
    void popSurface ();                       // remove top, resume new top if any
    void popSurfaces();                       // remove top until menu claim control
    bool handleKey(int ch);
    void draw();
    /// @}

    /// @name InputHandling
    /// @{
    /// @}

private:
    // Order doesn't matter
    std::vector<SurfaceBase*> stack_;    // non-owning stack
    std::mutex stackMx_;
    using SurfaceList = std::vector<SurfaceBase*>;
    SurfaceList surfaces_;
    
    SurfaceManager() = default;
    void shutdownNcursesResources();
    void draw(SurfaceBase* pSurface);
};

/*-----------------------------------------------------------*//**

@class SurfaceManager

@brief Manages surfaces and keyboard input thread.

Responsibilities of SurfaceView:

@li Implements the UI run loop
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
