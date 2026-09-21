//----------------------------------------------------------------
//
// File: CuiReceiver.h
//
//----------------------------------------------------------------

#pragma once

#include <controller/GameEvents.h>

namespace Cui {

class CuiReceiver
{
public:
    /// @name Lifecycle
    /// @{
    CuiReceiver();
   ~CuiReceiver() = default;
    static CuiReceiver& instance();
    /// @}

    /// @name Modifiers
    /// @{
    /// @}

    /// @name Observers
    /// @{
    /// @}

private:
    void onDiceNewValue    (const Ctrl::UslDiceNewValue& ev)     const;
    void onDiceThrowStart  (const Ctrl::UslDiceThrowStart& ev)   const;
    void onPointEstablished(const Ctrl::UslPointEstablished& ev) const;
    void onSevenOut        (const Ctrl::UslSevenOut& ev)         const;
    void onPassLineWinner  (const Ctrl::UslPassLineWinner& ev)   const;
};

/*-----------------------------------------------------------*//**

@class CuiReceiver

@brief Receives application GameEvents

Receives application GameEvents and converts them
into work orders for the CuiThread.

Responsibilities of CuiReceiver:

@li Event handler for GameEvents the CUI is interested in
@li Is the interface between the Controller's view vocabulary and the
    CUI's surface vocabulary.
@li Subscribes to Controller::EventManager
@li Converts events into work orders and enqueues the work order to
    the CuiThread for processing.
*/

} // namespace Cui

//----------------------------------------------------------------
