//----------------------------------------------------------------
//
// File: EnumBetName.h
//
//----------------------------------------------------------------

#pragma once

#include <string>
#include <ostream>
#include <array>

/*-----------------------------------------------------------*//**

@brief Name of a craps bet.

@ingroup free

*/
enum class BetName
{
    Invalid = 0, ///< Used for conversion errors
    PassLine,    ///< Pass line bet
    Come,        ///< Come bet
    DontPass,    ///< Dont Pass bet
    DontCome,    ///< Dont Come bet
    Place,       ///< Place bet
    Lay,         ///< Lay bet
    Buy,         ///< Buy bet
    Hardway,     ///< Hardway bet
    Field,       ///< Field bet
    AnyCraps,    ///< Any Craps bet
    AnySeven,    ///< Any Seven bet
    CandE,       ///< Craps & Eleven bet
    Horn         ///< Horn bet
};

namespace EnumBetName
{
    BetName     fromString(const std::string& s);
    BetName     fromUnsigned(unsigned v)                noexcept;
    std::string toString(BetName b)                     noexcept;
    bool        isConvertibleFrom(const std::string& s) noexcept;
    std::string listAsString();

    /** Initialized array of enums for iterating. */
    constexpr const std::array<BetName, 13> enumerators
    {
        // BetName::Invalid, // Purposely leave out
        BetName::PassLine,   ///< Pass line bet
        BetName::Come,       ///< Come bet
        BetName::DontPass,   ///< Dont Pass bet
        BetName::DontCome,   ///< Dont Come bet
        BetName::Place,      ///< Place bet
        BetName::Lay,        ///< Lay bet
        BetName::Buy,        ///< Buy bet
        BetName::Hardway,    ///< Hardway bet
        BetName::Field,      ///< Field bet
        BetName::AnyCraps,   ///< Any Craps bet
        BetName::AnySeven,   ///< Any Seven bet
        BetName::CandE,      ///< Craps & Eleven bet
        BetName::Horn        ///< Horn bet
    };
}

std::ostream& operator<< (std::ostream& out, const BetName& d);

/*-----------------------------------------------------------*//**

@namespace EnumBetName

@brief Functions that operate on BetName enum.

This is a small namespace holding functions that only
operate on BetName enum.

*/

//-----------------------------------------------------------------
