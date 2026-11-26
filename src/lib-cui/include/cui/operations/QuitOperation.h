//----------------------------------------------------------------
//
// File: QuitOperation.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/operations/Operation.h>

namespace Cui {

/// Ask the person to confirm, then tell the Controller to terminate.
class QuitOperation : public Operation
{
public:
    void start() override;
    void onConfirm(SurfaceBase::OperationResult result) override;
};

} // namespace Cui

//----------------------------------------------------------------
