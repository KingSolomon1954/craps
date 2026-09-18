//----------------------------------------------------------------
//
// File: WorkOrder.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/bases/SurfaceBase.h>
#include <ncurses.h>

namespace Cui {

//----------------------------------------------------------------

enum class Type
{
    Draw,
    Event,
    Key,
    PopSurface,
    PushSurface,
    SetSurface
};

//----------------------------------------------------------------

struct WorkOrder
{
    Type     type;
    int      key          = ERR;
    SurfaceBase* pSurface = nullptr;
};

} // namespace Cui

//----------------------------------------------------------------
