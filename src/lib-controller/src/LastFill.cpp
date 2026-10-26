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

