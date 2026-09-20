//----------------------------------------------------------------
//
// File: WorkOrder.h
//
//----------------------------------------------------------------

#pragma once

#include <controller/GameEvents.h>
#include <ncurses.h>

namespace Cui {

class SurfaceBase;  // fwd

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
    Ctrl::UslDiceThrowStart,
    Ctrl::UslDiceNewValue
    >;
    
struct WorkOrderEvent
{
    GameEvent event;
};

//----------------------------------------------------------------
    
struct WorkOrderSurface
{
    SurfaceType type = SurfaceType::Unset;  // Must always be set
    SurfaceBase* pSurface = nullptr;
    // if SurfaceType == Draw then
    //     if pSurface ==  null then
    //        the active surface draw() is called
    //     else
    //        the supplied surface draw is called
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
