//----------------------------------------------------------------
//
// File: AutoOdds.h
//
//----------------------------------------------------------------

#pragma once

#include <controller/BetSpec.h>
#include <craps/EnumBetName.h>
#include <craps/TableRules.h>
#include <yaml-cpp/yaml.h>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace Ctrl {

class AutoOdds
{
public:
    static constexpr unsigned MaxMultiple = 100;

    /// A bet on the table that AutoOdds may put odds on. The controller
    /// fills these in from the player's bets and the table's limits so
    /// that plan() needs no table.
    struct Candidate
    {
        unsigned                betId    = 0;
        BetName                 betName  = BetName::Invalid;
        unsigned                pivot    = 0;
        Gen::Money              contract = 0;
        Gen::Money              odds     = 0;
        Craps::BetAmountLimits  limits;     ///< Table limits for these odds
    };

    /// What to put on one bet type at one point: a number of times the
    /// contract amount, or an exact dollar amount. Both zero means none.
    struct Setting
    {
        unsigned   multiple = 0;
        Gen::Money amount   = 0;

        bool isSet() const    { return multiple != 0 || amount != 0; }
        bool isAmount() const { return amount != 0; }
        bool operator==(const Setting&) const = default;

        /// "3x" or "$50"; "none" if not set
        std::string toString() const;
    };

    /// What one pass decided
    struct Plan
    {
        std::vector<BetSpec>     specs;     ///< Odds to place
        std::vector<std::string> skipped;   ///< Sentences for odds not placed
    };

    /// @name Modifiers
    /// @{
    void setEnabled(bool enabled);

    /// Sets the bet type at the point. A setting that is not set clears it.
    /// A point of 0 means every point. Returns "" on success, otherwise
    /// why not.
    std::string set(BetName betName, unsigned point, const Setting& setting);

    /// Same as set() for a number of times the contract.
    std::string setMultiple(BetName betName, unsigned point, unsigned multiple);

    /// Same as set() for an exact amount.
    std::string setAmount(BetName betName, unsigned point, Gen::Money amount);

    /// 3x for 4 and 10, 4x for 5 and 9, 5x for 6 and 8, for all four types.
    void setPreset345();

    /// Clears every setting. The on/off switch is left as it is.
    void clearAll();

    /// Works out the odds to place for the candidates, and remembers the
    /// bets so that they are not tried again. See the design document.
    Plan plan(const std::vector<Candidate>& candidates);

    /// Forgets which bets were handled.
    void forgetHandled();

    void fromYAML(const YAML::Node& node);
    /// @}

    /// @name Observers
    /// @{
    bool     enabled() const;
    Setting  setting(BetName betName, unsigned point) const;
    unsigned multiple(BetName betName, unsigned point) const;
    Gen::Money amount(BetName betName, unsigned point) const;
    bool     anySet() const;

    /// The settings of a bet type in a line, e.g. "4:3x 5:4x 6:$50" or
    /// "none". Points with the same setting are not combined.
    std::string summary(BetName betName) const;

    YAML::Node toYAML() const;
    /// @}

    /// @name Helpers
    /// @{
    static bool isOddsBetName(BetName betName);
    static bool isPoint(unsigned point);

    /// The four bet types that can have odds, in menu order.
    static const std::vector<BetName>& oddsBetNames();

    /// The six points, in order.
    static const std::vector<unsigned>& points();

    /// The odds to place: the exact amount, or the multiple times the
    /// contract. Limited to the table's maximum and rounded down to the
    /// table's increment. Zero means nothing can be placed.
    static Gen::Money oddsAmount(const Setting& setting, Gen::Money contract,
                                 const Craps::BetAmountLimits& limits);

    /// Reads what a person typed: "3x" for a multiple, "50" or "$50" for
    /// an amount, "0" for none. Returns "" on success, otherwise why not.
    static std::string parseSetting(const std::string& text, Setting& setting);
    /// @}

private:
    using Key = std::pair<BetName, unsigned>;

    bool                    enabled_ = false;
    std::map<Key, Setting>  settings_;
    std::set<unsigned>      handled_;
};

/*-----------------------------------------------------------*//**

@class AutoOdds

@brief One player's automatic odds settings

Holds, per bet type (PassLine, DontPass, Come, DontCome) and point
(4, 5, 6, 8, 9, 10), what to put on as odds: either a number of times the
contract amount or an exact dollar amount. Nothing set means no automatic
odds for that spot. A master
switch turns the whole thing on and off without losing the settings.

plan() is the pass: it looks at the bets with a point and no odds that it
has not handled yet and says what odds to place. Whatever happens to those
odds, by the table or by the player, the bet is handled and is not tried
again, so odds the player takes down stay down. The handled bets are not
saved; they are forgotten on load and with forgetHandled(), and a bet that
is gone is dropped from the list.

Persistence, as the "AutoOdds" key of the player's Prefs:

@code
AutoOdds:
  enabled: true
  multiples:
    PassLine: {4: 3x, 5: 4x, 6: 5x, 8: 5x, 9: 4x, 10: 3x}
    Come: {6: $50}
@endcode

A number with an x is a multiple and one with a $ is an amount. A plain
number is read as a multiple. fromYAML() is forgiving: anything unusable
is skipped, never thrown.

*/

} // namespace Ctrl

//----------------------------------------------------------------
