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
#include <craps/PlayerStats.h>
#include <craps/TableStats.h>
#include <craps/BetStats.h>
#include <gen/Logger.h>
#include <ncurses.h>
#include <limits>
#include <stdexcept>
#include <iomanip>
#include <map>
#include <sstream>

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
money(Gen::Money amount)
{
    return Gen::MoneyUtils::toString(amount);
}

//----------------------------------------------------------------

std::string
expectedMoney(double amount)
{
    const auto cents = static_cast<long long>(amount * 100.0 + 0.5);
    const auto dollars = static_cast<Gen::Money>(cents / 100);
    const auto remainder = static_cast<unsigned>(cents % 100);
    std::ostringstream out;
    out << money(dollars) << '.' << std::setw(2) << std::setfill('0')
        << remainder;
    return out.str();
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

std::string
percent(unsigned wins, unsigned losses)
{
    const unsigned total = wins + losses;
    if (total == 0)
        return "--.-%";
    std::ostringstream out;
    out << std::fixed << std::setprecision(1)
        << (100.0 * static_cast<double>(wins) / total) << '%';
    return out.str();
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
    return name == BetName::Field || name == BetName::AnyCraps ||
           name == BetName::AnySeven || name == BetName::CandE ||
           name == BetName::Horn;
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
    "$10 bet -> $6 profit; $16 total returned",
    "Bet stays on table until a 7 or 5/9 is rolled",
    "Can be bet anytime, ok to remove anytime",
    "Off on comeout roll by default (configurable)"
};
static constexpr RefArray Lay6or8{
    "Pays 5:6 True Odds 5:6 House Edge 2.27%",
    "Wins on 7. Loses on 6 or 8 respectively",
    "$10 bet -> $8 profit; $18 total returned",
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
    "$10 bet -> $6 profit; $16 total returned",
    "Bet stays on table until a 7 or 5/9 is rolled",
    "Can only be bet after comeout roll",
    "Off on comeout roll by default (configurable)"
};
static constexpr RefArray DontOddsBet6or8{
    "Pays 5:6 True Odds 5:6 House Edge 0%",
    "Loses on 6 or 8 respectively. Wins on 7",
    "$10 bet -> $5 profit; $15 total returned",
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
void
DialogBetAmount::draw()
{
    drawTitle();
    drawHintMultiples();
    drawReference();
    drawStatistics();
    // Draw the input last: later mvwaddstr() calls otherwise leave the
    // ncurses cursor at the end of the statistics area.
    drawInput();
    CuiUtils::transfer(pWin_);
}

//----------------------------------------------------------------

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
DialogBetAmount::drawHintMultiples()
{
    const auto& cb = CarrierBet::instance();

    // Group points with identical payout rules.
    enum class PointGroup { FourTen, FiveNine, SixEight, Other };

    PointGroup pointGroup = PointGroup::Other;
    switch (cb.getPivot())
    {
    case 4:
    case 10: pointGroup = PointGroup::FourTen; break;
    case 5:
    case 9:  pointGroup = PointGroup::FiveNine; break;
    case 6:
    case 8:  pointGroup = PointGroup::SixEight; break;
    }

    const auto index      = static_cast<std::size_t>(pointGroup);
    std::string_view hint = defaultHints[index];
    const auto betName    = cb.getBetName();

    if (cb.isOddsBet())
    {
        if (betName == BetName::PassLine || betName == BetName::Come)
            hint = passCome[index];
        else if (betName == BetName::DontPass || betName == BetName::DontCome)
            hint = dontPassCome[index];
    }
    else if (betName == BetName::Place)
        hint = place[index];
    else if (betName == BetName::Buy)
        hint = buy[index];
    else if (betName == BetName::Lay)
        hint = lay[index];

    mvwaddstr(pWin_, 3, 2, padLine(std::string(hint)).c_str());
}

//----------------------------------------------------------------

void
DialogBetAmount::drawReference()
{
    const auto& cb = CarrierBet::instance();
    const RefArray& lines = referenceFor(cb.getBetName(), cb.getPivot(), cb.isOddsBet());

    // The reference area is a fixed six-row fragment. Keep this loop simple
    // so each bet's wording is defined entirely by its RefArray above.
    for (std::size_t i = 0; i < lines.size(); ++i)
        mvwaddstr(pWin_, 13 + static_cast<int>(i), 2,
                  padLine(std::string(lines[i])).c_str());
}
//----------------------------------------------------------------

void
DialogBetAmount::drawStatistics()
{
    const auto& cb = CarrierBet::instance();
    Craps::PlayerStats playerStats;
    Craps::TableStats tableStats;
    Craps::PlayerId playerId;
    Craps::TableId tableId;
    Gen::ErrorPass ep;

    const bool havePlayer =
        Ctrl::CrapsReaders::getUserPlayer(playerId, ep) == Gen::ReturnCode::Success &&
        Ctrl::CrapsReaders::readPlayerCurrentStats(playerId, playerStats, ep) == Gen::ReturnCode::Success;
    ep.clear();
    const bool haveTable =
        Ctrl::CrapsReaders::getActiveCrapsTable(tableId, ep) == Gen::ReturnCode::Success &&
        Ctrl::CrapsReaders::readTableCurrentStats(tableId, tableStats, ep) == Gen::ReturnCode::Success;

    Craps::BetStat playerWins, playerLosses, simulatedWins, simulatedLosses;
    const std::string key = betStatsKey(cb.getBetName(), cb.getPivot());
    const bool aggregateByPrefix =
        cb.getPivot() == 0 && isRollOutcomeBet(cb.getBetName());
    if (havePlayer)
    {
        const auto& stats = playerStats.betStats.betTypeStats;
        playerWins = findBetStat(stats.wins, key, aggregateByPrefix);
        playerLosses = findBetStat(stats.lose, key, aggregateByPrefix);
    }
    if (haveTable)
    {
        const auto& stats = tableStats.betStats.betTypeStats;
        simulatedWins = findBetStat(stats.wins, key, aggregateByPrefix);
        simulatedLosses = findBetStat(stats.lose, key, aggregateByPrefix);
    }

    const unsigned playerCount = playerWins.count + playerLosses.count;
    const unsigned simulatedCount = simulatedWins.count + simulatedLosses.count;
    const Gen::Money playerTotal = playerWins.amountBet + playerLosses.amountBet;
    const Gen::Money playerWinAmount = playerWins.amount;
    const Gen::Money playerLoseAmount = playerLosses.amount;
    const BetProfile profile = profileFor(cb.getBetName(), cb.getPivot());
    std::string expected = "--";
    std::string expectedPct = "--.-%";
    if (profile.winPct >= 0.0)
    {
        std::ostringstream out;
        out << std::fixed << std::setprecision(1) << profile.winPct << '%';
        expectedPct = out.str();
    }
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
    std::ostringstream line1;
    line1 << std::left << std::setw(14) << "Total Amount:"
          << std::setw(8) << money(playerTotal)
          << ' ' << "Net:" << ' '
          << std::setw(8) << signedMoney(playerWinAmount, playerLoseAmount)
          << std::string(4, ' ') << "Expected:" << ' '
          << std::setw(6) << expected;
    mvwaddstr(pWin_, 20, 2, padLine(line1.str()).c_str());

    auto statsLine = [&](std::string_view label, unsigned count,
                         const Craps::BetStat& wins, const Craps::BetStat& losses)
    {
        std::ostringstream out;
        out << std::left << std::setw(10) << label
            << std::right << std::setw(3) << count << ' '
            << std::left << std::setw(8) << (std::to_string(wins.count) + "W," +
                                             std::to_string(losses.count) + "L")
            << ' ' << "Win rate:" << ' '
            << std::right << std::setw(6) << percent(wins.count, losses.count)
            << ' ' << "Expected:" << ' '
            << std::setw(6) << expectedPct;
        return padLine(out.str());
    };
    mvwaddstr(pWin_, 21, 2, statsLine("Counts:", playerCount,
                                      playerWins, playerLosses).c_str());
    mvwaddstr(pWin_, 22, 2, statsLine("Simulated:", simulatedCount,
                                      simulatedWins, simulatedLosses).c_str());
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
    const std::string title = CarrierBet::instance().getTitleBetAmount();
    const std::string s = prepTitle(title);
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
    case KEY_ENTER: doEnter();      return true;
    case 'a' : doAutoFill();        return true;
    case 'q' : doQuickBet();        return true;
    case '?' : doHelp();            return true;
    case '.' :
    case 27  : back();              return true;
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
