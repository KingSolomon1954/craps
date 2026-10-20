//----------------------------------------------------------------
//
// File: CuiSender.h
//
//----------------------------------------------------------------

#pragma once

#include <craps/EnumBetName.h>
#include <gen/MoneyUtils.h>

namespace Cui {

class CuiSender
{
public:
    /// @name Lifecycle
    /// @{
    CuiSender() = default;
   ~CuiSender() = default;
    static CuiSender& instance();
    /// @}

    /// @name Modifiers
    /// @{
    void reqRollDice();
    void reqAutoFill(BetName betName,
                     unsigned pivot,
                     bool isOddsBet,
                     Gen::Money amount);
    void reqQuickBet(BetName betName,
                     unsigned pivot,
                     bool isOddsBet,
                     Gen::Money amount);
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
