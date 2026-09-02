//----------------------------------------------------------------
//
// File: SurfaceManager.cpp
//
//----------------------------------------------------------------

#include <cui/SurfaceManager.h>
#include <cui/bases/SurfaceBase.h>
#include <gen/Logger.h>
#include <cassert>
#include <chrono>

using namespace Cui;
using namespace std::chrono_literals;

//----------------------------------------------------------------

SurfaceManager&
SurfaceManager::instance()
{
    static SurfaceManager mgr;
    return mgr;
}

//----------------------------------------------------------------

void
SurfaceManager::run()
{
    inputThread_ = std::thread(&SurfaceManager::inputThreadFunc, this);
}

//----------------------------------------------------------------

void
SurfaceManager::shutdownInputThread()
{
    running_ = false;
    if (inputThread_.joinable())
    {
        inputThread_.join();
    }

    {
        std::lock_guard<std::mutex> lk(stackMx_);
        for (auto* s : stack_) s->onDetach();
        stack_.clear();
    }
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
    shutdownInputThread();
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

//    if (std::find(surfaces_.begin(), surfaces_.end(), pSurface)
//        == surfaces_.end())
//    {
//        surfaces_.push_back(pSurface);
//    }
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
    std::lock_guard<std::mutex> lock(stackMx_);
    for (auto* s : stack_) s->onDetach();
    stack_.clear();
    stack_.push_back(pSurface);
    draw(pSurface);
    pSurface->onAttach(nullptr);  // No parent to attach to.
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
    {
        std::lock_guard<std::mutex> lock(stackMx_);
        if (stack_.empty()) return;

        auto* pSurface = stack_.back();
        stack_.pop_back();
        pSurface->onDetach();

        if (!stack_.empty())
        {
            stack_.back()->onResume();
        }
    }
    draw(stack_.back());
}

//----------------------------------------------------------------

void
SurfaceManager::popSurfaces()
{
    while (stack_.size() > 1)
    {
        popSurface();
        auto* pSurface = stack_.back();
        if (!pSurface->shouldSkip()) break;
    }
}

//----------------------------------------------------------------

void
SurfaceManager::inputThreadFunc()
{
    LOG_TRACE("SurfaceManager::inputThreadFunc() started");
    nodelay(stdscr, TRUE);
    while (running_)
    {
        LOG_TRACE("SurfaceManager::inputThreadFunc() before wgetch()");
        int ch = wgetch(stdscr);
        LOG_TRACE("SurfaceManager::inputThreadFunc() after wgetch(" + std::to_string(ch) + ")");
        if (ch == ERR)
        {
            LOG_TRACE("SurfaceManager::inputThreadFunc() sleeping");
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            continue;
        }

        std::lock_guard<std::mutex> lk(stackMx_);
        if (!stack_.empty())
        {
            LOG_TRACE("SurfaceManager::inputThreadFunc() calling handlekey(" + std::to_string(ch) + ")");
            stack_.back()->handleKey(ch);
        }
    }
}

//----------------------------------------------------------------
