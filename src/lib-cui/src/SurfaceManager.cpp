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
        LOG_TRACE("shutdownNcursesResources() " + surface->surfaceName() + " calling releaseNcursesResources()");
        surface->releaseNcursesResources();
    }
    surfaces_.clear();
}

//----------------------------------------------------------------

void
SurfaceManager::prepareForShutdown()
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

void
SurfaceManager::setSurface(SurfaceBase* pSurface)
{
    SurfaceList oldSurfaces;

    {
        std::lock_guard<std::mutex> lock(stackMx_);

        oldSurfaces = stack_;
        stack_.clear();
        stack_.push_back(pSurface);
    }

    for (auto* s : oldSurfaces)
    {
        s->onDetach();
    }
    pSurface->onAttach(nullptr);

    draw(pSurface);
}

//----------------------------------------------------------------

void
SurfaceManager::pushSurface(SurfaceBase* pSurface)
{
    LOG_TRACE("SurfaceManager::pushSurface() pushing " + pSurface->surfaceName());
    SurfaceBase* pParent = nullptr;
    if (!stack_.empty())
    {
        pParent = stack_.back();
        pParent->onPause();
    }
    {
        std::lock_guard<std::mutex> lock(stackMx_);
        stack_.push_back(pSurface);
    }
    pSurface->onAttach(pParent);
    draw(pSurface);
}

//----------------------------------------------------------------

void
SurfaceManager::popSurface()
{
    SurfaceBase* pSurface = nullptr;
    SurfaceBase* pResumed = nullptr;

    {
        std::lock_guard<std::mutex> lock(stackMx_);

        if (stack_.size() <= 1) return;

        pSurface = stack_.back();
        stack_.pop_back();

        pResumed = stack_.back();
    }

    pSurface->onDetach();
    pResumed->onResume();

    draw(pResumed);
}

//----------------------------------------------------------------

void
SurfaceManager::popSurfaces()
{
    while (true)
    {
        SurfaceBase* pSurface = nullptr;

        {
            std::lock_guard<std::mutex> lock(stackMx_);
            if (stack_.size() <= 1) return;
            pSurface = stack_.back();
        }

        popSurface();

        SurfaceBase* pCurrent = nullptr;

        {
            std::lock_guard<std::mutex> lock(stackMx_);
            if (!stack_.empty()) pCurrent = stack_.back();
        }

        if (!pCurrent || !pCurrent->shouldSkip()) return;
    }
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
