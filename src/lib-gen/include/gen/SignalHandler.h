//---------------------------------------------------------------
//
// File: SignalHandler.h
// 
//---------------------------------------------------------------

#pragma once
#include <pthread.h>

namespace Gen {

/*-----------------------------------------------------------*//**

@class SignalHandler

@brief Signal Handling capability.

Meant for multi-threaded applications.  Sets up an intiial signal mask
so that spawned threads inherit the mask and block signals.  Provides a
waitForTerminate() function that unblocks selected signals and waits for
one to arrive.

@ingroup misc
*/
class SignalHandler
{
public:
    /// @name Lifecycle
    /// @{
   ~SignalHandler();
    SignalHandler();
    /// @}
    
    /// @name Modifiers
    /// @{
    void waitForTerminate();
    void terminate();
    /// @}

private:
    pthread_t tid_;
};

//---------------------------------------------------------------

}  // namespace Gen

