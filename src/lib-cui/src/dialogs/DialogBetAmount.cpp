//----------------------------------------------------------------
//
// File: DialogBetAmount.cpp
//
//----------------------------------------------------------------

#include <cui/dialogs/DialogBetAmount.h>
#include <cui/core/CuiUtils.h>
#include <cui/layouts/LayoutConsole.h>
#include <cui/managers/CarrierBet.h>
#include <cui/managers/OperationManager.h>
#include <cui/managers/SurfaceManager.h>
#include <controller/AutoFill.h>
#include <controller/LastFill.h>
#include <controller/CrapsReaders.h>
#include <craps/EnumBetName.h>
#include <craps/PlayerStats.h>
#include <craps/TableStats.h>
#include <craps/BetStats.h>
#include <fmt/format.h>
#include <gen/Logger.h>
#include <ncurses.h>
#include <array>
#include <limits>
#include <map>
#include <stdexcept>
#include <string_view>

using namespace Cui;

//----------------------------------------------------------------

namespace
{
constexpr int ContentWidth = 56;

std::string
padLine(std::string text)
{
    if (text.size() > ContentWidth)
        text.resize(ContentWidth);
    text.resize(ContentWidth, ' ');
    return text;
}

//----------------------------------------------------------------

std::string
payoutLabel(std::string_view referenceLine)
{
    if (!referenceLine.starts_with("Pays "))
        return {};

    std::size_t end = referenceLine.size();
    for (const std::string_view marker :
         {" True Odds", ", True Odds", ". House Edge", " House Edge"})
    {
        const auto pos = referenceLine.find(marker);
        if (pos != std::string_view::npos)
            end = std::min(end, pos);
    }
    return std::string(referenceLine.substr(0, end));
}

//----------------------------------------------------------------

std::string
referencePayoutDetails(std::string_view referenceLine)
{
    const std::string label = payoutLabel(referenceLine);
    if (label.empty())
        return std::string(referenceLine);

    const auto pos = referenceLine.find(label);
    std::string details(referenceLine.substr(pos + label.size()));
    const auto first = details.find_first_not_of(" .,:");
    if (first == std::string::npos)
        return {};
    details.erase(0, first);
    return details;
}

//----------------------------------------------------------------

std::string
money(Gen::Money amount)
{
    return Gen::MoneyUtils::toString(amount);
}

//----------------------------------------------------------------

std::string
expectedMoney(double amount)
{
    const auto cents     = static_cast<long long>(amount * 100.0 + 0.5);
    const auto dollars   = static_cast<Gen::Money>(cents / 100);
    const auto remainder = static_cast<unsigned>(cents % 100);

    return fmt::format("{}.{:02}", money(dollars), remainder);
}

//----------------------------------------------------------------

std::string
signedMoney(Gen::Money wins, Gen::Money losses)
{
    const long long net = static_cast<long long>(wins) -
                          static_cast<long long>(losses);
    const auto magnitude = static_cast<Gen::Money>(net < 0 ? -net : net);
    return std::string(net < 0 ? "-" : "+") + money(magnitude);
}

//----------------------------------------------------------------

std::string percent(unsigned wins, unsigned losses)
{
    const double total =
        static_cast<double>(wins) + static_cast<double>(losses);

    if (total == 0.0)
        return "--.-%";

    return fmt::format("{:.1f}%", 100.0 * wins / total);
}

//----------------------------------------------------------------

std::string
betStatsKey(BetName betName, unsigned pivot)
{
    std::string key = EnumBetName::toString(betName);
    if (pivot != 0)
        key += std::to_string(pivot);
    return key;
}

//----------------------------------------------------------------

Craps::BetStat
findBetStat(const std::map<std::string, Craps::BetStat>& stats,
            const std::string& key,
            bool aggregateByPrefix)
{
    Craps::BetStat result;
    if (!aggregateByPrefix)
    {
        if (const auto it = stats.find(key); it != stats.end())
            result = it->second;
        return result;
    }

    for (const auto& [name, stat] : stats)
    {
        if (name.starts_with(key))
            result.merge(stat);
    }
    return result;
}

//----------------------------------------------------------------

bool
isRollOutcomeBet(BetName name)
{
    return name == BetName::Field    || name == BetName::AnyCraps ||
           name == BetName::AnySeven || name == BetName::CandE    ||
           name == BetName::Horn     || name == BetName::World;
}

struct SimulatedBetCounts
{
    unsigned wins   = 0;
    unsigned losses = 0;
};

//----------------------------------------------------------------

SimulatedBetCounts
simulatedBetCounts(const Craps::RollStats& stats,
                   BetName betName,
                   unsigned pivot)
{
    SimulatedBetCounts result;

    const auto addPointCounts = [&](const Craps::PointCounts& counts)
    {
        result.wins   += counts.wins.count();
        result.losses += counts.lose.count();
    };

    const auto addAllPointCounts =
        [&](const std::array<Craps::PointCounts, 11>& counts)
    {
        for (unsigned point : {4U, 5U, 6U, 8U, 9U, 10U})
            addPointCounts(counts[point]);
    };

    const auto countByNumber = [&](auto isWin)
    {
        for (unsigned number = 2; number <= 12; ++number)
        {
            const unsigned count = stats.numberCounts[number].count();

            if (isWin(number))
                result.wins += count;
            else
                result.losses += count;
        }
    };

    switch (betName)
    {
    case BetName::PassLine:
        if (pivot != 0)
        {
            if (pivot < stats.passPntCnts.size())
                addPointCounts(stats.passPntCnts[pivot]);
        }
        else
        {
            result.wins   += stats.passWinsComeOut.count();
            result.losses += stats.passLoseComeOut.count();
            addAllPointCounts(stats.passPntCnts);
        }
        break;

    case BetName::DontPass:
        if (pivot != 0)
        {
            if (pivot < stats.dontPassPntCnts.size())
                addPointCounts(stats.dontPassPntCnts[pivot]);
        }
        else
        {
            result.wins   += stats.dontPassWinsComeOut.count();
            result.losses += stats.dontPassLoseComeOut.count();
            addAllPointCounts(stats.dontPassPntCnts);
        }
        break;

    case BetName::Come:
        if (pivot != 0)
        {
            if (pivot < stats.comePntCnts.size())
                addPointCounts(stats.comePntCnts[pivot]);
        }
        else
        {
            result.wins   += stats.comeWinsComeOut.count();
            result.losses += stats.comeLoseComeOut.count();
            addAllPointCounts(stats.comePntCnts);
        }
        break;

    case BetName::DontCome:
        if (pivot != 0)
        {
            if (pivot < stats.dontComePntCnts.size())
                addPointCounts(stats.dontComePntCnts[pivot]);
        }
        else
        {
            result.wins   += stats.dontComeWinsComeOut.count();
            result.losses += stats.dontComeLoseComeOut.count();
            addAllPointCounts(stats.dontComePntCnts);
        }
        break;

    case BetName::Place:
    case BetName::Buy:
    case BetName::Lay:
        if (pivot >= 2 && pivot <= 12 && pivot != 7)
        {
            result.wins   = stats.numberCounts[pivot].count();
            result.losses = stats.numberCounts[7].count();
        }
        break;

    case BetName::Hardway:
        if (pivot < stats.hardwayCounts.size())
            addPointCounts(stats.hardwayCounts[pivot]);
        break;

    case BetName::Field:
        result.wins   = stats.fieldBetWins.count();
        result.losses = stats.fieldBetLose.count();
        break;

    case BetName::AnyCraps:
        countByNumber([](unsigned n)
        {
            return n == 2 || n == 3 || n == 12;
        });
        break;

    case BetName::AnySeven:
        countByNumber([](unsigned n)
        {
            return n == 7;
        });
        break;

    case BetName::CandE:
    case BetName::Horn:
        countByNumber([](unsigned n)
        {
            return n == 2 || n == 3 || n == 11 || n == 12;
        });
        break;

    case BetName::World:
        countByNumber([](unsigned n)
        {
            return n == 2 || n == 3 || n == 7 || n == 11 || n == 12;
        });
        break;

    default:
        break;
    }

    return result;
}

//----------------------------------------------------------------

struct BetProfile
{
    double houseEdgePct = -1.0;
    double winPct = -1.0;       // Win percentage among resolved bets.
    unsigned payoutNum = 0;
    unsigned payoutDen = 0;
    std::string_view loses;
    std::string_view wins;
};

//----------------------------------------------------------------

BetProfile
profileFor(BetName name, unsigned pivot)
{
    BetProfile p;
    if (name == BetName::Place)
    {
        p.wins = "Wins on the point";
        p.loses = "Loses on 7";
        switch (pivot)
        {
        case 4: case 10: p = {6.6667, 33.3333, 9, 5, "Loses on 7", "Wins on point"}; break;
        case 5: case 9:  p = {4.0000, 40.0000, 7, 5, "Loses on 7", "Wins on point"}; break;
        case 6: case 8:  p = {1.5152, 45.4545, 7, 6, "Loses on 7", "Wins on point"}; break;
        }
    }
    else if (name == BetName::Buy)
    {
        switch (pivot)
        {
        case 4: case 10: p = {1.6667, 33.3333, 2, 1, "Loses on 7", "Wins on point"}; break;
        case 5: case 9:  p = {2.0000, 40.0000, 3, 2, "Loses on 7", "Wins on point"}; break;
        case 6: case 8:  p = {2.2727, 45.4545, 6, 5, "Loses on 7", "Wins on point"}; break;
        }
    }
    else if (name == BetName::Lay)
    {
        switch (pivot)
        {
        case 4: case 10: p = {1.6667, 66.6667, 1, 2, "Wins on 7", "Loses on point"}; break;
        case 5: case 9:  p = {2.0000, 60.0000, 2, 3, "Wins on 7", "Loses on point"}; break;
        case 6: case 8:  p = {2.2727, 54.5455, 5, 6, "Wins on 7", "Loses on point"}; break;
        }
    }
    else if (name == BetName::PassLine || name == BetName::Come)
        p = {1.4141, 49.2929, 1, 1, "Loses on 7 or craps", "Wins on 7 or point"};
    else if (name == BetName::DontPass || name == BetName::DontCome)
        p = {1.3636, 49.2929, 1, 1, "Loses on point", "Wins on 7"};
    else if (name == BetName::Hardway)
    {
        if (pivot == 4 || pivot == 10)
            p = {11.1111, 11.1111, 7, 1, "Loses on 7 or easy way", "Wins on hard way"};
        else if (pivot == 6 || pivot == 8)
            p = {9.0909, 16.6667, 9, 1, "Loses on 7 or easy way", "Wins on hard way"};
    }
    else if (name == BetName::Field)
        p = {5.5556, 44.4444, 0, 0, "Loses on 5, 6, 7, 8", "Wins on 2, 3, 4, 9, 10, 11, 12"};
    else if (name == BetName::AnyCraps)
        p = {11.1111, 11.1111, 0, 0, "Loses on all other rolls", "Wins on 2, 3, 12"};
    else if (name == BetName::AnySeven)
        p = {16.6667, 16.6667, 0, 0, "Loses on all other rolls", "Wins on 7"};
    else if (name == BetName::CandE)
        p = {2.7778, 16.6667, 0, 0, "Loses on all other rolls", "Wins on craps or 11"};
    else if (name == BetName::Horn)
        p = {0.0, 16.6667, 0, 0, "Loses on all other rolls", "Wins on 2, 3, 11, 12"};
    else if (name == BetName::World)
        p = {13.3333, 33.3333, 0, 0, "Loses on all other rolls", "Wins on 2, 3, 7, 11, 12"};
    return p;
}

//----------------------------------------------------------------

using RefArray = std::array<std::string_view, 6>;

static constexpr RefArray Place4or10{
    "Pays 9:5 True Odds 2:1 House Edge 6.67%",
    "Loses on 7. Wins on 4 or 10 respectively",
    "$10 bet -> $18 profit; $28 total returned",
    "Bet stays on table until a 7 or 4/10 is rolled",
    "Can be bet anytime, ok to remove anytime",
    "Off on comeout roll by default (configurable)"
};
static constexpr RefArray Place5or9{
    "Pays 7:5 True Odds 3:2 House Edge 4.00%",
    "Loses on 7. Wins on 5 or 9 respectively",
    "$10 bet -> $14 profit; $24 total returned",
    "Bet stays on table until a 7 or 5/9 is rolled",
    "Can be bet anytime, ok to remove anytime",
    "Off on comeout roll by default (configurable)"
};
static constexpr RefArray Place6or8{
    "Pays 7:6 True Odds 6:5 House Edge 1.52%",
    "Loses on 7. Wins on 6 or 8 respectively",
    "$12 bet -> $14 profit; $26 total returned",
    "Bet stays on table until a 7 or 6/8 is rolled",
    "Can be bet anytime, ok to remove anytime",
    "Off on comeout roll by default (configurable)"
};
static constexpr RefArray Buy4or10{
    "Pays 2:1 True Odds 2:1 House Edge 1.67%",
    "Loses on 7. Wins on 4 or 10 respectively",
    "$5 bet -> $10 profit; $15 total returned",
    "Bet stays on table until a 7 or 4/10 is rolled",
    "Can be bet anytime, ok to remove anytime",
    "Buy vig/commission rules depend on table settings"
};
static constexpr RefArray Buy5or9{
    "Pays 3:2 True Odds 3:2 House Edge 2.00%",
    "Loses on 7. Wins on 5 or 9 respectively",
    "$2 bet -> $3 profit; $5 total returned",
    "Bet stays on table until a 7 or 5/9 is rolled",
    "Can be bet anytime, ok to remove anytime",
    "Buy vig/commission rules depend on table settings"
};
static constexpr RefArray Buy6or8{
    "Pays 6:5 True Odds 6:5 House Edge 2.27%",
    "Loses on 7. Wins on 6 or 8 respectively",
    "$5 bet -> $6 profit; $11 total returned",
    "Bet stays on table until a 7 or 6/8 is rolled",
    "Can be bet anytime, ok to remove anytime",
    "Buy vig/commission rules depend on table settings"
};
static constexpr RefArray Lay4or10{
    "Pays 1:2 True Odds 1:2 House Edge 1.67%",
    "Wins on 7. Loses on 4 or 10 respectively",
    "$10 bet -> $5 profit; $15 total returned",
    "Bet stays on table until a 7 or 4/10 is rolled",
    "Can be bet anytime, ok to remove anytime",
    "Off on comeout roll by default (configurable)"
};
static constexpr RefArray Lay5or9{
    "Pays 2:3 True Odds 2:3 House Edge 2.00%",
    "Wins on 7. Loses on 5 or 9 respectively",
    "$9 bet -> $6 profit; $15 total returned",
    "Bet stays on table until a 7 or 5/9 is rolled",
    "Can be bet anytime, ok to remove anytime",
    "Off on comeout roll by default (configurable)"
};
static constexpr RefArray Lay6or8{
    "Pays 5:6 True Odds 5:6 House Edge 2.27%",
    "Wins on 7. Loses on 6 or 8 respectively",
    "$12 bet -> $10 profit; $22 total returned",
    "Bet stays on table until a 7 or 6/8 is rolled",
    "Can be bet anytime, ok to remove anytime",
    "Off on comeout roll by default (configurable)"
};
static constexpr RefArray Passline{
    "Pays 1:1 True Odds 2:9. House Edge 1.41%",
    "Wins on 7,11; loses on 2,3,12 (comeout roll)",
    "Otherwise a point is chosen; later wins on point",
    "$10 bet -> $10 profit; $20 total returned",
    "Bet stays on table until a point or 7 is rolled",
    "Only bet on comeout; normally cannot be removed"
};
static constexpr RefArray Come{
    "Pays 1:1 True Odds 2:9. House Edge 1.41%",
    "Wins on 7,11; loses on 2,3,12 on comeout roll",
    "Otherwise follows the rolled number as its point",
    "$10 bet -> $10 profit; $20 total returned",
    "Bet stays on table until the number or 7 is rolled",
    "Can be bet after the comeout roll"
};
static constexpr RefArray Dontpass{
    "Pays 1:1 True Odds 1:12. House Edge 1.36%",
    "Wins on 2,3; loses on 7,11; 12 pushes (comeout)",
    "Otherwise point is chosen; later wins on 7",
    "$10 bet -> $10 profit; $20 total returned",
    "Bet stays on table until a point or 7 is rolled",
    "Only bet on comeout; may be removed by table rules"
};
static constexpr RefArray Dontcome{
    "Pays 1:1 True Odds 1:12. House Edge 1.36%",
    "Wins on 2,3; loses on 7,11; 12 pushes on comeout",
    "Otherwise follows rolled number; later wins on 7",
    "$10 bet -> $10 profit; $20 total returned",
    "Bet stays on table until a number or 7 is rolled",
    "Can be bet after the comeout roll"
};
static constexpr RefArray OddsBet4or10{
    "Pays 2:1 True Odds 2:1 House Edge 0%",
    "Loses on 7. Wins on 4 or 10 respectively",
    "$10 bet -> $20 profit; $30 total returned",
    "Bet stays on table until a 7 or 4/10 is rolled",
    "Can only be bet after comeout roll",
    "Off on comeout roll by default (configurable)"
};
static constexpr RefArray OddsBet5or9{
    "Pays 3:2 True Odds 3:2 House Edge 0%",
    "Loses on 7. Wins on 5 or 9 respectively",
    "$10 bet -> $15 profit; $25 total returned",
    "Bet stays on table until a 7 or 5/9 is rolled",
    "Can only be bet after comeout roll",
    "Off on comeout roll by default (configurable)"
};
static constexpr RefArray OddsBet6or8{
    "Pays 6:5 True Odds 6:5 House Edge 0%",
    "Loses on 7. Wins on 6 or 8 respectively",
    "$10 bet -> $12 profit; $22 total returned",
    "Bet stays on table until a 7 or 6/8 is rolled",
    "Can only be bet after comeout roll",
    "Off on comeout roll by default (configurable)"
};
static constexpr RefArray DontOddsBet4or10{
    "Pays 1:2 True Odds 1:2 House Edge 0%",
    "Loses on 4 or 10 respectively. Wins on 7",
    "$10 bet -> $5 profit; $15 total returned",
    "Bet stays on table until a 7 or 4/10 is rolled",
    "Can only be bet after comeout roll",
    "Off on comeout roll by default (configurable)"
};
static constexpr RefArray DontOddsBet5or9{
    "Pays 2:3 True Odds 2:3 House Edge 0%",
    "Loses on 5 or 9 respectively. Wins on 7",
    "$9 bet -> $6 profit; $15 total returned",
    "Bet stays on table until a 7 or 5/9 is rolled",
    "Can only be bet after comeout roll",
    "Off on comeout roll by default (configurable)"
};
static constexpr RefArray DontOddsBet6or8{
    "Pays 5:6 True Odds 5:6 House Edge 0%",
    "Loses on 6 or 8 respectively. Wins on 7",
    "$12 bet -> $10 profit; $22 total returned",
    "Bet stays on table until a 7 or 6/8 is rolled",
    "Can only be bet after comeout roll",
    "Off on comeout roll by default (configurable)"
};
static constexpr RefArray Hardway4or10{
    "Pays 7:1 True Odds 8:1 House Edge 11.11%",
    "Wins when the point rolls hard (double 2s or 5s)",
    "Loses on 7 or when the point rolls easy",
    "$1 bet -> $7 profit; $8 total returned",
    "Bet stays on table until it wins or loses",
    "Can be bet anytime; may be turned off by rules"
};
static constexpr RefArray Hardway6or8{
    "Pays 9:1 True Odds 10:1 House Edge 9.09%",
    "Wins when the point rolls hard (double 3s or 4s)",
    "Loses on 7 or when the point rolls easy",
    "$1 bet -> $9 profit; $10 total returned",
    "Bet stays on table until it wins or loses",
    "Can be bet anytime; may be turned off by rules"
};
static constexpr RefArray Field{
    "Pays 1:1, True Odds 4:5. House Edge 5.56%",
    "Pays 2:1 on 2,12",
    "Loses on 5,6,7,8 Wins on 2,3,4,9,10,11,12",
    "$10 bet -> $10 profit; $20 total returned",
    "Bet stays on table for one roll",
    "Can be bet anytime"
};
static constexpr RefArray AnyCraps{
    "Pays 7:1. House Edge depends on table payout",
    "Wins on 2, 3, or 12",
    "Loses on every other roll",
    "$1 bet -> $7 profit; $8 total returned",
    "Bet resolves on the next roll",
    "Can be bet before any roll"
};
static constexpr RefArray AnySeven{
    "Pays 4:1. House Edge depends on table payout",
    "Wins on 7",
    "Loses on every other roll",
    "$1 bet -> $4 profit; $5 total returned",
    "Bet resolves on the next roll",
    "Can be bet before any roll"
};
static constexpr RefArray CandE{
    "Craps & Eleven pays depend on table rules",
    "Wins on craps (2,3,12) or 11",
    "Loses on every other roll",
    "Payout depends on which number is rolled",
    "Bet resolves on the next roll",
    "Can be bet before any roll"
};
static constexpr RefArray Horn{
    "Horn payout depends on the number rolled",
    "Wins on 2, 3, 11, or 12",
    "Loses on every other roll",
    "Payout depends on which number is rolled",
    "Bet resolves on the next roll",
    "Can be bet before any roll"
};
static constexpr RefArray World{
    "Pays 30:1 on 2 or 12, 15:1 on 3 or 11, 4:1 on 7",
    "Five equal components: one each on 2, 3, 7, 11 and 12",
    "$5 World bet: $1 on each of the five totals",
    "One roll only; all five components settle independently",
    "Any total other than 2, 3, 7, 11 or 12 loses",
    "Requires a wager that is a multiple of $5"
};

const RefArray&
referenceFor(BetName name, unsigned pivot, bool oddsBet)
{
    const bool point410 = pivot == 4 || pivot == 10;
    const bool point59  = pivot == 5 || pivot == 9;
    if (oddsBet)
    {
        if (name == BetName::DontPass || name == BetName::DontCome)
            return point410 ? DontOddsBet4or10 : point59 ? DontOddsBet5or9 : DontOddsBet6or8;
        if (name == BetName::PassLine || name == BetName::Come)
            return point410 ? OddsBet4or10 : point59 ? OddsBet5or9 : OddsBet6or8;
    }
    switch (name)
    {
    case BetName::Place:    return point410 ? Place4or10 : point59 ? Place5or9 : Place6or8;
    case BetName::Buy:      return point410 ? Buy4or10   : point59 ? Buy5or9   : Buy6or8;
    case BetName::Lay:      return point410 ? Lay4or10   : point59 ? Lay5or9   : Lay6or8;
    case BetName::PassLine: return Passline;
    case BetName::Come:     return Come;
    case BetName::DontPass: return Dontpass;
    case BetName::DontCome: return Dontcome;
    case BetName::Hardway:  return point410 ? Hardway4or10 : Hardway6or8;
    case BetName::Field:    return Field;
    case BetName::AnyCraps: return AnyCraps;
    case BetName::AnySeven: return AnySeven;
    case BetName::CandE:    return CandE;
    case BetName::Horn:     return Horn;
    case BetName::World:    return World;
    default:                return Passline;
    }
}
    
} // namespace

