//----------------------------------------------------------------
//
// File: CuiSender.h
//
//----------------------------------------------------------------

#pragma once

#include <craps/EnumBetName.h>
#include <gen/MoneyUtils.h>
#include <craps/CrapsTypes.h>
#include <controller/GameEvents.h>

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
    void reqTerminate();
    Ctrl::CorrelationId reqSwitchPlayer(const Craps::PlayerId& playerId);
    Ctrl::CorrelationId reqManagePlayer(Ctrl::PlayerAction action,
                                        const Craps::PlayerId& playerId,
                                        const std::string& name,
                                        const std::string& description = "");
    void reqRollDice();
    void reqSetAnimationSpeed(Ctrl::AnimationSpeed speed);
    void reqSetRollHistoryFormat(Ctrl::RollHistoryFormat format);
    Ctrl::CorrelationId reqMakeBet(const Craps::PlayerId& playerId,
                                   BetName betName,
                                   Gen::Money contractAmount,
                                   unsigned pivot);
    Ctrl::CorrelationId reqRemoveBet(const Craps::PlayerId& playerId,
                                     Craps::BetId betId);
    Ctrl::CorrelationId reqOddsBet(const Craps::PlayerId& playerId,
                                   Craps::BetId betId,
                                   Gen::Money oddsAmount);
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
