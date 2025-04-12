//----------------------------------------------------------------
//
// File: EnumBetName.cpp
//
//----------------------------------------------------------------

#include "EnumBetName.h"
#include "gen/StringUtils.h"

using namespace EnumBetName;
using namespace Gen;

constexpr const char* InvalidStr  = "Invalid";
constexpr const char* PassLineStr = "PassLine";
constexpr const char* ComeStr     = "Come";
constexpr const char* DontPassStr = "DontPass";
constexpr const char* DontComeStr = "DontCome";
constexpr const char* PlaceStr    = "Place";
constexpr const char* LayStr      = "Lay";
constexpr const char* BuyStr      = "Buy";
constexpr const char* HardwayStr  = "Hardway";
constexpr const char* FieldStr    = "Field";
constexpr const char* AnyCrapsStr = "AnyCraps";
constexpr const char* AnySevenStr = "AnySeven";
constexpr const char* CandEStr    = "C&E";
constexpr const char* HornStr     = "Horn";

/*-----------------------------------------------------------*//**

Convert enum to string.

@param[in] d
    The enum of interest.

@return
    Enum as a string.
*/
std::string
EnumBetName::toString(BetName b) noexcept
{
    switch (b)
    {
    case BetName::Invalid:  return InvalidStr;
    case BetName::PassLine: return PassLineStr;
    case BetName::Come:     return ComeStr;
    case BetName::DontPass: return DontPassStr;
    case BetName::DontCome: return DontComeStr;
    case BetName::Place:    return PlaceStr;
    case BetName::Lay:      return LayStr;
    case BetName::Buy:      return BuyStr;
    case BetName::Hardway:  return HardwayStr;
    case BetName::Field:    return FieldStr;
    case BetName::AnyCraps: return AnyCrapsStr;
    case BetName::AnySeven: return AnySevenStr;
    case BetName::CandE:    return CandEStr;
    case BetName::Horn:     return HornStr;
    }
    return InvalidStr;
}

/*-----------------------------------------------------------*//**

Convert BetName string to enum.

@param[in] s
    The bet name as a string. Matching is not sensitive to case.
    Matches against these strings:
    "Invalid", "PassLine", "Come", "DontPass", "DontCome",
    "Place", "Lay", "Buy", "Hardway", "Field", "AnyCraps",
    "AnySeven", "C&E", or "Horn".

@return
    Enum value.

@throws std::invalid_argument
    Throws for unrecognized strings. isConvertibleFrom()
    can be used if throw is not needed.
*/
BetName
EnumBetName::fromString(const std::string& s)
{
    for (auto val : enumerators)
    {
        if (StringUtils::insensitiveEquals(s, toString(val)))
        {
            return val;
        }
    }

    throw std::invalid_argument(
        "EnumBetName: Conversion error: "
        "Invalid conversion to enum: " +
        s +
        ", does not match any of " +
        listAsString());
}

/*-----------------------------------------------------------*//**

Returns a std::string of the enums separated by a space.

@return
    Returns a std::string of the enums separated by a space.
*/
std::string
EnumBetName::listAsString()
{
    std::string s;
    for (auto val : enumerators)
    {
        s += toString(val);
        s += " ";
    }
    StringUtils::trimEnd(s);
    return s;
}

/*-----------------------------------------------------------*//**

Determines if given string can convert to enum.

@param[in] s
    The string of interest.

@return
    True if convertible; false otherwise.
*/
bool
EnumBetName::isConvertibleFrom(const std::string& s) noexcept
{
    for (auto val : enumerators)
    {
        if (StringUtils::insensitiveEquals(s, toString(val)))
        {
            return true;
        }
    }
    return false;
}

/*-----------------------------------------------------------*//**

Convert to enum from unsigned integer.

@param[in] v
    The enum as an unsigned integer.

@return
    Enum value. Returns Invalid for an unrecognized conversion.
*/
BetName
EnumBetName::fromUnsigned(unsigned v) noexcept
{
    if (v == static_cast<unsigned>(BetName::PassLine))
    {
        return BetName::PassLine;
    }
    else if (v == static_cast<unsigned>(BetName::Come))
    {
        return BetName::Come;
    }
    else if (v == static_cast<unsigned>(BetName::DontPass))
    {
        return BetName::DontPass;
    }
    else if (v == static_cast<unsigned>(BetName::DontCome))
    {
        return BetName::DontCome;
    }
    else if (v == static_cast<unsigned>(BetName::Place))
    {
        return BetName::Place;
    }
    else if (v == static_cast<unsigned>(BetName::Lay))
    {
        return BetName::Lay;
    }
    else if (v == static_cast<unsigned>(BetName::Buy))
    {
        return BetName::Buy;
    }
    else if (v == static_cast<unsigned>(BetName::Hardway))
    {
        return BetName::Hardway;
    }
    else if (v == static_cast<unsigned>(BetName::Field))
    {
        return BetName::Field;
    }
    else if (v == static_cast<unsigned>(BetName::AnyCraps))
    {
        return BetName::AnyCraps;
    }
    else if (v == static_cast<unsigned>(BetName::AnySeven))
    {
        return BetName::AnySeven;
    }
    else if (v == static_cast<unsigned>(BetName::CandE))
    {
        return BetName::CandE;
    }
    else if (v == static_cast<unsigned>(BetName::Horn))
    {
        return BetName::Horn;
    }
    else
    {
        return BetName::Invalid;
    }
}

/*-----------------------------------------------------------*//**

Stream output operator for ::BetName.

@param[in,out] out
    The output stream.

@param[in] d
    The bet name enum.

@return
    The output stream.

@ingroup free
*/
std::ostream&
operator<< (std::ostream& out, const BetName& d)
{
    out << EnumBetName::toString(d);
    return out;
}

//-----------------------------------------------------------------