//----------------------------------------------------------------

DialogBetAmount::DialogBetAmount()
    : DialogBase("DialogBetAmount")
{
    // Create an initial WINDOW at location 0,0. Gets resized later.
    newWindow(winSize_.rows, winSize_.cols, winPos_.row, winPos_.col);
    drawStaticContent();
}

//----------------------------------------------------------------

DialogBetAmount&
DialogBetAmount::instance()
{
    static DialogBetAmount da;
    return da;
}

//----------------------------------------------------------------

void
DialogBetAmount::onAttach(SurfaceBase* pParent)
{
    prepLineInput();
    curs_set(1);
}

//----------------------------------------------------------------

void
DialogBetAmount::onDetach()
{
    curs_set(0);
}

//----------------------------------------------------------------
//
// SurfaceManager wants our window size and more.
// This occurs in context of SurfaceManager::pushSurface()
// SurfaceManager informs us shortly of our screen position.
// See setLocation() below.
//
LocationRequest
DialogBetAmount::getLocationRequest() const
{
    LocationRequest req;
    req.kind      = LocationKind::Dialog;
    req.size.rows = winSize_.rows;
    req.size.cols = winSize_.cols;
    req.direction = Direction::Right;
    return req;
}

//----------------------------------------------------------------
//
// SurfaceManager tells us our location.
// This occurs in context of SurfaceManager::pushSurface().
// We now have enough information to create/resize our
// ncurses WINDOW. Following this, SurfaceManager will
// call draw() on us.
//
void
DialogBetAmount::setLocation(WindowPosition pos)
{
    winPos_ = pos;
    resize(winSize_);  // Dynamic menu needs resizing
    repos(winPos_);    // and repositioning
}

