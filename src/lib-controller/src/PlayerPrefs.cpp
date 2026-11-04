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

YAML::Node
PlayerPrefs::toYAML() const
{
    YAML::Node node(YAML::NodeType::Map);
    node["LastFills"] = lastFill_.toYAML();
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
    }
    else
    {
        lastFill_.clear();
    }
}

//----------------------------------------------------------------
