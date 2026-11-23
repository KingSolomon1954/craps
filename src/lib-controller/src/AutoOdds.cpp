//----------------------------------------------------------------
//
// File: AutoOdds.cpp
//
//----------------------------------------------------------------

#include <controller/AutoOdds.h>
#include <gen/Logger.h>
#include <algorithm>
#include <cctype>

using namespace Ctrl;

//----------------------------------------------------------------

bool
AutoOdds::isOddsBetName(BetName betName)
{
    return betName == BetName::PassLine || betName == BetName::DontPass ||
           betName == BetName::Come     || betName == BetName::DontCome;
}

//----------------------------------------------------------------

bool
AutoOdds::isPoint(unsigned point)
{
    return point == 4 || point == 5 || point == 6 ||
           point == 8 || point == 9 || point == 10;
}

//----------------------------------------------------------------

const std::vector<BetName>&
AutoOdds::oddsBetNames()
{
    static const std::vector<BetName> names{
        BetName::PassLine, BetName::DontPass, BetName::Come, BetName::DontCome};
    return names;
}

//----------------------------------------------------------------

const std::vector<unsigned>&
AutoOdds::points()
{
    static const std::vector<unsigned> pts{4, 5, 6, 8, 9, 10};
    return pts;
}

//----------------------------------------------------------------

std::string
AutoOdds::Setting::toString() const
{
    if (amount != 0)   return "$" + std::to_string(amount);
    if (multiple != 0) return std::to_string(multiple) + "x";
    return "none";
}

//----------------------------------------------------------------

Gen::Money
AutoOdds::oddsAmount(const Setting& setting, Gen::Money contract,
                     const Craps::BetAmountLimits& limits)
{
    Gen::Money amount = 0;
    if (setting.isAmount())
    {
        amount = setting.amount;
    }
    else if (setting.multiple != 0 && contract != 0)
    {
        amount = static_cast<Gen::Money>(setting.multiple) * contract;
    }
    if (amount == 0)
    {
        return 0;
    }
    if (limits.maximum > 0 && amount > limits.maximum)
    {
        amount = limits.maximum;
    }
    const Gen::Money inc = (limits.increment == 0) ? 1 : limits.increment;
    return amount - (amount % inc);
}

//----------------------------------------------------------------

std::string
AutoOdds::parseSetting(const std::string& raw, Setting& setting)
{
    std::string text;
    for (char c : raw)
    {
        if (!std::isspace(static_cast<unsigned char>(c))) text += c;
    }
    setting = Setting{};

    const bool isMultiple = !text.empty() && (text.back() == 'x' || text.back() == 'X');
    if (isMultiple) text.pop_back();
    else if (!text.empty() && text.front() == '$') text.erase(0, 1);

    if (text.empty() || text.size() > 9 ||
        !std::all_of(text.begin(), text.end(),
                     [](unsigned char c) { return std::isdigit(c); }))
    {
        return "Enter an amount like 50 or $50, a multiple like 3x, or 0.";
    }

    const unsigned long value = std::stoul(text);
    if (isMultiple)
    {
        if (value > MaxMultiple)
        {
            return "The multiple can be at most " + std::to_string(MaxMultiple) + "x.";
        }
        setting.multiple = static_cast<unsigned>(value);
    }
    else
    {
        setting.amount = static_cast<Gen::Money>(value);
    }
    return "";
}

//----------------------------------------------------------------

void
AutoOdds::setEnabled(bool enabled)
{
    enabled_ = enabled;
}

//----------------------------------------------------------------

bool
AutoOdds::enabled() const
{
    return enabled_;
}

//----------------------------------------------------------------

std::string
AutoOdds::set(BetName betName, unsigned point, const Setting& setting)
{
    if (!isOddsBetName(betName))
    {
        return EnumBetName::toString(betName) + " cannot have odds.";
    }
    if (point != 0 && !isPoint(point))
    {
        return std::to_string(point) + " is not a point.";
    }
    if (setting.multiple > MaxMultiple)
    {
        return "The multiple can be at most " + std::to_string(MaxMultiple) + "x.";
    }
    if (setting.multiple != 0 && setting.amount != 0)
    {
        return "A setting is a multiple or an amount, not both.";
    }

    for (unsigned pt : points())
    {
        if (point != 0 && pt != point) continue;
        if (setting.isSet())
            settings_[{betName, pt}] = setting;
        else
            settings_.erase({betName, pt});
    }
    return "";
}

//----------------------------------------------------------------

std::string
AutoOdds::setMultiple(BetName betName, unsigned point, unsigned multiple)
{
    Setting s;
    s.multiple = multiple;
    return set(betName, point, s);
}

//----------------------------------------------------------------

std::string
AutoOdds::setAmount(BetName betName, unsigned point, Gen::Money amount)
{
    Setting s;
    s.amount = amount;
    return set(betName, point, s);
}

//----------------------------------------------------------------

void
AutoOdds::setPreset345()
{
    for (BetName b : oddsBetNames())
    {
        setMultiple(b, 4, 3);
        setMultiple(b, 10, 3);
        setMultiple(b, 5, 4);
        setMultiple(b, 9, 4);
        setMultiple(b, 6, 5);
        setMultiple(b, 8, 5);
    }
}

//----------------------------------------------------------------

void
AutoOdds::clearAll()
{
    settings_.clear();
}

//----------------------------------------------------------------

