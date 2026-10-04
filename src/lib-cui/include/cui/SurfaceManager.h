//----------------------------------------------------------------
//
// File: SurfaceManager.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/LocationManager.h>
#include <string>
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
    void shutdown();
    void registerForShutdown(SurfaceBase* pSurface);
    /// @}

    /// @name StackOps
    /// @{
    void setSurface (SurfaceBase* pSurface);  // clear stack, push this (replace)
    void pushSurface(SurfaceBase* pSurface);  // overlay (pauses previous top)
    void popSurface ();                       // remove top, resume new top
    void popSurfaces();                       // remove top until menu claims control
    bool handleKey(int ch);
    SurfaceBase* activeSurface() const;
    bool isActiveSurface(SurfaceBase* pSurface) const;
    /// @}

    /// @name Draw
    /// @{
    void draw();  // Redraw complete current surface composition
    void draw(SurfaceBase* pSurface);  // Redraw only the specified surface
    /// @}

private:
    // stack_ contains the surfaces that currently make up the screen,
    // in drawing order from root to active surface.
    std::vector<SurfaceBase*> stack_;

    // Collection of unique surface pointers used for ncurses shutdown.
    using SurfaceList = std::vector<SurfaceBase*>;
    SurfaceList surfaces_;

    LocationManager locationMgr_;

private:
    SurfaceManager() = default;

    void shutdownNcursesResources();
    void assignLocation(SurfaceBase* pSurface, SurfaceBase* pParent);
};

/*-----------------------------------------------------------*//**

@class SurfaceManager

@brief Manages the active CUI surface stack and surface lifecycle.

@li Maintains the active surface stack
@li Manages surface attach, detach, pause, and resume operations
@li Coordinates surface placement through LocationManager
@li Creates and releases ncurses windows for surfaces
@li Dispatches keyboard input to the active surface
@li Redraws the complete current surface composition
@li Manages ncurses resource shutdown

*/

} // namespace Cui

//----------------------------------------------------------------


