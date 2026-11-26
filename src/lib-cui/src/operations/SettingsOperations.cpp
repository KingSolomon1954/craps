//----------------------------------------------------------------
//
// File: SettingsOperations.cpp
//
//----------------------------------------------------------------

#include <cui/operations/SettingsOperations.h>
#include <cui/core/CuiSender.h>
#include <controller/AnimationSpeed.h>
#include <controller/BuddyFollow.h>
#include <controller/CrapsReaders.h>
#include <controller/RollHistoryFormat.h>
#include <cctype>

using namespace Cui;

namespace {

const char*
describe(Ctrl::BuddyFollow b)
{
    switch (b)
    {
    case Ctrl::BuddyFollow::Follow: return "buddies go to the new table with you";
    case Ctrl::BuddyFollow::Stay:   return "buddies leave the game";
    case Ctrl::BuddyFollow::Ask:    return "ask each time buddies are at the table";
    }
    return "";
}

const char*
label(Ctrl::BuddyFollow b)
{
    switch (b)
    {
    case Ctrl::BuddyFollow::Follow: return "Follow";
    case Ctrl::BuddyFollow::Stay:   return "Stay";
    case Ctrl::BuddyFollow::Ask:    return "Ask";
    }
    return "";
}

const char*
describe(Ctrl::AnimationSpeed s)
{
    switch (s)
    {
    case Ctrl::AnimationSpeed::Off:    return "no animation; the roll shows at once";
    case Ctrl::AnimationSpeed::Fast:   return "twice as fast as normal";
    case Ctrl::AnimationSpeed::Normal: return "the usual pace";
    case Ctrl::AnimationSpeed::Slow:   return "one and a half times slower";
    }
    return "";
}

std::string
label(Ctrl::AnimationSpeed s)
{
    std::string t = Ctrl::toString(s);
    t[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(t[0])));
    return t;
}

const char*
sample(Ctrl::RollHistoryFormat f)
{
    switch (f)
    {
    case Ctrl::RollHistoryFormat::Spaced:     return "7 8 6 10";
    case Ctrl::RollHistoryFormat::Commas:     return "7,8,6,10";
    case Ctrl::RollHistoryFormat::DiceJoined: return "7(34) 8(44) 6(33) 10(65)";
    case Ctrl::RollHistoryFormat::DiceComma:  return "7(3,4) 8(4,4) 6(3,3) 10(6,5)";
    }
    return "";
}

} // namespace

//----------------------------------------------------------------
//
// Buddy Follow
//

void
BuddyFollowOperation::start()
{
    const auto current = Ctrl::CrapsReaders::readBuddyFollow();

    std::vector<DialogPicker::Item> items;
    std::size_t index = 0;
    for (auto b : Ctrl::AllBuddyFollows)
    {
        DialogPicker::Item item;
        item.tag         = index++;
        item.text        = label(b);
        item.note        = (b == current) ? "(current)" : "";
        item.description = describe(b);
        items.push_back(std::move(item));
    }
    pickFrom("Buddies Follow on Table Switch", std::move(items));
}

//----------------------------------------------------------------

void
BuddyFollowOperation::onPick(bool accepted, const Craps::PlayerId&,
                             Craps::BetId, std::size_t tag)
{
    if (accepted && tag < Ctrl::AllBuddyFollows.size())
    {
        CuiSender::instance().reqSetBuddyFollow(Ctrl::AllBuddyFollows[tag]);
    }
    finish();
}

//----------------------------------------------------------------
//
// Animation Speed
//

void
AnimationSpeedOperation::start()
{
    const auto current = Ctrl::CrapsReaders::readAnimationSpeed();

    std::vector<DialogPicker::Item> items;
    std::size_t index = 0;
    for (auto s : Ctrl::AllAnimationSpeeds)
    {
        DialogPicker::Item item;
        item.tag         = index++;
        item.text        = label(s);
        item.note        = (s == current) ? "(current)" : "";
        item.description = describe(s);
        items.push_back(std::move(item));
    }
    pickFrom("Animation Speed", std::move(items));
}

//----------------------------------------------------------------

void
AnimationSpeedOperation::onPick(bool accepted, const Craps::PlayerId&,
                                Craps::BetId, std::size_t tag)
{
    if (accepted && tag < Ctrl::AllAnimationSpeeds.size())
    {
        CuiSender::instance().reqSetAnimationSpeed(
            Ctrl::AllAnimationSpeeds[tag]);
    }
    finish();
}

//----------------------------------------------------------------
//
// Roll History Format. Keys 1 to 4 pick directly.
//

void
RollHistoryFormatOperation::start()
{
    const auto current = Ctrl::CrapsReaders::readRollHistoryFormat();

    std::vector<DialogPicker::Item> items;
    std::size_t index = 0;
    for (auto f : Ctrl::AllRollHistoryFormats)
    {
        DialogPicker::Item item;
        item.tag         = index++;
        item.text        = sample(f);
        item.description = (f == current) ? "current" : "";
        items.push_back(std::move(item));
    }
    pickFrom("Format Roll History", std::move(items));
}

//----------------------------------------------------------------

void
RollHistoryFormatOperation::onPick(bool accepted, const Craps::PlayerId&,
                                   Craps::BetId, std::size_t tag)
{
    if (accepted && tag < Ctrl::AllRollHistoryFormats.size())
    {
        CuiSender::instance().reqSetRollHistoryFormat(
            Ctrl::AllRollHistoryFormats[tag]);
    }
    finish();
}

//----------------------------------------------------------------
