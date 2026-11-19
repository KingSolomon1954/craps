//----------------------------------------------------------------
//
// File: PlayerAreaOnePlayer.cpp
//
//----------------------------------------------------------------

#include <cui/panels/PlayerAreaOnePlayer.h>
#include <cui/managers/ColorManager.h>
#include <controller/CrapsReaders.h>
#include <gen/ErrorPass.h>
#include <gen/Logger.h>
#include <gen/MoneyUtils.h>
#include <cassert>

using namespace Cui;

//----------------------------------------------------------------

PlayerAreaOnePlayer::PlayerAreaOnePlayer(int height, int width)
    : winHeight_(height)
    , winWidth_(width)
{
}

//----------------------------------------------------------------

void
PlayerAreaOnePlayer::init(WINDOW* pWin)
{
    pWin_ = pWin;
    buildPlayerInfo();
}

//----------------------------------------------------------------

void
PlayerAreaOnePlayer::buildPlayerInfo()
{
    auto orderedIds = CuiUtils::orderedPlayerIds();
    
    players_.clear();
    players_.reserve(MaxPlayers);

    for (const auto& playerId : orderedIds)
    {
        players_.push_back(
        {
            .playerId  = playerId,
            .name      = CuiUtils::playerName(playerId),
            .colorPair = CuiUtils::playerColorPair(playerId)
        });
    }
}

//----------------------------------------------------------------

//
// Who is at the table or a name has changed. Learn the players again.
// Keep showing the same player if he is still here, otherwise the user
// player, who is always first.
//
void
PlayerAreaOnePlayer::rebuild()
{
    Craps::PlayerId showing;
    if (playerIndex_ >= 0 &&
        static_cast<std::size_t>(playerIndex_) < players_.size())
    {
        showing = players_[playerIndex_].playerId;
    }

    buildPlayerInfo();

    playerIndex_ = 0;
    for (std::size_t i = 0; i < players_.size(); ++i)
    {
        if (players_[i].playerId == showing)
        {
            playerIndex_ = static_cast<int>(i);
            break;
        }
    }
    refreshAllBets();
}

//----------------------------------------------------------------

std::size_t
PlayerAreaOnePlayer::removePlayer(const Craps::PlayerId& playerId)
{
    for (std::size_t i = 0; i < players_.size(); ++i)
    {
        if (players_[i].playerId == playerId)
        {
            players_.erase(players_.begin() + i);
            return i;
        }
    }
    return 0;
}

//----------------------------------------------------------------

void
PlayerAreaOnePlayer::drawInternalBorders()
{
    using L = Layout;

    // Horizontal lines
    mvwhline(pWin_, L::fieldBorderTopRow,    0, 0, winWidth_);
    mvwhline(pWin_, L::crapsBorderTopRow,    0, 0, winWidth_);
    mvwhline(pWin_, L::lineBetsBorderTopRow, 0, 0, winWidth_);

    // Vertical lines
    mvwvline(pWin_, 0, L::col4_5,  0, L::rowsNumbers);
    mvwvline(pWin_, 0, L::col5_6,  0, L::rowsNumbers);
    mvwvline(pWin_, 0, L::col6_8,  0, L::rowsNumbers);
    mvwvline(pWin_, 0, L::col8_9,  0, L::rowsNumbers);
    mvwvline(pWin_, 0, L::col9_10, 0, L::rowsNumbers);
    mvwvline(pWin_, L::lineBetsBorderTopRow + 1, L::colComeDont2, 0, L::rowsLineBets);
    
    // Junctions
    mvwaddch(pWin_, L::lineBetsBorderTopRow, L::col4_5,       ACS_BTEE);
    mvwaddch(pWin_, L::lineBetsBorderTopRow, L::col5_6,       ACS_BTEE);
    mvwaddch(pWin_, L::lineBetsBorderTopRow, L::col6_8,       ACS_BTEE);
    mvwaddch(pWin_, L::lineBetsBorderTopRow, L::col8_9,       ACS_BTEE);
    mvwaddch(pWin_, L::lineBetsBorderTopRow, L::col9_10,      ACS_BTEE);
    mvwaddch(pWin_, L::lineBetsBorderTopRow, L::colComeDont2, ACS_TTEE);
    mvwaddch(pWin_, L::fieldBorderTopRow,    L::colComeDont2, ACS_BTEE);
}

