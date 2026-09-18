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
    void onReqRollDice(const Ctrl::ReqRollDice& ev) const;
    /// @}

private:
};

/*-----------------------------------------------------------*//**

@class EventHandler

@brief Processes GameEvents for lib-controller

Has these responsibilities:

@li Listens/subscribes to specific GameEvents.
@li Listens for certain events from the View (requests).
@li Invokes functions in model/craps, obtaining function return results.
@li Converts results into a GameEvent and publishes them.
@li Also listens for certain events from the Model/Craps that
    lib-controller needs to handle, e.g. UslHouseLowOnFunds.
@li Takes various actions depending on the Craps event.
*/

} // namespace Ctrl

//----------------------------------------------------------------
