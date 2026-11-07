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

AutoFill&
PlayerPrefs::autoFill()
{
    return autoFill_;
}

//----------------------------------------------------------------

const AutoFill&
PlayerPrefs::autoFill() const
{
    return autoFill_;
}

//----------------------------------------------------------------

QuickBet&
PlayerPrefs::quickBet()
{
    return quickBet_;
}

//----------------------------------------------------------------

const QuickBet&
PlayerPrefs::quickBet() const
{
    return quickBet_;
}

//----------------------------------------------------------------

YAML::Node
PlayerPrefs::toYAML() const
{
    YAML::Node node(YAML::NodeType::Map);
    node["LastFills"] = lastFill_.toYAML();
    node["AutoFills"] = autoFill_.toYAML();
    node["QuickBets"] = quickBet_.toYAML();
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
        autoFill_.fromYAML(node["AutoFills"]);
        quickBet_.fromYAML(node["QuickBets"]);
    }
    else
    {
        lastFill_.clear();
        autoFill_.clear();
        quickBet_.clear();
    }
}

//----------------------------------------------------------------