AutoOdds::Setting
AutoOdds::setting(BetName betName, unsigned point) const
{
    auto it = settings_.find({betName, point});
    return (it == settings_.end()) ? Setting{} : it->second;
}

//----------------------------------------------------------------

unsigned
AutoOdds::multiple(BetName betName, unsigned point) const
{
    return setting(betName, point).multiple;
}

//----------------------------------------------------------------

Gen::Money
AutoOdds::amount(BetName betName, unsigned point) const
{
    return setting(betName, point).amount;
}

//----------------------------------------------------------------

bool
AutoOdds::anySet() const
{
    return !settings_.empty();
}

//----------------------------------------------------------------

std::string
AutoOdds::summary(BetName betName) const
{
    std::string s;
    for (unsigned point : points())
    {
        const Setting st = setting(betName, point);
        if (!st.isSet()) continue;
        if (!s.empty()) s += " ";
        s += std::to_string(point) + ":" + st.toString();
    }
    return s.empty() ? "none" : s;
}

//----------------------------------------------------------------

void
AutoOdds::forgetHandled()
{
    handled_.clear();
}

//----------------------------------------------------------------

AutoOdds::Plan
AutoOdds::plan(const std::vector<Candidate>& candidates)
{
    Plan result;

    // Drop bets that are gone
    std::set<unsigned> present;
    for (const auto& c : candidates)
    {
        present.insert(c.betId);
    }
    for (auto it = handled_.begin(); it != handled_.end();)
    {
        it = present.count(*it) ? std::next(it) : handled_.erase(it);
    }

    for (const auto& c : candidates)
    {
        if (!isOddsBetName(c.betName) || !isPoint(c.pivot) || c.contract == 0)
        {
            continue;
        }
        if (handled_.count(c.betId))
        {
            continue;
        }
        const Setting st = setting(c.betName, c.pivot);
        if (!st.isSet())
        {
            continue;
        }

        handled_.insert(c.betId);       // handled, whatever happens next
        if (c.odds != 0)
        {
            continue;                   // the player already dealt with it
        }

        BetSpec spec;
        spec.betName = c.betName;
        spec.pivot   = (c.betName == BetName::PassLine ||
                        c.betName == BetName::DontPass) ? 0u : c.pivot;
        spec.isOdds  = true;
        spec.amount  = oddsAmount(st, c.contract, c.limits);

        if (spec.amount == 0)
        {
            const Gen::Money inc = (c.limits.increment == 0) ? 1 : c.limits.increment;
            result.skipped.push_back(
                EnumBetName::toString(c.betName) + " " + std::to_string(c.pivot) +
                " odds: " + st.toString() + " is less than the table's $" +
                std::to_string(inc) + " odds increment.");
            continue;
        }
        result.specs.push_back(spec);
    }
    return result;
}

//----------------------------------------------------------------

YAML::Node
AutoOdds::toYAML() const
{
    YAML::Node node(YAML::NodeType::Map);
    node["enabled"] = enabled_;

    YAML::Node mult(YAML::NodeType::Map);
    for (BetName b : oddsBetNames())
    {
        YAML::Node byPoint(YAML::NodeType::Map);
        for (unsigned point : points())
        {
            const Setting st = setting(b, point);
            if (st.isSet())
            {
                byPoint[std::to_string(point)] = st.toString();
            }
        }
        if (byPoint.size() > 0)
        {
            mult[EnumBetName::toString(b)] = byPoint;
        }
    }
    node["multiples"] = mult;
    return node;
}

//----------------------------------------------------------------

void
AutoOdds::fromYAML(const YAML::Node& node)
{
    enabled_ = false;
    settings_.clear();
    handled_.clear();

    if (!node.IsDefined() || node.IsNull())
    {
        return;
    }
    if (!node.IsMap())
    {
        LOG_WARN("AutoOdds: not a map; ignoring it");
        return;
    }

    try
    {
        if (node["enabled"])
        {
            enabled_ = node["enabled"].as<bool>();
        }
    }
    catch (const std::exception&)
    {
        LOG_WARN("AutoOdds: bad 'enabled'; leaving it off");
    }

    const YAML::Node mult = node["multiples"];
    if (!mult.IsDefined() || !mult.IsMap())
    {
        return;
    }
    for (const auto& kv : mult)
    {
        try
        {
            const BetName b = EnumBetName::fromString(kv.first.as<std::string>());
            if (!isOddsBetName(b) || !kv.second.IsMap())
            {
                LOG_WARN("AutoOdds: skipping an unusable bet type");
                continue;
            }
            for (const auto& pm : kv.second)
            {
                // "3x" or "$50"; a plain number is an old style multiple
                std::string text = pm.second.as<std::string>();
                if (!text.empty() && text.front() != '$' &&
                    std::isdigit(static_cast<unsigned char>(text.back())))
                {
                    text += "x";
                }
                Setting st;
                const unsigned point = pm.first.as<unsigned>();
                std::string why = isPoint(point) ? parseSetting(text, st)
                                                 : "not a point.";
                if (why.empty())
                {
                    why = set(b, point, st);
                }
                if (!why.empty())
                {
                    LOG_WARN("AutoOdds: skipping a setting: " + why);
                }
            }
        }
        catch (const std::exception&)
        {
            LOG_WARN("AutoOdds: skipping an unreadable entry");
        }
    }
}

//----------------------------------------------------------------
