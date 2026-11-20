//----------------------------------------------------------------
//
// File: RollLog.h
//
//----------------------------------------------------------------

#pragma once

#include <cstddef>
#include <deque>

//----------------------------------------------------------------

namespace Cui {

/// One remembered roll and what it meant. The flags are set after the
/// roll, as the results arrive, so colors can be redrawn at any time.
struct RollEntry
{
    unsigned d1 = 0;
    unsigned d2 = 0;
    bool passLineWinner = false;
    bool sevenOut       = false;
    bool point          = false;
};

/*-----------------------------------------------------------*//**

@class RollLog

@brief The View's memory of recent rolls, newest first

@li Shared by WindowRollHistory (a row at the top of the screen) and
    DeepRollHistory (a popup of the last Capacity rolls).
@li Holds the rolls with their colors' flags, which the model does not
    keep.
@li Not a game rule: it is only what the View has seen since it started.
*/
class RollLog
{
public:
    /// Enough for the Deep Roll History popup: 20 rows of 6 columns.
    static constexpr std::size_t Capacity = 120;

    static RollLog& instance()
    {
        static RollLog log;
        return log;
    }

    /// @name Modifiers
    /// @{
    void add(unsigned d1, unsigned d2)
    {
        rolls_.push_front(RollEntry{d1, d2});
        if (rolls_.size() > Capacity) rolls_.pop_back();
    }

    // Retroactively mark the newest roll. No-ops when there is none.
    void markSevenOut()       { if (!rolls_.empty()) rolls_.front().sevenOut       = true; }
    void markPassLineWinner() { if (!rolls_.empty()) rolls_.front().passLineWinner = true; }
    void markPoint()          { if (!rolls_.empty()) rolls_.front().point          = true; }
    void clear()              { rolls_.clear(); }
    /// @}

    /// @name Observers
    /// @{
    const std::deque<RollEntry>& rolls() const { return rolls_; }   // newest first
    /// @}

private:
    RollLog() = default;
    std::deque<RollEntry> rolls_;
};

} // namespace Cui

//----------------------------------------------------------------
