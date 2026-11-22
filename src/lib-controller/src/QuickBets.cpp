//----------------------------------------------------------------
//
// File: QuickBets.cpp
//
//----------------------------------------------------------------

#include <controller/QuickBets.h>
#include <gen/Logger.h>
#include <algorithm>
#include <cctype>
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

std::string
QuickBets::checkName(const std::string& name)
{
    if (name.empty())
    {
        return "Give the entry a name.";
    }
    if (name.size() > MaxNameLength)
    {
        return "The name can have at most " + std::to_string(MaxNameLength) +
               " characters.";
    }
    return std::string();
}

//----------------------------------------------------------------

std::string
QuickBets::checkSpec(const BetSpec& spec)
{
    if (spec.betName == BetName::Invalid || spec.betName == BetName::Count)
    {
        return "That is not a bet.";
    }
    if (spec.amount == 0)
    {
        return "The amount must be more than zero.";
    }
    if (spec.isOdds && !isPlanOddsBet(spec.betName))
    {
        return "A QuickBets entry can hold odds only for Pass Line and "
               "Don't Pass bets.";
    }
    return std::string();
}

//----------------------------------------------------------------

namespace {

bool
sameNameIgnoringCase(const std::string& a, const std::string& b)
{
    return a.size() == b.size() &&
           std::equal(a.begin(), a.end(), b.begin(),
                      [](unsigned char x, unsigned char y)
                      { return std::tolower(x) == std::tolower(y); });
}

} // namespace

//----------------------------------------------------------------

std::string
QuickBets::create(const std::string& name, const BetSpec& spec)
{
    if (auto why = checkName(name); !why.empty())  return why;
    if (auto why = checkSpec(spec); !why.empty())  return why;

    for (const auto& p : plans_)
    {
        if (sameNameIgnoringCase(p.name, name))
        {
            return "A QuickBets entry named " + p.name + " already exists.";
        }
    }
    if (plans_.size() >= MaxEntries)
    {
        return "You already have " + std::to_string(MaxEntries) +
               " QuickBets entries. Delete one first.";
    }

    BetPlan plan;
    plan.name = name;
    plan.specs.push_back(spec);
    plans_.push_back(std::move(plan));
    return std::string();
}

//----------------------------------------------------------------

std::string
QuickBets::addBet(const std::string& name, const BetSpec& spec, bool& replaced)
{
    replaced = false;
    if (auto why = checkSpec(spec); !why.empty())  return why;

    auto it = std::find_if(plans_.begin(), plans_.end(),
        [&](const BetPlan& p) { return p.name == name; });
    if (it == plans_.end())
    {
        return "There is no QuickBets entry named " + name + ".";
    }

    auto same = std::find_if(it->specs.begin(), it->specs.end(),
        [&](const BetSpec& s) { return s.sameSpot(spec); });
    if (same != it->specs.end())
    {
        same->amount = spec.amount;
        replaced = true;
        return std::string();
    }
    if (it->specs.size() >= MaxSpecsPerEntry)
    {
        return "The entry " + name + " already holds " +
               std::to_string(MaxSpecsPerEntry) + " bets, the most it can.";
    }
    it->specs.push_back(spec);
    *it = tidy(std::move(*it));        // Contract bets stay before odds
    return std::string();
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
