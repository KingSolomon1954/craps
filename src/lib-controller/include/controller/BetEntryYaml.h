//----------------------------------------------------------------
//
// File: BetEntryYaml.h
//
//----------------------------------------------------------------

#pragma once

#include <craps/EnumBetName.h>
#include <gen/Logger.h>
#include <yaml-cpp/yaml.h>
#include <exception>
#include <string>

namespace Ctrl {

/// Entries as a YAML sequence, in the order they are in the container.
template <class Container>
YAML::Node
betEntriesToYAML(const Container& entries)
{
    YAML::Node seq(YAML::NodeType::Sequence);
    for (const auto& e : entries)
    {
        YAML::Node n;
        n["betName"]   = EnumBetName::toString(e.betName);
        n["pivot"]     = e.pivot;
        n["isOddsBet"] = e.isOddsBet;
        n["amount"]    = e.amount;
        seq.push_back(n);
    }
    return seq;
}

/// Reads a YAML sequence, calling add(entry) for each good entry in
/// order. Never throws. A missing section is normal and silently gives
/// nothing. A section that is not a sequence, and individual bad
/// entries, are logged and skipped. who is the log message prefix.
template <class Entry, class AddFn>
void
betEntriesFromYAML(const YAML::Node& node, const std::string& who, AddFn add)
{
    if (!node.IsDefined() || node.IsNull())
    {
        return;
    }
    if (!node.IsSequence())
    {
        LOG_WARN(who + "not a sequence; ignoring it");
        return;
    }

    for (const auto& n : node)
    {
        try
        {
            Entry e;
            e.betName   = EnumBetName::fromString(n["betName"].as<std::string>());
            e.pivot     = n["pivot"].as<size_t>();
            e.isOddsBet = n["isOddsBet"].as<bool>();
            e.amount    = n["amount"].as<Gen::Money>();
            add(e);
        }
        catch (const std::exception& ex)
        {
            LOG_WARN(who + "skipping bad entry: " + ex.what());
        }
    }
}

/*-----------------------------------------------------------*//**

@file BetEntryYaml.h

@brief Shared YAML form of a list of bet entries (LastFill today)

Classes that hold a list of entries with these four fields use this, so
there is one place that knows how such a list looks in a player file:

@code
- betName: PassLine
  pivot: 0
  isOddsBet: false
  amount: 50
@endcode

*/

} // namespace Ctrl

//----------------------------------------------------------------
