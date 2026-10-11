//----------------------------------------------------------------
//
// File: CarrierBet.h
//
//----------------------------------------------------------------

#pragma once

#include <craps/CrapsTypes.h>
#include <craps/EnumBetName.h>
#include <gen/MoneyUtils.h>

namespace Cui {

class CarrierBet
{
public:
    /// @name Lifecycle
    /// @{
   ~CarrierBet() = default;
    /// @}

    /// @name Modifiers
    /// @{
    void clear();
    void setPlayerId      (const Craps::PlayerId& id);
    void setBetType       (BetName betName);
    void setPivot         (unsigned pivot);
    void setContractAmount(const Gen::Money& amount);
    void setOddsAmount    (const Gen::Money& amount);
    /// @}

    /// @name Observers
    /// @{
    static CarrierBet& instance();
    Craps::PlayerId   getPlayerId()       const;
    BetName           getBetName ()       const;
    unsigned          getPivot   ()       const;
    Gen::Money        getContractAmount() const;
    Gen::Money        getOddsAmount()     const;
    /// @}

private:
    Craps::PlayerId playerId_;
    Craps::BetId    betId_;
    BetName         betName_;
    unsigned        pivot_;
    Gen::Money      contractAmount_;
    Gen::Money      oddsAmount_;

    CarrierBet() = default;
};

/*-----------------------------------------------------------*//**

@struct CarrierBet

@brief Static bet object filled in by a chain of menus

 A single static object shared by menu classes involved in betting.

Top level betting menus are responsible for clearing the CarrierBet upon
activation and then each child menu fills in their betting information.
At the very end of the chain the CarrierBet holds all information needed
to make a bet.

*/

} // namespace Cui

//----------------------------------------------------------------
