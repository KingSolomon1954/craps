//----------------------------------------------------------------
//
// File: CuiSender.h
//
//----------------------------------------------------------------

#pragma once

namespace Cui {

class CuiSender
{
public:
    /// @name Lifecycle
    /// @{
    CuiSender();
   ~CuiSender() = default;
    static CuiSender& instance();
    /// @}

    /// @name Modifiers
    /// @{
    /// @}

    /// @name Observers
    /// @{
    /// @}
        
private:
};

/*-----------------------------------------------------------*//**

@class CuiSender

@brief Publishes CUI-originated requests to the application EventManager

Responsibilities of CuiSender:

@li Publishes events to Controller EventManager
@li OperationManager calls CuiSender as needed to form/send 
    events, e.g. reqMakeBet, reqRollDice, reqRemoveBet, ...
@li Converts operations into GameEvents.
*/

} // namespace Cui

//----------------------------------------------------------------
