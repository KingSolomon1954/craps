//---------------------------------------------------------------
//
// File: CarrierBet.cpp
//
//---------------------------------------------------------------

#include <cui/CarrierBet.h>

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
}

//----------------------------------------------------------------

void
CarrierBet::setPlayerId(const Craps::PlayerId& id)
{
    playerId_ = id;
}

//----------------------------------------------------------------

void
CarrierBet::setBetType(BetName betName)
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
CarrierBet::setContractAmount(const Gen::Money& amount)
{
    contractAmount_ = amount;
}

//----------------------------------------------------------------

void
CarrierBet::setOddsAmount(const Gen::Money& amount)
{
    contractAmount_ = amount;
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
    return contractAmount_;
}

//----------------------------------------------------------------
