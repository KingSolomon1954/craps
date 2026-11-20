//----------------------------------------------------------------
//
// File: RollHistoryFormat.h
//
//----------------------------------------------------------------

#pragma once

#include <array>
#include <optional>
#include <string>
#include <string_view>

//----------------------------------------------------------------

namespace Ctrl {

/// How the roll history shows each roll, most compact first.
/// The numbers are what config.yaml and state.yaml use.
enum class RollHistoryFormat
{
    Spaced     = 1,    ///< 7 8 6 10
    Commas     = 2,    ///< 7,8,6,10
    DiceJoined = 3,    ///< 7(34) 8(44) 6(33) 10(65)
    DiceComma  = 4     ///< 7(3,4) 8(4,4) 6(3,3) 10(6,5)
};

inline constexpr std::array<RollHistoryFormat, 4> AllRollHistoryFormats =
{
    RollHistoryFormat::Spaced,     RollHistoryFormat::Commas,
    RollHistoryFormat::DiceJoined, RollHistoryFormat::DiceComma
};

inline std::string toString(RollHistoryFormat f)
{
    return std::to_string(static_cast<int>(f));
}

/// Accepts "1" to "4", with surrounding blanks.
inline std::optional<RollHistoryFormat> parseRollHistoryFormat(std::string_view v)
{
    std::string w;
    for (char c : v)
    {
        if (c != ' ' && c != '\t') w += c;
    }
    if (w == "1") return RollHistoryFormat::Spaced;
    if (w == "2") return RollHistoryFormat::Commas;
    if (w == "3") return RollHistoryFormat::DiceJoined;
    if (w == "4") return RollHistoryFormat::DiceComma;
    return std::nullopt;
}

} // namespace Ctrl

//----------------------------------------------------------------
