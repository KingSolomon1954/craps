//----------------------------------------------------------------
//
// File: QuickBets.cpp
//
//----------------------------------------------------------------

#include <controller/QuickBets.h>
#include <gen/Logger.h>
#include <algorithm>
#include <exception>

using namespace Ctrl;

//----------------------------------------------------------------

BetPlan
QuickBets::tidy(BetPlan plan)
{
    std::vector<BetSpec> kept;
    for (const auto& s : plan.specs)
    {
        if (s.betName == BetName::Invalid || s.betName == BetName::Count ||
            s.amount == 0 ||
            (s.isOdds && !isPlanOddsBet(s.betName)))
        {
            continue;
        }

        auto same = std::find_if(kept.begin(), kept.end(),
            [&](const BetSpec& k) { return k.sameSpot(s); });
        if (same != kept.end())
        {
            same->amount = s.amount;     // Later one wins, earlier place
        }
        else
        {
            kept.push_back(s);
        }
    }

    // Contract bets before the odds on them
    std::stable_partition(kept.begin(), kept.end(),
        [](const BetSpec& s) { return !s.isOdds; });

    if (kept.size() > MaxSpecsPerEntry)
    {
        kept.resize(MaxSpecsPerEntry);
    }
    plan.specs = std::move(kept);
    return plan;
}

//----------------------------------------------------------------

bool
QuickBets::set(BetPlan plan)
{
    if (plan.name.empty())
    {
        return false;
    }
    plan = tidy(std::move(plan));
    if (plan.specs.empty())
    {
        return false;
    }

    auto it = std::find_if(plans_.begin(), plans_.end(),
        [&](const BetPlan& p) { return p.name == plan.name; });
    if (it != plans_.end())
    {
        *it = std::move(plan);
        return true;
    }
    if (plans_.size() >= MaxEntries)
    {
        return false;
    }
    plans_.push_back(std::move(plan));
    return true;
}

//----------------------------------------------------------------

bool
QuickBets::remove(std::size_t index)
{
    if (index >= plans_.size())
    {
        return false;
    }
    plans_.erase(plans_.begin() + static_cast<std::ptrdiff_t>(index));
    return true;
}

//----------------------------------------------------------------

void
QuickBets::clear()
{
    plans_.clear();
}

//----------------------------------------------------------------

const std::vector<BetPlan>&
QuickBets::plans() const
{
    return plans_;
}

//----------------------------------------------------------------

const BetPlan*
QuickBets::find(const std::string& name) const
{
    for (const auto& p : plans_)
    {
        if (p.name == name) return &p;
    }
    return nullptr;
}

//----------------------------------------------------------------

YAML::Node
QuickBets::toYAML() const
{
    YAML::Node seq(YAML::NodeType::Sequence);
    for (const auto& p : plans_)
    {
        YAML::Node bets(YAML::NodeType::Sequence);
        for (const auto& s : p.specs)
        {
            YAML::Node b;
            b["betName"] = EnumBetName::toString(s.betName);
            b["pivot"]   = s.pivot;
            if (s.isOdds)
            {
                b["isOdds"] = true;
            }
            b["amount"] = s.amount;
            bets.push_back(b);
        }
        YAML::Node n;
        n["name"] = p.name;
        n["bets"] = bets;
        seq.push_back(n);
    }
    return seq;
}

//----------------------------------------------------------------
//
// Replaces current contents. Never throws.
//
void
QuickBets::fromYAML(const YAML::Node& node)
{
    plans_.clear();
    if (!node.IsDefined() || node.IsNull())
    {
        return;
    }
    if (!node.IsSequence())
    {
        LOG_WARN("QuickBets: not a sequence; ignoring it");
        return;
    }

    for (const auto& n : node)
    {
        try
        {
            BetPlan plan;
            plan.name = n["name"].as<std::string>();
            for (const auto& b : n["bets"])
            {
                BetSpec s;
                s.betName = EnumBetName::fromString(b["betName"].as<std::string>());
                s.pivot   = b["pivot"]  ? b["pivot"].as<unsigned>()  : 0u;
                s.isOdds  = b["isOdds"] ? b["isOdds"].as<bool>()     : false;
                s.amount  = b["amount"].as<Gen::Money>();
                plan.specs.push_back(s);
            }
            if (!set(std::move(plan)))
            {
                LOG_WARN("QuickBets: skipping an entry that has no name, "
                         "no usable bets, or is over the limit");
            }
        }
        catch (const std::exception& ex)
        {
            LOG_WARN(std::string("QuickBets: skipping bad entry: ") + ex.what());
        }
    }
}

//----------------------------------------------------------------
