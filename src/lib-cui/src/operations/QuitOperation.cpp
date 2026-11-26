//----------------------------------------------------------------
//
// File: QuitOperation.cpp
//
//----------------------------------------------------------------

#include <cui/operations/QuitOperation.h>
#include <cui/core/CuiSender.h>

using namespace Cui;

//----------------------------------------------------------------

void
QuitOperation::start()
{
    askConfirmation("Confirm Quit RoyalCraps game");
}

//----------------------------------------------------------------

void
QuitOperation::onConfirm(SurfaceBase::OperationResult result)
{
    if (result == SurfaceBase::OperationResult::Yes)
    {
        CuiSender::instance().reqTerminate();
    }
    finish();
}

//----------------------------------------------------------------