//----------------------------------------------------------------
//
// Fixed 24 x 59 layout. The reference and statistics areas are
// deliberately separate from the interactive amount-entry area.
//
//    0123456789 123456789 123456789 123456789 123456789 12345678
// 0  ┌─────────────────────────────────────────────────────────┐
// 1  │ DontPass bet Odds on the 10                    Pays 1:2 │
// 2  ├─────────────────────────────────────────────────────────┤
// 3  │ Use multiple of 2, min $10 max $125 current $25         │
// 4  │                                                         │
// 5  │ Enter amount: 25                                        │
// 6  │                                                         │
// 7  │ [enter] Submit Bet                                      │
// 8  │ [a] Save as AutoFill                                    │
// 9  │ [q] Save as Quick Bet                                   │
// 10 │ [?] Help                                                │
// 11 │ [. or esc] Back                                         │
// 12 ├─────────────────────────────────────────────────────────┤
// 13 │ Pays 1:2 True Odds 1:2 House Edge 0%                    │
// 14 │ Loses on 4 or 10 respectively. Wins on 7                │
// 15 │ $10 bet -> $5 profit; $15 total returned                │
// 16 │ Bet stays on table until a 7 or 4/10 is rolled          │
// 17 │ Can only be bet after comeout roll                      │
// 18 │ Off on comeout roll by default (configurable)           │
// 19 ├─────────────────────────────────────────────────────────┤
// 20 │ Total Amount: $0       Net: +$0         Expected: -$0.00│
// 21 │ Counts:     0 0W,0L    Win rate:  --.-% Expected:  49.3%│
// 22 │ Simulated:  0 0W,0L    Win rate:  --.-% Expected:  49.3%│
// 23 └─────────────────────────────────────────────────────────┘
//
void
DialogBetAmount::draw()
{
    drawTitle();
    drawAmountLimits();
    drawReference();
    drawStatistics();
    drawInput();  // Last, so cursor appears after input area
    CuiUtils::transfer(pWin_);
}

