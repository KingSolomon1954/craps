//----------------------------------------------------------------
//
// File: BuddyFollow.h
//
//----------------------------------------------------------------

#pragma once

#include <array>
#include <cctype>
#include <optional>
#include <string>
#include <string_view>

//----------------------------------------------------------------

namespace Ctrl {

enum class BuddyFollow
{
    Follow,    ///< Buddies at the table go along to the new table
    Stay,      ///< Buddies leave the game when the user changes tables
    Ask        ///< Ask each time there are buddies at the table
};

inline constexpr std::array<BuddyFollow, 3> AllBuddyFollows =
{
    BuddyFollow::Follow, BuddyFollow::Stay, BuddyFollow::Ask
};

/// The word used in the player file
inline const char* toString(BuddyFollow b)
{
    switch (b)
    {
    case BuddyFollow::Follow: return "follow";
    case BuddyFollow::Stay:   return "stay";
    case BuddyFollow::Ask:    return "ask";
    }
    return "follow";
}

/// Accepts the words above in any case, with surrounding blanks.
inline std::optional<BuddyFollow> parseBuddyFollow(std::string_view v)
{
    std::string w;
    for (char c : v)
    {
        if (!std::isspace(static_cast<unsigned char>(c)))
        {
            w += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        }
    }
    if (w == "follow") return BuddyFollow::Follow;
    if (w == "stay")   return BuddyFollow::Stay;
    if (w == "ask")    return BuddyFollow::Ask;
    return std::nullopt;
}

/*-----------------------------------------------------------*//**

@enum BuddyFollow

@brief What the buddy players do when the user player changes tables

Follow is the default. The choice is kept with the user player's
preferences, see PlayerPrefs. With Ask the View asks at the time and
then sends the answer, Follow or Stay, with the request to switch.

*/

} // namespace Ctrl

//----------------------------------------------------------------