//----------------------------------------------------------------

void
PlayerAreaOnePlayer::drawStaticContent()
{
    drawName();
    drawLabels();

    // A full redraw erased the window. Put back the amounts that exist.
    for (const auto& b : bets_)
    {
        if (b.amount > 0)
        {
            drawOneBet(b);
        }
    }
}

//----------------------------------------------------------------

void
PlayerAreaOnePlayer::drawName()
{
    wattron(pWin_, COLOR_PAIR(players_[playerIndex_].colorPair));
    // Drawn on the Field numbers row, left side, where there is room
    // for the longest legal name (16 chars) before the numbers at col 24.
    mvwaddstr(pWin_, Layout::fieldNumbersRow, 1,
              players_[playerIndex_].name.c_str());
    wattroff(pWin_, COLOR_PAIR(players_[playerIndex_].colorPair));
}

//----------------------------------------------------------------

void
PlayerAreaOnePlayer::drawLabels()
{
    for (const auto& label : labels_)
    {
        mvwaddstr(pWin_, label.row, label.col, label.label.data());
    }
}

//----------------------------------------------------------------

void
PlayerAreaOnePlayer::drawBets()
{
    for (const auto& b : bets_)
    {
        if (b.amount > 0)
        {
            drawOneBet(b);
        }
    }
}

//----------------------------------------------------------------

void
PlayerAreaOnePlayer::drawOneBet(const Bet& b)
{
    using L = Layout;

    // Always blank the whole field first so a shorter amount does not
    // leave digits from a longer one behind.
    mvwaddstr(pWin_, b.row, b.col,
              std::string(L::betAmountWidth, ' ').c_str());

    if (b.amount == 0) return;

    // Left justified. Show the "$" only if the amount fits with it.
    std::string text = Gen::MoneyUtils::toStringNoCommas(b.amount);
    if (text.size() > static_cast<std::size_t>(L::betAmountWidth))
    {
        text = Gen::MoneyUtils::toStringPlain(b.amount);
    }
    // Amounts stand out from the labels: the player's color and bold.
    // Swap the attributes below to taste (e.g. A_REVERSE).
    const attr_t  attrs = A_BOLD;
    const short   pair  = players_.empty()
                              ? short(0)
                              : players_[playerIndex_].colorPair;
    wattr_on(pWin_, attrs | COLOR_PAIR(pair), nullptr);
    mvwaddstr(pWin_, b.row, b.col, text.c_str());
    wattr_off(pWin_, attrs | COLOR_PAIR(pair), nullptr);
}

//----------------------------------------------------------------
//
// Switching players. Have to refresh all bets.
//
void
PlayerAreaOnePlayer::refreshAllBets()
{
    clearBets();  // First set all bet amounts to zero

    Gen::ErrorPass ep;
    std::vector<Craps::BetId> betIds;
        
    auto rc = Ctrl::CrapsReaders::readPlayerGetBets(
        players_[playerIndex_].playerId, betIds, ep);
    assert(rc == Gen::ReturnCode::Success);

    for (auto betId : betIds)
    {
        applyBet(betId,
                 getBetName(betId),
                 getPivot(betId),
                 getContractAmount(betId),
                 getOddsAmount(betId));
    }
}

//----------------------------------------------------------------

void
PlayerAreaOnePlayer::clearBets()
{
    for (auto& b : bets_)
    {
        b.betId  = 0;
        b.amount = 0;
    }
}

//----------------------------------------------------------------

BetName
PlayerAreaOnePlayer::getBetName(Craps::BetId betId) const
{
    Gen::ErrorPass ep;
    BetName betName;

    auto rc = Ctrl::CrapsReaders::readBetName(betId, betName, ep);
    assert(rc == Gen::ReturnCode::Success);
    return betName;
}

//----------------------------------------------------------------

unsigned
PlayerAreaOnePlayer::getPivot(Craps::BetId betId) const
{
    Gen::ErrorPass ep;
    unsigned pivot;

    auto rc = Ctrl::CrapsReaders::readBetPivot(betId, pivot, ep);
    assert(rc == Gen::ReturnCode::Success);
    return pivot;
}

//----------------------------------------------------------------

