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
    using SurfaceList = std::vector<SurfaceBase*>;
    SurfaceList surfaces_;
    LocationManager locationMgr_;
    
    SurfaceManager() = default;
    void shutdownNcursesResources();
    void draw(SurfaceBase* pSurface);
    SurfaceBase* activeSurface() const;
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
@li Draws the active surface
@li Manages ncurses resource shutdown

*/

} // namespace Cui

//----------------------------------------------------------------