//----------------------------------------------------------------
//
// Only done once at init time. These fields are never overwritten
// and we don't call werase().
//
void
DialogBetAmount::drawStaticContent()
{
    drawBorders();
    drawPrompts();
}

//----------------------------------------------------------------

void
DialogBetAmount::drawBorders()
{
    box(pWin_, 0, 0);

    // Section separators: title, reference, statistics.
    for (const int row : {2, 12, 19})
    {
        mvwhline(pWin_, row, 1, ACS_HLINE, winSize_.cols - 2);
        mvwaddch(pWin_, row, 0, ACS_LTEE);
        mvwaddch(pWin_, row, winSize_.cols - 1, ACS_RTEE);
    }
}

//----------------------------------------------------------------

void
DialogBetAmount::drawAmountLimits()
{
    const auto& cb = CarrierBet::instance();
    Craps::PlayerId playerId;
    Craps::BetAmountLimits limits;
    Gen::ErrorPass ep;

    const auto rc = Ctrl::CrapsReaders::getUserPlayer(playerId, ep);
    if (rc != Gen::ReturnCode::Success ||
        Ctrl::CrapsReaders::readBetAmountLimits(
            playerId, cb.getBetName(), cb.getPivot(), cb.isOddsBet(),
            cb.getContractAmount(), limits, ep) != Gen::ReturnCode::Success)
    {
        mvwaddstr(pWin_, 3, 2,
                  padLine("Unable to retrieve bet amount limits").c_str());
        return;
    }

    const std::string line = fmt::format(
        "Use multiple of {}, min {} max {} current {}",
        limits.increment,
        money(limits.minimum),
        money(limits.maximum),
        money(limits.currentAmount));
    mvwaddstr(pWin_, 3, 2, padLine(line).c_str());
}

