//----------------------------------------------------------------
//
// File: LastFill.cpp
//
//----------------------------------------------------------------

#include <controller/LastFill.h>
#include <controller/GameEvents.h>
#include <controller/PlayerManager.h>
#include <craps/CrapsTypes.h>
#include <craps/Player.h>
#include <gen/Logger.h>
#include <algorithm>
#include <exception>
#include <string>

using namespace Ctrl;

//----------------------------------------------------------------

LastFill&
LastFill::instance()
{
    static LastFill lf;
    return lf;
}

//----------------------------------------------------------------

Gen::ReturnCode
LastFill::getLastFill(LastFillEntry& lfe)
{
    auto it = findEntry(lfe);
    if (it != fills_.end())
    {
        lfe.amount = it->amount;  // Replace amount
        return Gen::ReturnCode::Success;
    }
    return Gen::ReturnCode::Fail;
}

//----------------------------------------------------------------

void
LastFill::setLastFill(const LastFillEntry& lfe)
{
    auto it = findEntry(lfe);
    if (it != fills_.end())
    {
        it->amount = lfe.amount;  // Replace amount only
    }
    else
    {
        fills_.push_back(lfe);    // No match: add new entry
    }
}

//----------------------------------------------------------------

void
LastFill::deleteLastFill(const LastFillEntry& lfe)
{
    auto it = findEntry(lfe);
    if (it != fills_.end())
    {
        // Erase compacts the vector automatically (no "holes")
        fills_.erase(it);
    }
}

//----------------------------------------------------------------

const LastFill::LastFills&
LastFill::getLastFills()
{
    return fills_;
}

//----------------------------------------------------------------

void
LastFill::clear()
{
    fills_.clear();
}

//----------------------------------------------------------------
//
// Entries as a YAML sequence, in the order they were first set.
//
YAML::Node
LastFill::toYAML() const
{
    YAML::Node seq(YAML::NodeType::Sequence);
    for (const auto& e : fills_)
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

//----------------------------------------------------------------
//
// Replaces current entries with those in the YAML sequence. Never
// throws. Anything unusable is logged and skipped, leaving whatever
// good entries remain.
//
void
LastFill::fromYAML(const YAML::Node& node)
{
    const std::string fname("LastFill::fromYAML(): ");

    fills_.clear();

    // Missing section is normal, e.g. a player who never bet, or a
    // player file written before this feature existed.
    if (!node.IsDefined() || node.IsNull())
    {
        return;
    }
    if (!node.IsSequence())
    {
        LOG_WARN(fname + "LastFills is not a sequence; ignoring it");
        return;
    }

    for (const auto& n : node)
    {
        try
        {
            LastFillEntry e;
            e.betName   = EnumBetName::fromString(n["betName"].as<std::string>());
            e.pivot     = n["pivot"].as<size_t>();
            e.isOddsBet = n["isOddsBet"].as<bool>();
            e.amount    = n["amount"].as<Gen::Money>();

            if (e.amount < Gen::Money(0))
            {
                LOG_WARN(fname + "skipping entry with negative amount");
                continue;
            }
            setLastFill(e);   // also collapses duplicate entries
        }
        catch (const std::exception& ex)
        {
            LOG_WARN(fname + "skipping bad entry: " + ex.what());
        }
    }
}

//----------------------------------------------------------------
//
// Look for an existing entry with same betName, pivot, and isOddsBet
//
LastFill::LastFills::iterator
LastFill::findEntry(const LastFillEntry& lfe)
{
    return std::find_if(fills_.begin(), fills_.end(),
        [&](const LastFillEntry& existing) {
            return existing.betName   == lfe.betName
                && existing.pivot     == lfe.pivot
                && existing.isOddsBet == lfe.isOddsBet;
        });
}

//----------------------------------------------------------------
//
// Look for an existing entry with same betName, pivot, and isOddsBet
//
LastFill::LastFills::const_iterator
LastFill::findEntry(const LastFillEntry& lfe) const
{
    return std::find_if(fills_.cbegin(), fills_.cend(),
        [&](const LastFillEntry& existing) {
            return existing.betName   == lfe.betName
                && existing.pivot     == lfe.pivot
                && existing.isOddsBet == lfe.isOddsBet;
        });
}

//----------------------------------------------------------------

