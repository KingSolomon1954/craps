//----------------------------------------------------------------
//
// File: EventHandler.h
//
//----------------------------------------------------------------

#pragma once

#include <controller/GameEvents.h>

namespace Ctrl {

class EventHandler
{
public:
    /// @name Lifecycle
    /// @{
    EventHandler();
   ~EventHandler() = default;
    /// @}

    /// @name Modifiers
    /// @{
    /// @}

    /// @name Observers
    /// @{
    /// @}

private:
    void onReqRollDice(const Ctrl::ReqRollDice& ev) const;
};

/*-----------------------------------------------------------*//**

@class EventHandler

@brief Processes GameEvents for lib-controller

Has these responsibilities:

@li Listens/subscribes to specific GameEvents.
@li Listens for certain events (requests) from the View then,
@li Invokes functions in model/craps, obtaining function return results.
@li Converts results into a GameEvent and publishes them for View to obtain
@li Also listens for certain events from the Model/Craps that
    lib-controller needs to handle itself, e.g. UslHouseLowOnFunds.
@li Takes various actions depending on the Craps event.
*/

} // namespace Ctrl

//----------------------------------------------------------------
