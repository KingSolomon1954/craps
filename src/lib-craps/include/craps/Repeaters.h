//----------------------------------------------------------------
//
// File: Repeaters.h
//
//----------------------------------------------------------------

#pragma once

#include <map>
#include <craps/CrapsTypes.h>
#include <yaml-cpp/yaml.h>

namespace Craps {

class Repeaters
{
public:
    /// Maps hits to the number of times a run ended with that many hits.
    using Buckets = std::map<unsigned, unsigned>;

    /// @name Lifecycle
    /// @{
    explicit Repeaters(const DiceNums& numbers);
    /// @}

    /// @name Modifiers
    /// @{
    void hit   (unsigned number);
    void endRun  (bool recordZeros = false);
    void closeRun(const DiceNums& numbers, bool recordZeros = false);
    void merge (const Repeaters& session);
    void reset();
    /// @}

    /// @name Observers
    /// @{
    const Buckets& buckets(unsigned number) const;
    unsigned       times  (unsigned number, unsigned hits) const;
    unsigned       running(unsigned number) const;
    bool           tracks (unsigned number) const;
    bool           operator==(const Repeaters& other) const;
    /// @}

    YAML::Node toYAML() const;
    void fromYAML(const YAML::Node& node);

private:
    std::map<unsigned, Buckets>  buckets_;  // Key is a tracked number
    std::map<unsigned, unsigned> running_;  // Hits so far in the current run
};

/*-----------------------------------------------------------*//**

@class Repeaters

@brief Counts how many times a number repeats before a run ends.

A run is the stretch of rolls that ends with a seven (or a seven-out,
depending on who is counting). Each hit of a tracked number adds to a
running tally. When the run ends, every number whose tally is non-zero
adds one to its bucket for that many hits, and the tallies restart.

For example, (4: 1 hit: 57 times) means that a 4 was rolled exactly
once before the run ended, 57 times.

@par Zero hits

Zero-hit runs are normally not recorded, since a run only starts when
the number appears. endRun(true) records a zero-hit bucket for every
tracked number that was not hit. Place and Buy use this, since a bet
that lost without winning is a real outcome.

@par Persistence

Only the buckets are saved. The running tallies are transient, so a
partial run at the end of a session is never recorded as finished.
toYAML() writes a map of number to a map of hits to times. fromYAML()
is strict and throws std::runtime_error when a tracked number is
missing.

*/

} // namespace Craps

//----------------------------------------------------------------
