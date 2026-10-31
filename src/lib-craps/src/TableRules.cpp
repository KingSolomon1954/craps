//----------------------------------------------------------------
// File: TableRules.cpp
//----------------------------------------------------------------

#include <craps/TableRules.h>
#include <initializer_list>
#include <stdexcept>
#include <string>

namespace Craps {

TableRules::TableRules()
{
    // Preserve the defaults used by the previous CrapsTable implementation.
    for (auto& limit : wagers)
        limit = {5, 1000, 1};

    wagerLimit(BetName::Field) = {1, 1000, 1};
    wagerLimit(BetName::CandE)  = {2, 1000, 2};
    wagerLimit(BetName::Horn)   = {4, 1000, 4};
    wagerLimit(BetName::World)  = {5, 1000, 5};

    fieldPayouts[2]  = {2, 1};
    fieldPayouts[3]  = {1, 1};
    fieldPayouts[4]  = {1, 1};
    fieldPayouts[9]  = {1, 1};
    fieldPayouts[10] = {1, 1};
    fieldPayouts[11] = {1, 1};
    fieldPayouts[12] = {3, 1};

    buyCommission.basis = CommissionBasis::OnWager;
    layCommission.basis = CommissionBasis::OnWin;

    // 3-4-5x odds schedule is available as a named alternative.  The
    // default remains the historical flat 5x table rule.
    odds.pointMultipliers.fill(0);
    odds.pointMultipliers[4] = 3;
    odds.pointMultipliers[5] = 4;
    odds.pointMultipliers[6] = 5;
    odds.pointMultipliers[8] = 5;
    odds.pointMultipliers[9] = 4;
    odds.pointMultipliers[10] = 3;
}

const TableRules::WagerLimit& TableRules::wagerLimit(BetName name) const
{
    const auto index = static_cast<std::size_t>(name);
    if (index >= wagers.size())
        throw std::out_of_range("Invalid bet name in TableRules::wagerLimit");
    return wagers[index];
}

TableRules::WagerLimit& TableRules::wagerLimit(BetName name)
{
    const auto index = static_cast<std::size_t>(name);
    if (index >= wagers.size())
        throw std::out_of_range("Invalid bet name in TableRules::wagerLimit");
    return wagers[index];
}


YAML::Node TableRules::toYAML() const
{
    YAML::Node node;
    YAML::Node wagerNode;
    for (const auto name : EnumBetName::enumerators)
    {
        const auto& limit = wagerLimit(name);
        YAML::Node entry;
        entry["minimum"] = limit.minimum;
        entry["maximum"] = limit.maximum;
        entry["increment"] = limit.increment;
        wagerNode[EnumBetName::toString(name)] = entry;
    }
    node["wagers"] = wagerNode;

    YAML::Node oddsNode;
    oddsNode["style"] = odds.style == OddsLimitStyle::FlatMultiplier
        ? "flatMultiplier" : "pointSchedule";
    oddsNode["flatMultiplier"] = odds.flatMultiplier;
    oddsNode["pointMultipliers"] = YAML::Node(YAML::NodeType::Sequence);
    for (const auto multiplier : odds.pointMultipliers)
        oddsNode["pointMultipliers"].push_back(multiplier);
    oddsNode["maximumWager"] = odds.maximumWager;
    oddsNode["maximumPotentialWin"] = odds.maximumPotentialWin;
    oddsNode["rounding"] = odds.rounding == PayoutRounding::Down ? "down" : "exact";
    oddsNode["payoutUnit"] = odds.payoutUnit;
    node["odds"] = oddsNode;

    node["payoutRounding"] = payoutRounding == PayoutRounding::Down ? "down" : "exact";
    node["payoutUnit"] = payoutUnit;
    YAML::Node fieldNode;
    for (std::size_t total = 0; total < fieldPayouts.size(); ++total)
    {
        YAML::Node payout;
        payout["numerator"] = fieldPayouts[total].numerator;
        payout["denominator"] = fieldPayouts[total].denominator;
        fieldNode[std::to_string(total)] = payout;
    }
    node["fieldPayouts"] = fieldNode;

    auto commissionToYAML = [](const Commission& commission)
    {
        YAML::Node value;
        value["basisPoints"] = commission.basisPoints;
        value["basis"] = commission.basis == CommissionBasis::OnWager ? "onWager" : "onWin";
        value["collection"] = commission.collection == CommissionCollection::OnPlacement
            ? "onPlacement" : "onWinningDecision";
        value["minimum"] = commission.minimum;
        value["roundingUnit"] = commission.roundingUnit;
        value["rounding"] = commission.rounding == PayoutRounding::Down ? "down" : "exact";
        return value;
    };
    node["buyCommission"] = commissionToYAML(buyCommission);
    node["layCommission"] = commissionToYAML(layCommission);
    node["aggregateLimitBasis"] = aggregateLimitBasis == LimitBasis::AmountWagered
        ? "amountWagered" : "potentialWin";
    node["aggregateMaximumPotentialWin"] = aggregateMaximumPotentialWin;
    node["enforceAggregateComponentLimits"] = enforceAggregateComponentLimits;
    return node;
}

void TableRules::fromYAML(const YAML::Node& node)
{
    if (!node || !node.IsMap())
        throw std::runtime_error("TableRules YAML must be a map");

    if (const auto wagersNode = node["wagers"])
    {
        if (!wagersNode.IsMap())
            throw std::runtime_error("TableRules.wagers must be a map");
        for (const auto name : EnumBetName::enumerators)
        {
            const auto entry = wagersNode[EnumBetName::toString(name)];
            if (!entry) continue;
            auto& limit = wagerLimit(name);
            if (entry["minimum"]) limit.minimum = entry["minimum"].as<Gen::Money>();
            if (entry["maximum"]) limit.maximum = entry["maximum"].as<Gen::Money>();
            if (entry["increment"]) limit.increment = entry["increment"].as<Gen::Money>();
        }
    }

    if (const auto oddsNode = node["odds"])
    {
        if (oddsNode["style"])
        {
            const auto style = oddsNode["style"].as<std::string>();
            if (style == "flatMultiplier") odds.style = OddsLimitStyle::FlatMultiplier;
            else if (style == "pointSchedule") odds.style = OddsLimitStyle::PointSchedule;
            else throw std::runtime_error("Unknown TableRules odds style: " + style);
        }
        if (oddsNode["flatMultiplier"]) odds.flatMultiplier = oddsNode["flatMultiplier"].as<unsigned>();
        if (const auto points = oddsNode["pointMultipliers"])
        {
            if (!points.IsSequence() || points.size() != odds.pointMultipliers.size())
                throw std::runtime_error("odds.pointMultipliers must contain 13 entries");
            for (std::size_t i = 0; i < points.size(); ++i)
                odds.pointMultipliers[i] = points[i].as<unsigned>();
        }
        if (oddsNode["maximumWager"]) odds.maximumWager = oddsNode["maximumWager"].as<Gen::Money>();
        if (oddsNode["maximumPotentialWin"]) odds.maximumPotentialWin = oddsNode["maximumPotentialWin"].as<Gen::Money>();
        if (oddsNode["rounding"])
        {
            const auto value = oddsNode["rounding"].as<std::string>();
            if (value == "down") odds.rounding = PayoutRounding::Down;
            else if (value == "exact") odds.rounding = PayoutRounding::Exact;
            else throw std::runtime_error("Unknown odds rounding: " + value);
        }
        if (oddsNode["payoutUnit"]) odds.payoutUnit = oddsNode["payoutUnit"].as<Gen::Money>();
    }

    auto readRounding = [](const YAML::Node& value, PayoutRounding& target, const char* field)
    {
        if (!value) return;
        const auto text = value.as<std::string>();
        if (text == "down") target = PayoutRounding::Down;
        else if (text == "exact") target = PayoutRounding::Exact;
        else throw std::runtime_error(std::string("Unknown rounding value for ") + field + ": " + text);
    };
    readRounding(node["payoutRounding"], payoutRounding, "payoutRounding");
    if (node["payoutUnit"]) payoutUnit = node["payoutUnit"].as<Gen::Money>();

    if (const auto fieldNode = node["fieldPayouts"])
    {
        for (std::size_t total = 0; total < fieldPayouts.size(); ++total)
        {
            const auto payout = fieldNode[std::to_string(total)];
            if (!payout) continue;
            if (payout["numerator"]) fieldPayouts[total].numerator = payout["numerator"].as<unsigned>();
            if (payout["denominator"]) fieldPayouts[total].denominator = payout["denominator"].as<unsigned>();
        }
    }

    auto readCommission = [&](const YAML::Node& value, Commission& target, const char* label)
    {
        if (!value) return;
        if (value["basisPoints"]) target.basisPoints = value["basisPoints"].as<unsigned>();
        if (value["basis"])
        {
            const auto basis = value["basis"].as<std::string>();
            if (basis == "onWager") target.basis = CommissionBasis::OnWager;
            else if (basis == "onWin") target.basis = CommissionBasis::OnWin;
            else throw std::runtime_error(std::string("Unknown commission basis for ") + label);
        }
        if (value["collection"])
        {
            const auto collection = value["collection"].as<std::string>();
            if (collection == "onPlacement") target.collection = CommissionCollection::OnPlacement;
            else if (collection == "onWinningDecision") target.collection = CommissionCollection::OnWinningDecision;
            else throw std::runtime_error(std::string("Unknown commission collection for ") + label);
        }
        if (value["minimum"]) target.minimum = value["minimum"].as<Gen::Money>();
        if (value["roundingUnit"]) target.roundingUnit = value["roundingUnit"].as<Gen::Money>();
        readRounding(value["rounding"], target.rounding, label);
    };
    readCommission(node["buyCommission"], buyCommission, "buyCommission");
    readCommission(node["layCommission"], layCommission, "layCommission");

    if (node["aggregateLimitBasis"])
    {
        const auto basis = node["aggregateLimitBasis"].as<std::string>();
        if (basis == "amountWagered") aggregateLimitBasis = LimitBasis::AmountWagered;
        else if (basis == "potentialWin") aggregateLimitBasis = LimitBasis::PotentialWin;
        else throw std::runtime_error("Unknown aggregateLimitBasis: " + basis);
    }
    if (node["aggregateMaximumPotentialWin"])
        aggregateMaximumPotentialWin = node["aggregateMaximumPotentialWin"].as<Gen::Money>();
    if (node["enforceAggregateComponentLimits"])
        enforceAggregateComponentLimits = node["enforceAggregateComponentLimits"].as<bool>();
    validate();
}

void TableRules::validate() const
{
    auto validUnit = [](Gen::Money value) { return value > 0; };
    for (const auto name : EnumBetName::enumerators)
    {
        const auto& limit = wagerLimit(name);
        if (limit.maximum < limit.minimum || !validUnit(limit.increment))
            throw std::runtime_error("Invalid wager limits for " + EnumBetName::toString(name));
    }
    if (!validUnit(payoutUnit) || !validUnit(odds.payoutUnit) ||
        !validUnit(buyCommission.roundingUnit) || !validUnit(layCommission.roundingUnit))
        throw std::runtime_error("TableRules payout and commission units must be positive");
    if (buyCommission.collection != CommissionCollection::OnWinningDecision ||
        layCommission.collection != CommissionCollection::OnWinningDecision)
        throw std::runtime_error("Commission collection on placement is not supported yet");
    if (odds.style == OddsLimitStyle::FlatMultiplier && odds.flatMultiplier == 0)
        throw std::runtime_error("Flat odds multiplier must be greater than zero");
    if (odds.style == OddsLimitStyle::PointSchedule)
    {
        for (const unsigned point : {4u, 5u, 6u, 8u, 9u, 10u})
            if (odds.pointMultipliers[point] == 0)
                throw std::runtime_error("Point-schedule odds multiplier must be greater than zero for point " +
                                         std::to_string(point));
    }
    for (const auto& payout : fieldPayouts)
        if (payout.denominator == 0)
            throw std::runtime_error("Field payout denominator must be greater than zero");
}

} // namespace Craps
