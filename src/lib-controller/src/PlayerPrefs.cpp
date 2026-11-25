//----------------------------------------------------------------
//
// File: PlayerPrefs.cpp
//
//----------------------------------------------------------------

#include <controller/PlayerPrefs.h>

using namespace Ctrl;

//----------------------------------------------------------------

LastFill&
PlayerPrefs::lastFill()
{
    return lastFill_;
}

//----------------------------------------------------------------

const LastFill&
PlayerPrefs::lastFill() const
{
    return lastFill_;
}

//----------------------------------------------------------------

QuickBets&
PlayerPrefs::quickBets()
{
    return quickBets_;
}

//----------------------------------------------------------------

const QuickBets&
PlayerPrefs::quickBets() const
{
    return quickBets_;
}

//----------------------------------------------------------------

AutoOdds&
PlayerPrefs::autoOdds()
{
    return autoOdds_;
}

//----------------------------------------------------------------

const AutoOdds&
PlayerPrefs::autoOdds() const
{
    return autoOdds_;
}

//----------------------------------------------------------------

void
PlayerPrefs::setBuddyFollow(BuddyFollow value)
{
    buddyFollow_ = value;
}

//----------------------------------------------------------------

BuddyFollow
PlayerPrefs::buddyFollow() const
{
    return buddyFollow_;
}

//----------------------------------------------------------------

YAML::Node
PlayerPrefs::toYAML() const
{
    YAML::Node node(YAML::NodeType::Map);
    node["LastFills"] = lastFill_.toYAML();
    node["QuickBets"] = quickBets_.toYAML();
    node["AutoOdds"]  = autoOdds_.toYAML();
    node["BuddiesFollow"] = toString(buddyFollow_);
    return node;
}

//----------------------------------------------------------------
//
// Replaces current contents. Anything missing or unusable gives
// empty preferences.
//
void
PlayerPrefs::fromYAML(const YAML::Node& node)
{
    if (node.IsDefined() && node.IsMap())
    {
        lastFill_.fromYAML(node["LastFills"]);
        quickBets_.fromYAML(node["QuickBets"]);
        autoOdds_.fromYAML(node["AutoOdds"]);

        buddyFollow_ = BuddyFollow::Follow;
        const YAML::Node follow = node["BuddiesFollow"];
        if (follow.IsDefined() && follow.IsScalar())
        {
            if (auto v = parseBuddyFollow(follow.as<std::string>()))
            {
                buddyFollow_ = *v;
            }
        }
    }
    else
    {
        lastFill_.clear();
        quickBets_.clear();
        autoOdds_.fromYAML(YAML::Node());
        buddyFollow_ = BuddyFollow::Follow;
    }
}

//----------------------------------------------------------------