//----------------------------------------------------------------

void
DialogBetAmount::drawReference()
{
    const auto& cb = CarrierBet::instance();
    const RefArray& lines = referenceFor(cb.getBetName(), cb.getPivot(), cb.isOddsBet());

    // The payout is shown in the title row. Keep its odds/house-edge details
    // in the reference area, along with the remaining bet-specific guidance.
    for (std::size_t i = 0; i < lines.size(); ++i)
    {
        const std::string line = i == 0
            ? referencePayoutDetails(lines[i])
            : std::string(lines[i]);
        mvwaddstr(pWin_, 13 + static_cast<int>(i), 2,
                  padLine(line).c_str());
    }
}

//----------------------------------------------------------------

void
DialogBetAmount::drawStatistics()
{
    Craps::PlayerStats playerStats;
    CuiUtils::getUserPlayerStats(playerStats);
    const auto& stats = playerStats.betStats.betTypeStats;

    Craps::TableStats tableStats;
    CuiUtils::getActiveTableStats(tableStats);

    const auto& cb = CarrierBet::instance();  // shorthand
    const std::string key = betStatsKey(cb.getBetName(), cb.getPivot());
    const bool aggregateByPrefix = cb.getPivot() == 0 &&
                                   isRollOutcomeBet(cb.getBetName());

    Craps::BetStat     playerWins;
    Craps::BetStat     playerLosses;
    SimulatedBetCounts simulated;
    
    playerWins   = findBetStat(stats.wins, key, aggregateByPrefix);
    playerLosses = findBetStat(stats.lose, key, aggregateByPrefix);
    simulated    = simulatedBetCounts(tableStats.rollStats,
                                      cb.getBetName(), cb.getPivot());

    const unsigned playerCount        = playerWins.count + playerLosses.count;
    const unsigned simulatedCount     = simulated.wins + simulated.losses;
    const Gen::Money playerTotal      = playerWins.amountBet + playerLosses.amountBet;
    const Gen::Money playerWinAmount  = playerWins.amount;
    const Gen::Money playerLoseAmount = playerLosses.amount;
    const BetProfile profile          = profileFor(cb.getBetName(), cb.getPivot());
    std::string expected = "--";
    std::string expectedPct = "--.-%";

    if (profile.winPct >= 0.0)
        expectedPct = fmt::format("{:.1f}%", profile.winPct);

    // BetStats records the contract and odds portions under the same
    // expanded bet name. Use the selected bet's profile for an estimate on
    // the recorded total; this is an approximation when the total includes
    // fair-odds supplements, because the current stats do not retain the
    // contract-only amount separately.
    if (profile.houseEdgePct >= 0.0)
    {
        const double loss =
            static_cast<double>(playerTotal) * profile.houseEdgePct / 100.0;
        expected = profile.houseEdgePct == 0.0
            ? expectedMoney(loss)
            : "-" + expectedMoney(loss);
    }

    // Explicit field widths keep the labels aligned between all three rows.
    // The statistics area has 56 usable columns starting at column 2.
    const std::string line1 = fmt::format(
        "{:<14}{:<8} Net: {:<8}    Expected: {:<6}",
        "Total Amount:",
        money(playerTotal),
        signedMoney(playerWinAmount, playerLoseAmount),
        expected);
    mvwaddstr(pWin_, 20, 2, padLine(line1).c_str());

    auto statsLine = [&](std::string_view label, unsigned count,
                         unsigned wins, unsigned losses)
    {
        const std::string winLoss = fmt::format("{}W,{}L", wins, losses);
        const std::string out = fmt::format(
            "{:<10}{:>3} {:<8} Win rate: {:>6} Expected: {:>6}",
            label,
            count,
            winLoss,
            percent(wins, losses),
            expectedPct);
        return padLine(out);
    };

    mvwaddstr(pWin_, 21, 2,
              statsLine("Counts:", playerCount,
                        playerWins.count, playerLosses.count).c_str());

    mvwaddstr(pWin_, 22, 2,
              statsLine("Simulated:", simulatedCount,
                        simulated.wins, simulated.losses).c_str());
}

