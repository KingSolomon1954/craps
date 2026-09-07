//---------------------------------------------------------------
//
// File: EdgeEvent.h
//
//---------------------------------------------------------------

#pragma once

#include <condition_variable>
#include <cstdint>
#include <mutex>

namespace Gen
{

class EdgeEvent
{
public:
    EdgeEvent() = default;
   ~EdgeEvent() = default;

    EdgeEvent(const EdgeEvent&) = delete;
    EdgeEvent& operator=(const EdgeEvent&) = delete;

    EdgeEvent(EdgeEvent&&) = delete;
    EdgeEvent& operator=(EdgeEvent&&) = delete;

    void wait();
    int  wait(std::int64_t waitNanos);
    void signal();     // Wake at most one waiting thread.

private:
    std::mutex mutex_;
    std::condition_variable condition_;
    bool signaled_ = false;
};

/*-----------------------------------------------------------*//**

@class EdgeEvent

@brief Synchronize threads

The EdgeEvent class is a mechanism for synchronizing
pthreads. Interested pthreads block waiting for a
indication that an event occured by calling wait().
Some other pthread notifies that the event
has occurred by calling signal().
A pthread must already be waiting for the signal
when it occurs otherwise the event is missed.

If several pthreads are waiting for the same EdgeEvent,
only one pthread is selected for wakeup. For example,
if 1 pthread is waiting, and signal() is called 5 times,
the one pthread will wake up and 4 signals will go unheard.
If 2 pthreads are waiting, and signal() is called 6 times,
the two pthreads will wake up and 4 signals will
go unheard.

@ingroup thread
*/

} // namespace Gen

//---------------------------------------------------------------
