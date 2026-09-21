//----------------------------------------------------------------
//
// File: CuiThread.cpp
//
//----------------------------------------------------------------

#include <cui/CuiThread.h>
#include <cui/SurfaceManager.h>
#include <cui/panels/WindowAnimation.h>
#include <cui/panels/WindowRollHistory.h>
#include <cui/panels/WindowTitleBar.h>
#include <gen/Logger.h>
#include <ncurses.h>
#include <cassert>
#include <chrono>

using namespace Cui;

//----------------------------------------------------------------

CuiThread::CuiThread()
{
    thread_ = std::thread(&CuiThread::cuiThreadFunc, this);
    threadStarted_.wait();  // Allow thread to start
}

//----------------------------------------------------------------

CuiThread&
CuiThread::instance()
{
    static CuiThread ct;
    return ct;
}

//----------------------------------------------------------------

CuiThread::~CuiThread()
{
    shutdown();
}

//----------------------------------------------------------------

bool
CuiThread::enqueueWork(WorkOrder workOrder)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (shutdownRequested_) return false;
        workQueue_.push_back(std::move(workOrder));
    }

    condition_.notify_one();
    return true;
}

//----------------------------------------------------------------

void
CuiThread::shutdown()
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        shutdownRequested_ = true;
    }

    condition_.notify_one();
    if (thread_.joinable()) thread_.join();
}

//----------------------------------------------------------------

bool
CuiThread::isCuiThread() const
{
    return std::this_thread::get_id() == threadId_;
}

//----------------------------------------------------------------

void
CuiThread::cuiThreadFunc()
{
    LOG_TRACE("CuiThread::cuiThreadFunc() running ");
    
    threadId_ = std::this_thread::get_id();
    threadStarted_.signal();
    
    nodelay(stdscr, TRUE);
    flushinp();  // Guarantee no keys yet. Buggy without this.
    
    while (true)
    {
        while (true)  // Read every key currently available.
        {
            const int ch = wgetch(stdscr);
            if (ch == ERR) break;
            if (shutdownRequested_) return;
            std::unique_lock<std::mutex> lock(mutex_);
            WorkOrderKey wo; wo.key = ch;            
            workQueue_.push_back(wo);
        }

        processWorkQueue();

        std::unique_lock<std::mutex> lock(mutex_);
        if (shutdownRequested_ && workQueue_.empty()) return;
        if (!workQueue_.empty()) continue;

        condition_.wait_for(
            lock,
            std::chrono::milliseconds(50),
            [this] {
                return shutdownRequested_ || !workQueue_.empty();
            });
    }
}

//----------------------------------------------------------------

void
CuiThread::processWorkQueue()
{
    while (true)
    {
        WorkOrder wo;

        {
            std::lock_guard<std::mutex> lock(mutex_);

            if (workQueue_.empty()) {
                return;
            }

            wo = std::move(workQueue_.front());
            workQueue_.pop_front();
        }

        processWorkOrder(wo);
    }
}

//----------------------------------------------------------------

void
CuiThread::processWorkOrder(const WorkOrder& wo)
{
    std::visit(
        [this](const auto& workOrder)
        {
            process(workOrder);
        },
        wo);
}

//----------------------------------------------------------------

void
CuiThread::process(const WorkOrderKey& wo)
{
    SurfaceManager::instance().handleKey(wo.key);    
}

//----------------------------------------------------------------

void
CuiThread::process(const WorkOrderSurface& wo)
{
    switch (wo.type)
    {
    case SurfaceType::Draw:
        if (wo.pSurface)
        {
            SurfaceManager::instance().draw(wo.pSurface);
        }
        else
        {
            SurfaceManager::instance().draw();
        }
        break;

    case SurfaceType::SetSurface:
        SurfaceManager::instance().setSurface(wo.pSurface);
        break;

    case SurfaceType::PopSurface:
        SurfaceManager::instance().popSurface();
        break;

    case SurfaceType::PushSurface:
        SurfaceManager::instance().pushSurface(wo.pSurface);
        break;

    default:
        assert(false);
    }
}

//----------------------------------------------------------------

void
CuiThread::process(const WorkOrderEvent& woe)
{
    std::visit(
        [this](const auto& event)
        {
            process(event);
        },
        woe.event);
}

//----------------------------------------------------------------

void
CuiThread::process(const Ctrl::UslDiceThrowStart& ev)
{
    LOG_TRACE("CuiThread::process(UslDiceThrowStart)");
    WindowAnimation::instance().onDiceThrowStart();
}

//----------------------------------------------------------------

void
CuiThread::process(const Ctrl::UslDiceNewValue& ev)
{
    LOG_TRACE("CuiThread::process(UslDiceNewValue)");
    WindowAnimation::instance().onDiceNewValue  (ev.d1, ev.d2, ev.rollCount);
    WindowRollHistory::instance().onDiceNewValue(ev.d1, ev.d2, ev.rollCount);
    WindowTitleBar::instance().onDiceNewValue(ev.d1, ev.d2, ev.rollCount);
}

//----------------------------------------------------------------

void
CuiThread::process(const Ctrl::UslBettingOpened& ev)
{
    LOG_TRACE("CuiThread::process(UslBettingOpened)");
}

//----------------------------------------------------------------

void
CuiThread::process(const Ctrl::UslPointEstablished& ev)
{
    LOG_TRACE("CuiThread::process(UslPointEstablished)");
    WindowTitleBar::instance().onPointEstablished(ev.point);
}

//----------------------------------------------------------------

void
CuiThread::process(const Ctrl::UslSevenOut& ev)
{
    LOG_TRACE("CuiThread::process(UslSevenOut)");
    WindowTitleBar::instance().onSevenOut();
}

//----------------------------------------------------------------

void
CuiThread::process(const Ctrl::UslPassLineWinner& ev)
{
    LOG_TRACE("CuiThread::process(UslPassLineWinner)");
    WindowTitleBar::instance().onPassLineWinner();
}

//----------------------------------------------------------------