//----------------------------------------------------------------

void
DialogBetAmount::drawPrompts()
{
    mvwaddstr(pWin_, inputRow_, 2, "Enter amount:");
    int row = 7;
    for (const std::string_view s : Prompts)
    {
        mvwaddstr(pWin_, row++, 2, s.data());
    }
}

//----------------------------------------------------------------

void
DialogBetAmount::drawTitle()
{
    const auto& cb = CarrierBet::instance();
    const std::string title = cb.getTitleBetAmount();
    const RefArray& lines = referenceFor(cb.getBetName(), cb.getPivot(), cb.isOddsBet());
    std::string payout = payoutLabel(lines.front());
    // Keep the aggregate World payout compact enough to leave its title intact.
    if (cb.getBetName() == BetName::World)
        payout = "Pays 30:1 (2/12), 15:1 (3/11), 4:1 (7)";

    constexpr std::size_t TitleWidth = winSize_.cols - 4;
    const std::size_t payoutWidth =
        payout.size() < TitleWidth ? payout.size() : TitleWidth;
    const std::size_t titleWidth = payout.empty()
        ? TitleWidth
        : TitleWidth - payoutWidth - 1;

    std::string s = title.substr(0, titleWidth);
    s.resize(TitleWidth - payoutWidth, ' ');
    s += payout.substr(0, payoutWidth);
    mvwaddstr(pWin_, 1, 2, s.c_str());
}

