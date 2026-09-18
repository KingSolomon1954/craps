//----------------------------------------------------------------
//
// File: WorkOrder.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/bases/SurfaceBase.h>
#include <controller/GameEvents.h>
#include <ncurses.h>

namespace Cui {

//----------------------------------------------------------------

enum class WorkOrderType
{
    Unset,
    Event,
    Key,
    Surface
};

enum class SurfaceType
{
    Unset,
    Draw,
    PopSurface,
    PushSurface,
    SetSurface
};

//----------------------------------------------------------------

struct WorkOrderKey
{
    int key = ERR;
};

//----------------------------------------------------------------
    
using GameEvent = std::variant<
    Ctrl::UslBettingOpened,
    Ctrl::UslDiceNewValue
    >;
    
struct WorkOrderEvent
{
    GameEvent event;
};

//----------------------------------------------------------------
    
struct WorkOrderSurface
{
    SurfaceType type = SurfaceType::Unset;
    SurfaceBase* pSurface = nullptr;
};
    
//----------------------------------------------------------------

using WorkOrder = std::variant<
    WorkOrderKey,
    WorkOrderEvent,
    WorkOrderSurface
    >;

//----------------------------------------------------------------

} // namespace Cui

//----------------------------------------------------------------
