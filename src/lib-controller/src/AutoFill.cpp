//----------------------------------------------------------------
//
// File: AutoFill.cpp
//
//----------------------------------------------------------------

#include <controller/AutoFill.h>
#include <controller/BetEntryYaml.h>
#include <algorithm>

using namespace Ctrl;

//----------------------------------------------------------------

Gen::ReturnCode
AutoFill::getAutoFill(AutoFillEntry& afe)
{
    auto it = findEntry(afe);
    if (it != fills_.end())
    {
        afe.amount = it->amount;  // Replace amount
        return Gen::ReturnCode::Success;
    }
    return Gen::ReturnCode::Fail;
}

//----------------------------------------------------------------

void
AutoFill::setAutoFill(const AutoFillEntry& afe)
{
    auto it = findEntry(afe);
    if (it != fills_.end())
    {
        it->amount = afe.amount;  // Replace amount only
    }
    else
    {
        fills_.push_back(afe);    // No match: add new entry
    }
}

//----------------------------------------------------------------

void
AutoFill::deleteAutoFill(const AutoFillEntry& afe)
{
    auto it = findEntry(afe);
    if (it != fills_.end())
    {
        // Erase compacts the vector automatically (no "holes")
        fills_.erase(it);
    }
}

//----------------------------------------------------------------

const AutoFill::AutoFills&
AutoFill::getAutoFills()
{
    return fills_;
}

//----------------------------------------------------------------

void
AutoFill::clear()
{
    fills_.clear();
}

//----------------------------------------------------------------
//
// Entries as a YAML sequence, in order.
//
YAML::Node
AutoFill::toYAML() const
{
    return betEntriesToYAML(fills_);
}

//----------------------------------------------------------------
//
// Replaces current entries with those in the YAML sequence. Never
// throws. Anything unusable is logged and skipped.
//
void
AutoFill::fromYAML(const YAML::Node& node)
{
    fills_.clear();
    betEntriesFromYAML<AutoFillEntry>(node, "AutoFill::fromYAML(): ",
        [this](const AutoFillEntry& e) { setAutoFill(e); });  // collapses dups
}

//----------------------------------------------------------------
//
// Look for an existing entry with same betName, pivot, and isOddsBet
//
AutoFill::AutoFills::iterator
AutoFill::findEntry(const AutoFillEntry& afe)
{
    return std::find_if(fills_.begin(), fills_.end(),
        [&](const AutoFillEntry& existing) {
            return existing.betName   == afe.betName
                && existing.pivot     == afe.pivot
                && existing.isOddsBet == afe.isOddsBet;
        });
}

//----------------------------------------------------------------
//
// Look for an existing entry with same betName, pivot, and isOddsBet
//
AutoFill::AutoFills::const_iterator
AutoFill::findEntry(const AutoFillEntry& afe) const
{
    return std::find_if(fills_.cbegin(), fills_.cend(),
        [&](const AutoFillEntry& existing) {
            return existing.betName   == afe.betName
                && existing.pivot     == afe.pivot
                && existing.isOddsBet == afe.isOddsBet;
        });
}

//----------------------------------------------------------------