//----------------------------------------------------------------

std::string
DialogBetAmount::prepTitle(const std::string& title)
{
    constexpr std::size_t MaxColsTitle = winSize_.cols - 4;
    std::string result = title.substr(0, MaxColsTitle);  // Too big
    result.resize(MaxColsTitle, ' ');                    // Pad it
    return result;
}

//----------------------------------------------------------------
//
// Override menu base class
//
bool
DialogBetAmount::handleKey(int ch)
{
    switch (ch)
    {
    case '\r':
    case '\n':
    case KEY_ENTER: doEnter(); return true;
    case 'a' : doAutoFill();   return true;
    case 'q' : doQuickBet();   return true;
    case '?' : doHelp();       return true;
    case '.' :
    case 27  : back();         return true;
    default:
        if (lineInput_.handleKey(ch))
        {
            // Keep the illustrative payout in sync with the amount being typed.
            if (!readInputAmount())
                inputAmount_ = 0;
            drawReference();
            drawStatistics();
            drawInput();
            CuiUtils::transfer(pWin_);
            // The normal surface redraw path calls doupdate(), but this key
            // handler updates only the input window. Commit the virtual-screen
            // changes now so both the edited digits and cursor are visible.
            doupdate();
            return true;
        }
        return false;
    }
}

//----------------------------------------------------------------

