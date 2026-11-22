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

YAML::Node
PlayerPrefs::toYAML() const
{
    YAML::Node node(YAML::NodeType::Map);
    node["LastFills"] = lastFill_.toYAML();
    node["QuickBets"] = quickBets_.toYAML();
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
    }
    else
    {
        lastFill_.clear();
        quickBets_.clear();
    }
}

//----------------------------------------------------------------
