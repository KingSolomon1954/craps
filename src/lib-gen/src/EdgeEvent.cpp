//---------------------------------------------------------------
//
// File: EdgeEvent.cpp
//
//---------------------------------------------------------------

#include <gen/EdgeEvent.h>

#include <cerrno>
#include <chrono>

using namespace Gen;

/*-----------------------------------------------------------*//**

Blocks the calling pthread(s) until this EdgeEvent object is
signaled. Only one pthread is selected for wakeup for each
signal. A pthread must already be waiting when the signal
occurs otherwise the event is missed. The underlying pthread
implementation determines which pthread to unblock and
usually pthread scheduling priorities determines this.

*/
void
EdgeEvent::wait()
{
    std::unique_lock<std::mutex> lock(mutex_);

    // A signal that occurred before this wait is intentionally discarded.
    signaled_ = false;

    condition_.wait(
        lock,
        [this]
        {
            return signaled_;
        });

    // Consume the signal.
    signaled_ = false;
}

/*-----------------------------------------------------------*//**

Blocks the calling pthread until this EdgeEvent object
is signaled or the specified time elapses. Same as
wait() with a timeout.

@param waitNanos
    Wait for waitNanos nanoseconds.
    A negative value, or zero, means wait forever.

@return
    zero if the EdgeEvent was signaled.
    ETIMEDOUT (in errno.h) if time expired.
*/
int
EdgeEvent::wait(std::int64_t waitNanos)
{
    std::unique_lock<std::mutex> lock(mutex_);

    // A signal that occurred before this wait is intentionally discarded.
    signaled_ = false;

    // Preserve the original behavior: non-positive values wait forever.
    if (waitNanos <= 0)
    {
        condition_.wait(
            lock,
            [this]
            {
                return signaled_;
            });

        signaled_ = false;
        return 0;
    }

    const auto timeout = std::chrono::steady_clock::now() +
                         std::chrono::nanoseconds(waitNanos);

    const bool signaled = condition_.wait_until(
        lock,
        timeout,
        [this]
        {
            return signaled_;
        });

    if (!signaled)
    {
        return ETIMEDOUT;
    }

    // Consume the signal.
    signaled_ = false;
    return 0;
}

/*-----------------------------------------------------------*//**

Unblocks a pthread that is waiting on this EdgeEvent object.
Note that multiple calls to signal() do not accumulate. That
is, each call to signal() arms the event, but if it is already
armed, then it just remains armed.
If signal() is called and no pthread is
currently waiting, the signal is missed.

*/
void
EdgeEvent::signal()
{
    {
        std::lock_guard<std::mutex> lock(mutex_);

        // Signals do not accumulate.
        signaled_ = true;
    }

    condition_.notify_one();
}

//-----------------------------------------------------------------