void
DialogBetAmount::back()
{
    // Set our own state in base class to reflect cancel.
    // Also informs parent surfaces of the state of operation.
    // In turn, parent menus can decide if they are skipped
    // when unwinding the menu stack.
    //
    setOperationResult(OperationResult::Cancel);  // base class
    SurfaceManager::instance().popSurfaces();
}

//----------------------------------------------------------------

void
DialogBetAmount::drawInput()
{
    // Erase the entry field area to avoid leftover characters
    mvwaddstr(pWin_, inputRow_, inputCol_, eraseInput_.c_str());

    // Print the current buffer
    mvwaddstr(pWin_, inputRow_, inputCol_, lineInput_.value().c_str());

    // Place the cursor at the correct position
    wmove(pWin_, inputRow_, inputCol_ + static_cast<int>(lineInput_.cursorPosition()));

    wnoutrefresh(pWin_);
}

//----------------------------------------------------------------

bool
DialogBetAmount::readInputAmount()
{
    const std::string& value = lineInput_.value();

    if (value.empty())
    {
        inputAmount_ = 0;
        return false;
    }

    try
    {
        std::size_t parsed = 0;
        const long long val = std::stoll(value, &parsed);

        // Require the entire input to be valid.
        if (parsed != value.size() ||
            val < 0 ||
            val > std::numeric_limits<Gen::Money>::max())
        {
            inputAmount_ = 0;
            return false;
        }

        inputAmount_ = static_cast<Gen::Money>(val);
        return true;
    }
    catch (const std::invalid_argument&)
    {
        inputAmount_ = 0;
        return false;
    }
    catch (const std::out_of_range&)
    {
        inputAmount_ = 0;
        return false;
    }
}

//----------------------------------------------------------------

void
DialogBetAmount::doEnter()
{
    if (!readInputAmount())
    {
        beep();
        return;
    }

    processAmount();
}

//----------------------------------------------------------------

void
DialogBetAmount::doAutoFill()
{
    if (!readInputAmount())
    {
        beep();
        return;
    }

    CarrierBet::instance().setAmount(inputAmount_);
    OperationManager::instance().autoFill();
}

//----------------------------------------------------------------

void
DialogBetAmount::doQuickBet()
{
    if (!readInputAmount())
    {
        beep();
        return;
    }

    CarrierBet::instance().setAmount(inputAmount_);
    OperationManager::instance().quickBet();
}

//----------------------------------------------------------------

void
DialogBetAmount::doHelp()
{
    // TODO
}

//----------------------------------------------------------------

void
DialogBetAmount::prepLineInput()
{
    lineInput_.clear();
    inputAmount_ = getPrefillAmount();
    lineInput_.setValue(std::to_string(inputAmount_));
}

//----------------------------------------------------------------

Gen::Money
DialogBetAmount::getPrefillAmount() const
{
    auto fill = getAutoFill();
    if (fill == 0) fill = getLastFill();
    return fill;
}

//----------------------------------------------------------------

Gen::Money
DialogBetAmount::getAutoFill() const
{
    const auto& cb = CarrierBet::instance();

    Ctrl::AutoFill::AutoFillEntry afe
    {
        cb.getBetName(),
        cb.getPivot(),
        cb.isOddsBet(),
        0                  // AutoFill returns preFillAmount here
    };

    Gen::ErrorPass ep;
    (void) Ctrl::CrapsReaders::getAutoFill(afe, ep);

    return afe.amount;
}

//----------------------------------------------------------------

Gen::Money
DialogBetAmount::getLastFill() const
{
    const auto& cb = CarrierBet::instance();

    Ctrl::LastFill::LastFillEntry lfe
    {
        cb.getBetName(),
        cb.getPivot(),
        cb.isOddsBet(),
        0                  // LastFill returns preFillAmount here
    };

    Gen::ErrorPass ep;
    (void) Ctrl::CrapsReaders::getLastFill(lfe, ep);

    return lfe.amount;
}

//----------------------------------------------------------------

void
DialogBetAmount::processAmount()
{
    CarrierBet::instance().setAmount(inputAmount_);
    // Keep this dialog active until the asynchronous request responds.
    // OperationManager owns the rest of this workflow.
    (void) OperationManager::instance().makeBet();
}

//----------------------------------------------------------------
