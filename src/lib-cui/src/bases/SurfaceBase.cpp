//----------------------------------------------------------------
//
// File: SurfaceBase.cpp
//
//----------------------------------------------------------------

#include <cui/bases/SurfaceBase.h>
#include <cui/SurfaceManager.h>
#include <cassert>
#include <stdexcept>

using namespace Cui;

//----------------------------------------------------------------

SurfaceBase::SurfaceBase(std::string surfaceName)
    : surfaceName_(std::move(surfaceName))
{
    SurfaceManager::instance().registerForShutdown(this);
}

//----------------------------------------------------------------
//
// Base class remembers parent surface, and inits operationResult_.
//
// What might derived class do in onAttach()
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
void
SurfaceBase::onAttach(SurfaceBase* pParent)
{
    pParentSurface_  = pParent;
    operationResult_ = OperationResult::Unset;
}

//----------------------------------------------------------------
//
// Basically undo what was done during onAttach()
//
void
SurfaceBase::onDetach()
{
    pParentSurface_ = nullptr;
}

//----------------------------------------------------------------

void
SurfaceBase::onPause()
{
    // Empty
}

//----------------------------------------------------------------

void
SurfaceBase::onResume()
{
    // Empty
}

//----------------------------------------------------------------
//
// Our immediate child surface is informing us of success/failure/error
// of the operation. The state is remembered here in the base class. We
// also propagate the state up the call chain of surfaces to our
// parent. This allows a surface to decide whether they retain control
// later upon unwinding the stack or should be skipped. See
// SurfaceManager::popSurfaces().
// 
void
SurfaceBase::setOperationResult(OperationResult r)
{
    operationResult_ = r;
    if (pParentSurface_) pParentSurface_->setOperationResult(r);
}

//----------------------------------------------------------------
//
// Return true if this surface should be skipped when unwinding
// the stack. Some surfaces in the call chain should not regain
// control depending on the success or failure of the overall
// operation. SurfaceManager calls this on each surface when
// popping surfaces.
//
// The default in the base class here is not to skip, otherwise
// a concrete class should set the shouldSkip_ variable as needed.
//
bool
SurfaceBase::shouldSkip() const
{
    return shouldSkip_;
}

//----------------------------------------------------------------

void
SurfaceBase::newWindow(int rows, int cols,
                       int row,  int col)
{
    pWin_ = newwin(rows, cols, row, col);
    if (pWin_ == nullptr)
    {
        assert(pWin_ != nullptr);
        std::string s = "SurfaceBase::newWindow(): Unable to "
                        "create ncurses WINDOW for: " + surfaceName_;
        throw std::runtime_error(s);
    }
}

//----------------------------------------------------------------

void
SurfaceBase::newWindow(WindowRect rect)
{
    newWindow(rect.rows, rect.cols, rect.row, rect.col);
}

//----------------------------------------------------------------

void
SurfaceBase::releaseNcursesResources()
{
    if (pWin_ != nullptr)
    {
        delwin(pWin_);
        pWin_ = nullptr;
    }
}

//----------------------------------------------------------------

void
SurfaceBase::setLocation(WindowPosition position)
{
    // Empty
}

//----------------------------------------------------------------

LocationRequest
SurfaceBase::getLocationRequest() const
{
    return LocationRequest{
        .size = {0, 0},
        .kind = LocationKind::Menu,
        .direction = Direction::Right
    };
}

//----------------------------------------------------------------

const std::string&
SurfaceBase::surfaceName() const
{
    return surfaceName_;
}

//----------------------------------------------------------------
