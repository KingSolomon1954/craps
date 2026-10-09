//----------------------------------------------------------------
//
// File: SurfaceManager.cpp
//
//----------------------------------------------------------------

#include <cui/SurfaceManager.h>
#include <cui/bases/SurfaceBase.h>
#include <gen/Logger.h>
#include <cassert>

using namespace Cui;

//----------------------------------------------------------------

SurfaceManager&
SurfaceManager::instance()
{
    static SurfaceManager mgr;
    return mgr;
}

//----------------------------------------------------------------

void
SurfaceManager::shutdownNcursesResources()
{
    for (auto* surface : surfaces_)
    {
        LOG_TRACE("SurfaceManager::shutdownNcursesResources() calling " +
                  surface->surfaceName() + "->releaseNcursesResources()");
        surface->releaseNcursesResources();
    }

    surfaces_.clear();
}

//----------------------------------------------------------------

void
SurfaceManager::shutdown()
{
    shutdownNcursesResources();
}

//----------------------------------------------------------------
//
// Keep a collection of unique surface pointers.
//
// Used when shutting down in order to issue delwin() before ncurses
// disappears. Can't control static order singleton class destructors.
//
void
SurfaceManager::registerForShutdown(SurfaceBase* pSurface)
{
    LOG_TRACE("SurfaceManager::registerForShutdown() " +
              pSurface->surfaceName());

    surfaces_.push_back(pSurface);
}

//----------------------------------------------------------------
//
// Redraw the complete current screen composition.
//
// stack_ is ordered from the root surface through each active
// overlay/menu to the currently active surface.  Each surface is
// drawn in that order so that higher surfaces appear on top of
// lower surfaces.
//
void
SurfaceManager::draw()
{
    if (stack_.empty()) return;

    for (auto* surface : stack_)
    {
        surface->draw();
    }

    doupdate();
}

//----------------------------------------------------------------

void
SurfaceManager::draw(SurfaceBase* pSurface)
{
    assert(pSurface);

    pSurface->draw();
    doupdate();
}

//----------------------------------------------------------------
//
// FullScreen surfaces use this. No window placement.
//
void
SurfaceManager::setSurface(SurfaceBase* pSurface)
{
    LOG_TRACE("SurfaceManager::setSurface() " +
              pSurface->surfaceName());

    SurfaceList oldSurfaces = stack_;
    stack_.clear();
    stack_.push_back(pSurface);

    for (auto* s : oldSurfaces)
    {
        s->onDetach();
    }

    pSurface->onAttach(nullptr);

    draw();
}

//----------------------------------------------------------------
//
// Menu/Dialog/Overlay/etc. LocationManager placement.
//
void
SurfaceManager::pushSurface(SurfaceBase* pSurface)
{
    LOG_TRACE("SurfaceManager::pushSurface() pushing " +
              pSurface->surfaceName());

    auto pParent = activeSurface();

    stack_.push_back(pSurface);

    pParent->onPause();

    pSurface->onAttach(pParent);
    assignLocation(pSurface, pParent);

    draw(pSurface);
}

//----------------------------------------------------------------

void
SurfaceManager::popSurface()
{
    if (stack_.size() <= 1)
        return;

    SurfaceBase* pSurface = stack_.back();
    stack_.pop_back();

    LOG_TRACE("SurfaceManager::popSurface() popping " +
              pSurface->surfaceName());

    SurfaceBase* pResumed = stack_.back();

    locationMgr_.release(pSurface->surfaceName());

    pSurface->onDetach();
    pResumed->onResume();

    // The popped surface's ncurses WINDOW may have left its
    // contents on the physical screen.  Redraw the complete
    // remaining surface chain to reconstruct the screen.
    draw();
}

//----------------------------------------------------------------

void
SurfaceManager::popSurfaces()
{
    LOG_TRACE("SurfaceManager::popSurfaces()");

    while (stack_.size() > 1)
    {
        SurfaceBase* pSurface = stack_.back();
        stack_.pop_back();

        LOG_TRACE("SurfaceManager::popSurfaces() popping " +
                  pSurface->surfaceName());

        locationMgr_.release(pSurface->surfaceName());
        pSurface->onDetach();

        SurfaceBase* pCurrent = stack_.back();
        pCurrent->onResume();

        if (!pCurrent->shouldSkip())
        {
            LOG_TRACE("popSurfaces(): !shouldSkip: break");
            break;
        }
    }

    draw();
}

#if 0

void
SurfaceManager::popSurfaces()
{
    LOG_TRACE("SurfaceManager::popSurfaces()");

    while (stack_.size() > 1)
    {
        popSurface();

        SurfaceBase* pCurrent = stack_.back();

        if (!pCurrent->shouldSkip())
            return;
    }
}

#endif

//----------------------------------------------------------------

void
SurfaceManager::assignLocation(SurfaceBase* pSurface,
                               SurfaceBase* pParent)
{
    assert(pParent);
    assert(pSurface);

    auto position = locationMgr_.findPosition(
        pSurface->getLocationRequest(),
        pParent->surfaceName(),
        pSurface->surfaceName());

    // findPosition() itself throws if position is null

    pSurface->setLocation(*position);
}

//----------------------------------------------------------------

bool
SurfaceManager::handleKey(int ch)
{
    if (!stack_.empty())
    {
        LOG_TRACE("SurfaceManager::handleKey() calling handlekey(" +
                  std::to_string(ch) + ")");

        return stack_.back()->handleKey(ch);
    }

    return false;
}

//----------------------------------------------------------------

SurfaceBase*
SurfaceManager::activeSurface() const
{
    if (stack_.empty())
        return nullptr;

    return stack_.back();
}

//----------------------------------------------------------------

bool
SurfaceManager::isActiveSurface(SurfaceBase* pSurface) const
{
    return (activeSurface() == pSurface);
}

//----------------------------------------------------------------

