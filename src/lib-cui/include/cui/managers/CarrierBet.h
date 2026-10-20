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
    static CarrierBet& instance();
    /// @}

    /// @name Modifiers
    /// @{
    void clear();
    void setPlayerId      (const Craps::PlayerId& id);
    void setBetName       (BetName betName);
    void setPivot         (unsigned pivot);
    void setAmount        (const Gen::Money& amount);
    void setContractAmount(const Gen::Money& amount);
    void setOddsAmount    (const Gen::Money& amount);
    void setDesireOdds    ();
    void setAutoFill      ();
    void setQuickBet      ();
    /// @}

    /// @name Observers
    /// @{
    Craps::PlayerId   getPlayerId      () const;
    BetName           getBetName       () const;
    unsigned          getPivot         () const;
    Gen::Money        getContractAmount() const;
    Gen::Money        getOddsAmount    () const;
    Gen::Money        getPreFillAmount () const;
    std::string       getTitleBetAmount() const;
    bool              isOddsBet        () const;
    /// @}

private:
    CarrierBet() = default;  // Private ctor

    Craps::PlayerId playerId_;
    Craps::BetId    betId_          = 0;
    BetName         betName_        = BetName::Invalid;
    unsigned        pivot_          = 0;
    Gen::Money      contractAmount_ = 0;
    Gen::Money      oddsAmount_     = 0;
    bool            isOddsBet_      = false;
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
