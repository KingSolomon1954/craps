//---------------------------------------------------------------
//
// File: SignalHandler.cpp
//
//---------------------------------------------------------------

#include <iostream>             // For cout
#include <csignal>              // For signals
#include <gen/SignalHandler.h>  // For our interface

using namespace Gen;

/*-----------------------------------------------------------*//**

Constructor.

Upon construction sets up the initial global signal mask. For this
reason this object should be constructed early, before any threads are
created, so that new threads will inherit the proper per-thread signal
mask.
*/
SignalHandler::SignalHandler()
{
    sigset_t sigsToBlock;
    sigfillset(&sigsToBlock);  // block everything
    // On Linux 2.4, SIGABRT & SIGSEGV need to be let through so children die
    // on asserts and segmentation faults.  See terminate()
    sigdelset(&sigsToBlock, SIGABRT);  // let this through
    sigdelset(&sigsToBlock, SIGSEGV);  // let this through
    sigprocmask(SIG_BLOCK, &sigsToBlock, nullptr);
    
//  pthread_sigmask(SIG_BLOCK, &sigsToBlock, 0);
}

//---------------------------------------------------------------
/**
 * Destructor - empty.
 */
SignalHandler::~SignalHandler()
{
    // Emtpy
}

/*-----------------------------------------------------------*//**

Blocks the calling thread and explicitly waits for a
hard coded set of signals. When a signal of interest
arrives it takes action, which usually means the loop
exits. We're only looking for signals that should cause
the program to exit and ignores all others. See the
code for the set of signals.
*/
void SignalHandler::waitForTerminate()
{
    tid_ = pthread_self();
    sigset_t sigsToCatch;
    sigemptyset(&sigsToCatch);

    sigaddset(&sigsToCatch, SIGHUP);
    sigaddset(&sigsToCatch, SIGINT);
    sigaddset(&sigsToCatch, SIGQUIT);
    sigaddset(&sigsToCatch, SIGABRT);
    sigaddset(&sigsToCatch, SIGTERM);

    // When running a debugger, we don't want this signal 
    // in the set. TODO figure a way to toggle with debug builds.
    sigaddset(&sigsToCatch, SIGINT);

    int caught = 0;
    sigwait(&sigsToCatch, &caught);
}

/*-----------------------------------------------------------*//**

Causes the signal handler loop to release and exit its infinite
loop. This implementation sends a signal to the signal catching 
thread to cause it to wake it up and exit.

*/
void SignalHandler::terminate()
{
    // Send SIGTERM signal to wakeup main thread.
    int rv = pthread_kill(tid_, SIGTERM);

    // It's possible that the main thread is gone, but
    // the children live on.
    if (0 != rv)
    {
        std::cout << "Terminate Handler: Forced hard exit." << std::endl;
        exit(1);
    }
}

//---------------------------------------------------------------
