//----------------------------------------------------------------
// File: TableRules.cpp
//----------------------------------------------------------------

#include <craps/TableRules.h>
#include <stdexcept>

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

} // namespace Craps
