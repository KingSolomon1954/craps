//---------------------------------------------------------------
//
// File: CarrierBet.cpp
//
//---------------------------------------------------------------

#include <cui/managers/CarrierBet.h>
#include <controller/AutoFill.h>
#include <gen/Logger.h>
#include <cassert>

using namespace Cui;

//----------------------------------------------------------------

CarrierBet&
CarrierBet::instance()
{
    static CarrierBet cb;
    return cb;
}

//----------------------------------------------------------------

void
CarrierBet::clear()
{
    playerId_.clear();
    betName_        = BetName::Invalid;
    betId_          = 0;
    pivot_          = 0;
    oddsAmount_     = 0;
    contractAmount_ = 0;
    wantOdds_       = false;
}

//----------------------------------------------------------------

void
CarrierBet::setPlayerId(const Craps::PlayerId& id)
{
    playerId_ = id;
}

//----------------------------------------------------------------

void
CarrierBet::setBetName(BetName betName)
{
    betName_ = betName;
}

//----------------------------------------------------------------

void
CarrierBet::setPivot(unsigned pivot)
{
    pivot_ = pivot;
}

//----------------------------------------------------------------

void
CarrierBet::setAmount(const Gen::Money& amount)
{
    if (wantOdds_)
        oddsAmount_ = amount;
    else
        contractAmount_ = amount;
}

//----------------------------------------------------------------

void
CarrierBet::setContractAmount(const Gen::Money& amount)
{
    contractAmount_ = amount;
}

//----------------------------------------------------------------

void
CarrierBet::setOddsAmount(const Gen::Money& amount)
{
    oddsAmount_ = amount;
}

//----------------------------------------------------------------

void
CarrierBet::setDesireOdds()
{
    wantOdds_ = true;
}

//----------------------------------------------------------------

Craps::PlayerId
CarrierBet::getPlayerId() const
{
    return playerId_;
}

//----------------------------------------------------------------

BetName
CarrierBet::getBetName() const
{
    return betName_;
}

//----------------------------------------------------------------

unsigned
CarrierBet::getPivot() const
{
    return pivot_;
}

//----------------------------------------------------------------

Gen::Money
CarrierBet::getContractAmount() const
{
    return contractAmount_;
}

//----------------------------------------------------------------

Gen::Money
CarrierBet::getOddsAmount() const
{
    return oddsAmount_;
}

//----------------------------------------------------------------

Gen::Money
CarrierBet::getPreFillAmount() const
{
    return 0;
}

//----------------------------------------------------------------

bool
CarrierBet::wantOdds() const
{
    return wantOdds_;
}

//----------------------------------------------------------------

std::string
CarrierBet::getTitleBetAmount() const
{
    std::string oddsStr;   // empty
    std::string pivotStr;  // empty
    
    if (wantOdds_)
    {
        assert(pivot_ != 0);
        oddsStr = " Odds ";
    }

    if (pivot_ != 0)
    {
        pivotStr = " on the " + std::to_string(pivot_);    
    }
    
    return EnumBetName::toString(betName_) +
           oddsStr + " Bet" + pivotStr;
}

//----------------------------------------------------------------

void
CarrierBet::setAutoFill()
{
    Ctrl::AutoFill::AutoFillEntry afe
    {
        betName_,
        pivot_,
        wantOdds_,
        wantOdds_ ? oddsAmount_ : contractAmount_
    };
    Ctrl::AutoFill::instance().setAutoFill(afe);
}

//----------------------------------------------------------------


#if 0

// TODO responsible for building titles for various menus
// menus and dialogs will query CarrierBet for a title.

//----------------------------------------------------------------

std::string
MenuPivot::amtTitle(int pivot)
{
    return EnumBetName::toString(betName_) +
           " Bet on the " + std::to_string(pivot);
}

#endif
