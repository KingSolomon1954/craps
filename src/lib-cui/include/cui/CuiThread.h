//----------------------------------------------------------------
//
// File: CuiThread.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/WorkOrder.h>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <thread>

//----------------------------------------------------------------

namespace Cui {

class CuiThread
{
public:
    CuiThread();
   ~CuiThread();
    static CuiThread& instance();

    CuiThread(const CuiThread&)            = delete;
    CuiThread& operator=(const CuiThread&) = delete;
    CuiThread(CuiThread&&)                 = delete;
    CuiThread& operator=(CuiThread&&)      = delete;

    // Returns false if shutdown is underway.
    bool enqueueWork(WorkOrder workOrder);
    void shutdown();
    bool isCuiThread() const;
    
private:
    void cuiThreadFunc();
    void processWorkQueue();
    void processWorkOrder(const WorkOrder& workOrder);

private:
    bool shutdownRequested_ = false;
    std::deque<WorkOrder> workQueue_;
    mutable std::mutex mutex_;
    std::condition_variable condition_;
    std::thread thread_;
    std::thread::id threadId_;
};

/*-----------------------------------------------------------*//**

@class CuiThread

@brief CuiThread processes CUI work orders and also obtains keypresses
        and manipulates surfaces 

CuiThread is the only class that is allowed to call into ncurses,
including reading key presses.

Responsibilities of CuiThread:

@li Implements a thread loop to enqueue keys and process work orders
@li Works closely with SurfaceManager
*/

} // namespace Cui

//----------------------------------------------------------------
