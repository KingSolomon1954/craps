//----------------------------------------------------------------
//
// File: BetExecutor.cpp
//
//----------------------------------------------------------------

#include <controller/BetExecutor.h>
#include <controller/LastFill.h>
#include <controller/UndoBet.h>
#include <controller/UndoManager.h>
#include <craps/Player.h>
#include <algorithm>
#include <memory>

using namespace Ctrl;

//----------------------------------------------------------------

std::size_t
BetExecutor::Result::count(Status status) const
{
    return static_cast<std::size_t>(std::count_if(
        outcomes.begin(), outcomes.end(),
        [&](const Outcome& o) { return o.status == status; }));
}

//----------------------------------------------------------------

bool
BetExecutor::Result::anyApplied() const
{
    return count(Status::Placed) + count(Status::Changed) > 0;
}

//----------------------------------------------------------------

std::string
BetExecutor::Result::headline() const
{
    std::string s;
    auto add = [&](std::size_t n, const char* what)
    {
        if (n == 0) return;
        if (!s.empty()) s += ", ";
        s += std::to_string(n) + " " + what;
    };
    add(count(Status::Placed),         "placed");
    add(count(Status::Changed),        "changed");
    add(count(Status::AlreadyInPlace), "already in place");
    add(count(Status::Skipped),        "skipped");
    return s.empty() ? "nothing to do" : s;
}

//----------------------------------------------------------------

std::string
BetExecutor::Result::skippedDetail() const
{
    std::string s;
    for (const auto& o : outcomes)
    {
        if (o.status != Status::Skipped) continue;
        if (!s.empty()) s += "\n";
        s += describe(o.spec) + ": " + o.reason;
    }
    return s;
}

//----------------------------------------------------------------

BetExecutor::Result
BetExecutor::apply(Craps::Player& player,
                   const std::vector<BetSpec>& specs,
                   LastFill* lastFill)
{
    Result result;
    UndoManager& undo = UndoManager::instance();
    const Craps::PlayerId playerId = player.getPlayerId();

    // Contract bets first so that odds have something to sit on
    std::vector<BetSpec> ordered(specs);
    std::stable_partition(ordered.begin(), ordered.end(),
        [](const BetSpec& s) { return !s.isOdds; });

    bool pushedAny = false;     // Later changes join the first one's step

    auto remember = [&](const Craps::BetPtr& bet, bool isOdds)
    {
        if (lastFill == nullptr) return;
        LastFill::LastFillEntry lfe;
        lfe.betName   = bet->betName();
        lfe.pivot     = bet->pivot();
        lfe.isOddsBet = isOdds;
        lfe.amount    = isOdds ? bet->oddsAmount() : bet->contractAmount();
        lastFill->setLastFill(lfe);
    };

    auto record = [&](std::unique_ptr<UndoBet> step)
    {
        undo.push(playerId, std::move(step), pushedAny);
        pushedAny = true;
    };

    for (const auto& spec : ordered)
    {
        Outcome out;
        out.spec = spec;

        if (spec.betName == BetName::Invalid || spec.amount == 0)
        {
            out.reason = "Not a usable bet.";
            result.outcomes.push_back(std::move(out));
            continue;
        }

        if (!spec.isOdds)
        {
            // Remember how an existing bet was before it changes
            std::unique_ptr<UndoBet> undoChange;
            if (auto existing = player.findBet(spec.betName, spec.pivot))
            {
                undoChange = std::make_unique<UndoBetModifiedAmount>(existing);
            }

            auto r = player.makeOrChangeBet(spec.betName, spec.amount, spec.pivot);
            if (!r)
            {
                out.reason = r.error().description();
            }
            else
            {
                const auto placed = r.value();
                if (!placed.changed)
                {
                    out.status = Status::Placed;
                    record(std::make_unique<UndoBetAdded>(placed.bet));
                    remember(placed.bet, false);
                }
                else if (placed.previousAmount == spec.amount)
                {
                    out.status = Status::AlreadyInPlace;
                }
                else
                {
                    out.status = Status::Changed;
                    if (undoChange) record(std::move(undoChange));
                    remember(placed.bet, false);
                }
            }
        }
        else
        {
            auto bet = player.findBet(spec.betName, spec.pivot);
            if (!bet)
            {
                out.reason = "You have no " + EnumBetName::toString(spec.betName) +
                             " bet to put odds on.";
            }
            else if (bet->oddsAmount() == spec.amount)
            {
                out.status = Status::AlreadyInPlace;
            }
            else
            {
                auto undoChange = std::make_unique<UndoBetModifiedAmount>(bet);
                const bool hadOdds = bet->oddsAmount() != 0;
                auto r = player.setOddsAmount(bet, spec.amount);
                if (!r)
                {
                    out.reason = r.error().description();
                }
                else
                {
                    out.status = hadOdds ? Status::Changed : Status::Placed;
                    record(std::move(undoChange));
                    remember(bet, true);
                }
            }
        }

        result.outcomes.push_back(std::move(out));
    }
    return result;
}

//----------------------------------------------------------------