Gen::Money
PlayerAreaOnePlayer::getContractAmount(Craps::BetId betId) const
{
    Gen::ErrorPass ep;
    Gen::Money contractAmount;

    auto rc = Ctrl::CrapsReaders::readBetContractAmount(
        betId, contractAmount, ep);
    assert(rc == Gen::ReturnCode::Success);
    return contractAmount;
}

//----------------------------------------------------------------

Gen::Money
PlayerAreaOnePlayer::getOddsAmount(Craps::BetId betId) const
{
    Gen::ErrorPass ep;
    Gen::Money oddsAmount;

    auto rc = Ctrl::CrapsReaders::readBetOddsAmount(
        betId, oddsAmount, ep);
    assert(rc == Gen::ReturnCode::Success);
    return oddsAmount;
}

//----------------------------------------------------------------

int
PlayerAreaOnePlayer::getBetIndex(BetName  betName,
                                 unsigned pivot) const
{
    CuiUtils::CuiBetName cuiBetName =
        CuiUtils::betToCuiBetName(betName, pivot);

    for (std::size_t i = 0; i < bets_.size(); ++i)
    {
        if (bets_[i].betName == cuiBetName)
        {
            return i;
        }
    }
    
    return bets_.size();
}

//----------------------------------------------------------------
//
// The screen has one cell per CuiBetName. A bet can fill two cells: the
// contract amount and its odds amount. Where they go:
//   PassLine/DontPass  contract -> PassLine/DontPass, any pivot
//                      odds     -> PassLineOdds/DontPassOdds
//   Come/DontCome      pivot 0  -> Come/DontCome (not yet traveled)
//                      pivot N  -> Come<N>/DontCome<N> and the odds row
//   everything else    contract -> its own cell (odds are always zero)
//
int
PlayerAreaOnePlayer::slotsFor(BetName    betName,
                              unsigned   pivot,
                              Gen::Money contractAmount,
                              Gen::Money oddsAmount,
                              std::array<Slot, 2>& slots)
{
    using N = CuiUtils::CuiBetName;

    auto byPivot = [pivot](N n4, N n5, N n6, N n8, N n9, N n10) -> N
    {
        switch (pivot)
        {
            case 4:  return n4;
            case 5:  return n5;
            case 6:  return n6;
            case 8:  return n8;
            case 9:  return n9;
            default: return n10;
        }
    };

    switch (betName)
    {
    case BetName::PassLine:
        slots[0] = {N::PassLine,     contractAmount};
        slots[1] = {N::PassLineOdds, oddsAmount};
        return 2;

    case BetName::DontPass:
        slots[0] = {N::DontPass,     contractAmount};
        slots[1] = {N::DontPassOdds, oddsAmount};
        return 2;

    case BetName::Come:
        if (pivot == 0)
        {
            slots[0] = {N::Come, contractAmount};
            return 1;
        }
        slots[0] = {byPivot(N::Come4, N::Come5, N::Come6,
                            N::Come8, N::Come9, N::Come10),
                    contractAmount};
        slots[1] = {byPivot(N::ComeOdds4, N::ComeOdds5, N::ComeOdds6,
                            N::ComeOdds8, N::ComeOdds9, N::ComeOdds10),
                    oddsAmount};
        return 2;

    case BetName::DontCome:
        if (pivot == 0)
        {
            slots[0] = {N::DontCome, contractAmount};
            return 1;
        }
        slots[0] = {byPivot(N::DontCome4, N::DontCome5, N::DontCome6,
                            N::DontCome8, N::DontCome9, N::DontCome10),
                    contractAmount};
        slots[1] = {byPivot(N::DontOdds4, N::DontOdds5, N::DontOdds6,
                            N::DontOdds8, N::DontOdds9, N::DontOdds10),
                    oddsAmount};
        return 2;

    default:
        slots[0] = {CuiUtils::betToCuiBetName(betName, pivot),
                    contractAmount};
        return 1;
    }
}

//----------------------------------------------------------------
//
// Remember a bet. First forget where it was, because a bet can move
// (a Come bet travels to its number, odds get added).
//
void
PlayerAreaOnePlayer::applyBet(Craps::BetId betId,
                              BetName      betName,
                              unsigned     pivot,
                              Gen::Money   contractAmount,
                              Gen::Money   oddsAmount)
{
    clearBetId(betId);

    std::array<Slot, 2> slots{};
    const int n = slotsFor(betName, pivot,
                           contractAmount, oddsAmount, slots);

    for (int i = 0; i < n; ++i)
    {
        if (slots[i].amount == 0) continue;

        for (auto& b : bets_)
        {
            if (b.betName == slots[i].name)
            {
                b.betId  = betId;
                b.amount = slots[i].amount;
                break;
            }
        }
    }
}

