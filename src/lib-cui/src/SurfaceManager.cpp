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
    LOG_TRACE("SurfaceManager::registerForShutdown() " + pSurface->surfaceName());
    SurfaceList oldSurfaces;
    surfaces_.push_back(pSurface);
}

//----------------------------------------------------------------

void
SurfaceManager::draw()
{
    draw(stack_.back());
}

//----------------------------------------------------------------

void
SurfaceManager::draw(SurfaceBase* pSurface)
{
    pSurface->draw();
    doupdate();  // Paint the physical screen
}

//----------------------------------------------------------------
//
// FullScreen surfaces use this. No window placement.
//
void
SurfaceManager::setSurface(SurfaceBase* pSurface)
{
    LOG_TRACE("SurfaceManager::setSurface() " + pSurface->surfaceName());
    SurfaceList oldSurfaces;

    oldSurfaces = stack_;
    stack_.clear();
    stack_.push_back(pSurface);

    for (auto* s : oldSurfaces)
    {
        s->onDetach();
    }
    pSurface->onAttach(nullptr);

    draw(pSurface);
}

//----------------------------------------------------------------
//
// Menu/Dialog/Overlya/etc. LocationManager placement
//
void
SurfaceManager::pushSurface(SurfaceBase* pSurface)
{
    LOG_TRACE("SurfaceManager::pushSurface() pushing " +
              pSurface->surfaceName());

    auto pParent = activeSurface();

    stack_.push_back(pSurface);

    pSurface->onAttach(pParent);
    assignLocation(pSurface, pParent);

    draw(pSurface);
}

//----------------------------------------------------------------

void
SurfaceManager::popSurface()
{
    SurfaceBase* pSurface = nullptr;
    SurfaceBase* pResumed = nullptr;

    if (stack_.size() <= 1) return;

    pSurface = stack_.back();
    stack_.pop_back();
    LOG_TRACE("SurfaceManager::popSurface() popping " + pSurface->surfaceName());

    pResumed = stack_.back();
    locationMgr_.release(pSurface->surfaceName());
    pSurface->onDetach();
    pResumed->onResume();

    draw(pResumed);
}

//----------------------------------------------------------------

void
SurfaceManager::popSurfaces()
{
    LOG_TRACE("SurfaceManager::popSurfaces()");
    while (true)
    {
        SurfaceBase* pSurface = nullptr;

        if (stack_.size() <= 1) return;
        pSurface = stack_.back();

        popSurface();

        SurfaceBase* pCurrent = nullptr;
        if (!stack_.empty()) pCurrent = stack_.back();
        if (!pCurrent || !pCurrent->shouldSkip()) return;
    }
}

//----------------------------------------------------------------

void
SurfaceManager::assignLocation(SurfaceBase* pSurface,
                               SurfaceBase* pParent)
{
    assert(pParent); assert(pSurface);
    
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
        LOG_TRACE("SurfaceManager::handleKey() calling handlekey(" + std::to_string(ch) + ")");
        return stack_.back()->handleKey(ch);
    }
    return false;
}

//----------------------------------------------------------------

SurfaceBase*
SurfaceManager::activeSurface() const
{
    if (stack_.empty()) return nullptr;
    return stack_.back();
}

//----------------------------------------------------------------

bool
SurfaceManager::isActiveSurface(SurfaceBase* pSurface) const
{
    return (activeSurface() == pSurface);
}

//----------------------------------------------------------------
