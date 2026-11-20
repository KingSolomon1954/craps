//----------------------------------------------------------------
//
// File: AnimationSpeed.h
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

enum class AnimationSpeed
{
    Off,
    Fast,
    Normal,
    Slow
};

//----------------------------------------------------------------

inline constexpr std::array<AnimationSpeed, 4> AllAnimationSpeeds =
{
    AnimationSpeed::Off,  AnimationSpeed::Fast,
    AnimationSpeed::Normal, AnimationSpeed::Slow
};

/// The word used in config.yaml, state.yaml and on the command line
inline const char* toString(AnimationSpeed s)
{
    switch (s)
    {
    case AnimationSpeed::Off:    return "off";
    case AnimationSpeed::Fast:   return "fast";
    case AnimationSpeed::Normal: return "normal";
    case AnimationSpeed::Slow:   return "slow";
    }
    return "normal";
}

/// Accepts the words above in any case, with surrounding blanks.
/// Also accepts "false" for off and "true" for normal, as YAML turns
/// an unquoted off or on into a boolean.
inline std::optional<AnimationSpeed> parseAnimationSpeed(std::string_view v)
{
    std::string w;
    for (char c : v)
    {
        if (!std::isspace(static_cast<unsigned char>(c)))
        {
            w += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        }
    }
    if (w == "off"    || w == "false" || w == "no")  return AnimationSpeed::Off;
    if (w == "on"     || w == "true"  || w == "yes" ||
        w == "normal")                               return AnimationSpeed::Normal;
    if (w == "fast")                                 return AnimationSpeed::Fast;
    if (w == "slow")                                 return AnimationSpeed::Slow;
    return std::nullopt;
}

/*-----------------------------------------------------------*//**

@enum AnimationSpeed

@brief How fast the dice roll animation plays, or Off for none

The View decides what each speed means in time. Off means the roll is
shown at once, with no animation.
*/

} // namespace Ctrl

//----------------------------------------------------------------