//----------------------------------------------------------------

void
PlayerAreaOnePlayer::clearBetId(Craps::BetId betId)
{
    for (auto& b : bets_)
    {
        if (b.betId == betId && b.amount != 0)
        {
            b.betId  = 0;
            b.amount = 0;
        }
    }
}

//----------------------------------------------------------------

void    
PlayerAreaOnePlayer::advancePlayer(bool next)
{
    if (next)
    {
        nextPlayerIndex();
    }
    else
    {
        prevPlayerIndex();
    }

    refreshAllBets();   // The caller redraws the window
}

//----------------------------------------------------------------

void
PlayerAreaOnePlayer::nextPlayerIndex()
{
    playerIndex_++;
    if (playerIndex_ == players_.size())
    {
        playerIndex_ = 0;
    }
}

//----------------------------------------------------------------

void
PlayerAreaOnePlayer::prevPlayerIndex()
{
    playerIndex_--;
    if (playerIndex_ < 0) 
    {
        playerIndex_ = players_.size() - 1;
    }
}

//----------------------------------------------------------------

void
PlayerAreaOnePlayer::onBetMade(
    const Craps::PlayerId& playerId,
    Craps::BetId           betId,
    BetName                betName,
    Gen::Money             contractAmount,
    Gen::Money             oddsAmount,
    unsigned               pivot)
{
    if (players_.empty()) return;
    if (playerId != players_[playerIndex_].playerId) return;

    // Only updates what is remembered. The caller redraws the window.
    applyBet(betId, betName, pivot, contractAmount, oddsAmount);
}

//----------------------------------------------------------------

void
PlayerAreaOnePlayer::onBetChanged(
    const Craps::PlayerId& playerId,
    Craps::BetId           betId,
    BetName                betName,
    Gen::Money             contractAmount,
    Gen::Money             oddsAmount,
    unsigned               pivot)
{
    onBetMade(playerId,
              betId,
              betName,
              contractAmount,
              oddsAmount,
              pivot);
}

//----------------------------------------------------------------

void
PlayerAreaOnePlayer::onBetResolved(
    const Craps::PlayerId& playerId,
    BetName                betName,
    Craps::BetId           betId,
    Gen::Money             amountWin,
    Gen::Money             amountLose)
{
    if (players_.empty()) return;
    if (playerId != players_[playerIndex_].playerId) return;

    // A win does not mean the bet left the table. Place, Buy, Lay and
    // Hardway bets stay up, and Come bets travel to their number. The
    // screen is re-synced from the model once the roll is settled; see
    // onResolveBetsEnd().
    (void)betName; (void)betId; (void)amountWin; (void)amountLose;
}

//----------------------------------------------------------------
//
// The roll is fully settled. Bets were removed or moved (Come bets
// travel to their number without any event), so learn them again.
//
void
PlayerAreaOnePlayer::onResolveBetsEnd()
{
    if (players_.empty()) return;
    refreshAllBets();
}

//----------------------------------------------------------------

void
PlayerAreaOnePlayer::onPlayerJoinedTable(
    const Craps::PlayerId& playerId)
{
    for (std::size_t i = 0; i < players_.size(); ++i)
    {
        if (players_[i].playerId == playerId)  // Already have it
            return;  
    }

    if (players_.size() >= MaxPlayers)
    {
        Gen::Logger::instance().logWarn(
            "PlayerAreaOnePlayer: cannot display player; "
            "maximum players reached");
        return;
    }
    
    players_.push_back(
    {
        .playerId  = playerId,
        .name      = CuiUtils::playerName(playerId),
        .colorPair = CuiUtils::playerColorPair(playerId),
    });
}

//----------------------------------------------------------------

void
PlayerAreaOnePlayer::onPlayerLeftTable(
    const Craps::PlayerId& playerId)
{
    std::size_t idx = removePlayer(playerId);
    if (idx == 0) return;

    // Is the leaving player displaying on screen
    if (playerIndex_ == idx)
    {
        playerIndex_ = 0;  // Revert to userPlayer
        refreshAllBets();
    }
}

//----------------------------------------------------------------
