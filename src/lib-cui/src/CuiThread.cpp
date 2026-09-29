//----------------------------------------------------------------
//
// File: CuiThread.cpp
//
//----------------------------------------------------------------

#include <cui/CuiThread.h>
#include <ncurses.h>
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

        dispatcher_.dispatch(wo);
    }
}

//----------------------------------------------------------------
