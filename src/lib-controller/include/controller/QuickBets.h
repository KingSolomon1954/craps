//----------------------------------------------------------------
//
// File: QuickBets.h
//
//----------------------------------------------------------------

#pragma once

#include <controller/BetSpec.h>
#include <yaml-cpp/yaml.h>
#include <cstddef>
#include <string>
#include <vector>

namespace Ctrl {

class QuickBets
{
public:
    static constexpr std::size_t MaxEntries        = 20;
    static constexpr std::size_t MaxSpecsPerEntry  = 12;

    /// @name Modifiers
    /// @{

    /// Adds the plan, or replaces the plan of the same name (case
    /// sensitive, keeping its place in the list). The plan is made valid
    /// first, see tidy(). Returns false, and changes nothing, if the plan
    /// has no name or no usable bets, or the list is full.
    bool set(BetPlan plan);

    /// Removes the entry at index. Returns false if there is none.
    bool remove(std::size_t index);

    void clear();
    void fromYAML(const YAML::Node& node);
    /// @}

    /// @name Observers
    /// @{
    const std::vector<BetPlan>& plans() const;
    const BetPlan* find(const std::string& name) const;
    YAML::Node toYAML() const;
    /// @}

    /// What a plan must be to be kept, so that applying one never meets
    /// nonsense. Drops bets with no or invalid bet name, a zero amount, or
    /// odds on a bet that cannot be in a plan. A later bet for the same spot
    /// replaces the earlier. Keeps at most MaxSpecsPerEntry. Contract bets
    /// come before odds bets, otherwise order is kept.
    static BetPlan tidy(BetPlan plan);

private:
    std::vector<BetPlan> plans_;
};

/*-----------------------------------------------------------*//**

@class QuickBets

@brief A player's named bundles of bets

Plain data, one per player, held in PlayerPrefs. It does not place bets;
BetExecutor does. A bad or missing section in a player file gives an
empty list, never an error.

@code
QuickBets:
  - name: Place Hi
    bets:
      - { betName: Place, pivot: 8,  amount: 24 }
      - { betName: Place, pivot: 9,  amount: 20 }
  - name: Line with odds
    bets:
      - { betName: PassLine, amount: 10 }
      - { betName: PassLine, isOdds: true, amount: 30 }
@endcode

pivot and isOdds may be left out; they are 0 and false. Entries and
bets that cannot be used are logged and skipped, and the entries
beyond the limit are dropped.

*/

} // namespace Ctrl

//----------------------------------------------------------------
