//---------------------------------------------------------------
//
// File: CarrierBet.cpp
//
//---------------------------------------------------------------

#include <cui/managers/CarrierBet.h>
#include <cui/core/CuiSender.h>
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
    isOddsBet_      = false;
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
    if (isOddsBet_)
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
    isOddsBet_ = true;
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
CarrierBet::isOddsBet() const
{
    return isOddsBet_;
}

//----------------------------------------------------------------

std::string
CarrierBet::getTitleBetAmount() const
{
    std::string oddsStr;   // empty
    std::string pivotStr;  // empty
    
    if (isOddsBet_)
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
    CuiSender::instance().reqAutoFill(
        betName_,
        pivot_,
        isOddsBet_,
        isOddsBet_ ? oddsAmount_ : contractAmount_);
}

//----------------------------------------------------------------

void
CarrierBet::setQuickBet()
{
    CuiSender::instance().reqQuickBet(
        betName_,
        pivot_,
        isOddsBet_,
        isOddsBet_ ? oddsAmount_ : contractAmount_);
}

//----------------------------------------------------------------
