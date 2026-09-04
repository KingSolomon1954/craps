//----------------------------------------------------------------
//
// File: CuiThread.cpp
//
//----------------------------------------------------------------

#include <cui/CuiThread.h>
#include <cui/SurfaceManager.h>

#include <ncurses.h>
#include <chrono>

using namespace Cui;

//----------------------------------------------------------------

CuiThread::CuiThread()
{
    thread_ = std::thread(&CuiThread::cuiThreadFunc, this);
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

    if (thread_.joinable())
        thread_.join();
}

//----------------------------------------------------------------

void
CuiThread::cuiThreadFunc()
{
    threadId_ = std::this_thread::get_id();    
    nodelay(stdscr, TRUE);

    while (true)
    {
        while (true)  // Read every key currently available.
        {
            const int ch = wgetch(stdscr);
            if (ch == ERR) break;
            if (shutdownRequested_) return;
            std::unique_lock<std::mutex> lock(mutex_);
            workQueue_.push_back(WorkOrder{Type::Key, ch});
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
        WorkOrder workOrder;

        {
            std::lock_guard<std::mutex> lock(mutex_);

            if (workQueue_.empty()) {
                return;
            }

            workOrder = std::move(workQueue_.front());
            workQueue_.pop_front();
        }

        processWorkOrder(workOrder);
    }
}

//----------------------------------------------------------------

void
CuiThread::processWorkOrder(const WorkOrder& workOrder)
{
    // TODO
    switch (workOrder.type)
    {
    case (Type::Key):
        SurfaceManager::instance().handleKey(workOrder.key);
        break;
        
    case (Type::Draw):
        SurfaceManager::instance().draw();
        break;
        
    case (Type::PopSurface):
        SurfaceManager::instance().popSurface();
        break;
    }
}

//----------------------------------------------------------------

bool
CuiThread::isCuiThread() const
{
    return std::this_thread::get_id() == threadId_;
}

//----------------------------------------------------------------
