//----------------------------------------------------------------
//
// File: SurfaceBase.h
//
//----------------------------------------------------------------

#pragma once

#include <ncurses.h>
#include <cassert>
#include <stdexcept>
#include <string>

namespace Cui
{

class SurfaceBase
{
public:
    enum class OperationResult
    {
        Unset,
        Cancel,
        Error,
        Success
    };
    
    virtual ~SurfaceBase() = default;

    virtual void draw() = 0;
    virtual void handleKey(int ch) = 0;
    
    virtual void onPause()  {};
    virtual void onResume() {};

    //----------------------------------------------------------------
    //
    // What might be done in onAttach()
    //
    // * Start a timer/animation associated with the surface.
    // * Subscribe to events that should only be received while
    //   the surface is active.
    // * Reset transient interaction state.
    // * Establish the initial input mode.
    // * Set the NavBar/context for the surface, if you decide to do
    //   that there.
    // * Trigger an initial data acquisition if appropriate.
    //    
    virtual void onAttach(SurfaceBase* pParent)
    {
        pParentSurface_  = pParent;
        operationResult_ = OperationResult::Unset;
    }
    
    //----------------------------------------------------------------
    //
    // Basically undo what was done during onAttach()
    //
    virtual void onDetach()
    {
        pParentSurface_ = nullptr;
    }

    //----------------------------------------------------------------
    //
    // Our immediate child surface is informing us of
    // success/failure/error of the operation. The state is
    // rememberd here in the base class. We also propagate the state
    // up the call chain of menus to our parent. This allows menus
    // to decide whether they retain control later upon unwinding the
    // stack or should be skipped. See ConsoleManager::popSurfaces().
    // 
    virtual void setOperationResult(OperationResult r)
    {
        operationResult_ = r;
        if (pParentSurface_) pParentSurface_->setOperationResult(r);
    }

    //----------------------------------------------------------------
    //
    // Return true if this surface should be skipped when unwinding
    // the stack. Some surfaces in the call chain should not regain
    // control depending on the success or failure of the overall
    // operation. ConsolerManager calls this on each surface when
    // popping surfaces.
    //
    // The default in the base class here is not to skip, otherwise
    // a concrete class should set the shouldSkip_ variable as needed.
    //
    virtual bool shouldSkip() const
    {
        return shouldSkip_;
    }
    
    virtual void releaseNcursesResources()
    {
        if (pWin_ != nullptr)
        {
            delwin(pWin_);
            pWin_ = nullptr;
        }
    }

    virtual void newWindow(int nlines, int ncols,
                           int topRow, int leftCol,
                           const std::string& surfaceName)
    {
        pWin_ = newwin(nlines, ncols, topRow, leftCol);
        if (pWin_ == nullptr)
        {
            assert(pWin_ != nullptr);
            std::string s = "SurfaceBase::newWindow(): "
                            "Unable to create ncurses WINDOW for: ";
            throw std::runtime_error(s + surfaceName);
        }
    }

protected:
    WINDOW*      pWin_               = nullptr;
    SurfaceBase* pParentSurface_     = nullptr;
    OperationResult operationResult_ = OperationResult::Unset;
    bool shouldSkip_                 = false;
};

/*-----------------------------------------------------------*//**

@class SurfaceBase

@brief Common abstraction for anything occupying a visible/input surface

@li Provides a generic UI surface contract
@li Allow ConsoleManager to work with just surface abstractions
@li Has no knowledge of ScreenBase, MenuBase, or DialogBase

*/

} // namespace Cui

//----------------------------------------------------------------
