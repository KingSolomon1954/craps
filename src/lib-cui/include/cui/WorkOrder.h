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
    Key,
    Draw,
    SetSurface,
    PopSurface,
    PushSurface
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
