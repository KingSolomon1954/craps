//----------------------------------------------------------------
//
// File: PlayerAreaOnePlayer.cpp
//
//----------------------------------------------------------------

#include <cui/panels/PlayerAreaOnePlayer.h>
#include <cui/ColorManager.h>
#include <controller/CrapsReaders.h>
#include <gen/ErrorPass.h>
#include <gen/Logger.h>
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
    mvwaddch(pWin_, L::fieldBorderTopRow,    L::col4_5,       ACS_BTEE);
    mvwaddch(pWin_, L::fieldBorderTopRow,    L::col5_6,       ACS_BTEE);
    mvwaddch(pWin_, L::fieldBorderTopRow,    L::col6_8,       ACS_BTEE);
    mvwaddch(pWin_, L::fieldBorderTopRow,    L::col8_9,       ACS_BTEE);
    mvwaddch(pWin_, L::fieldBorderTopRow,    L::col9_10,      ACS_BTEE);
    mvwaddch(pWin_, L::lineBetsBorderTopRow, L::colComeDont2, ACS_TTEE);
}

//----------------------------------------------------------------

void
PlayerAreaOnePlayer::drawStaticContent()
{
    drawName();
    drawLabels();
}

//----------------------------------------------------------------

void
PlayerAreaOnePlayer::drawName()
{
    wattron(pWin_, COLOR_PAIR(players_[playerIndex_].colorPair));
    mvwaddstr(pWin_, 0, 1, players_[playerIndex_].name.c_str());
    waddch(pWin_, ':');
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
        drawOneBet(b);
    }
}

//----------------------------------------------------------------

void
PlayerAreaOnePlayer::drawOneBet(const Bet& b)
{
    mvwaddstr(pWin_, b.row, b.col, std::to_string(b.amount).c_str());
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
        auto betName        = getBetName(betId);
        auto pivot          = getPivot(betId);
        auto contractAmount = getContractAmount(betId);
        auto oddsAmount     = getOddsAmount(betId);

        int idx = getBetIndex(betName, pivot);
        Gen::Money amount = pivot == 0 ? contractAmount : oddsAmount;
        bets_[idx].betId  = betId;
        bets_[idx].amount = amount;
        drawOneBet(bets_[idx]);
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

    refreshAllBets();
    drawBets();
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
    (void)betId;

    if (playerId != players_[playerIndex_].playerId) return;
    
    int idx = getBetIndex(betName, pivot);
    if (idx == bets_.size())
        return;

    bets_[idx].betId = betId;
    Gen::Money amount = pivot == 0 ? contractAmount : oddsAmount;
    bets_[idx].amount = amount;
    
    drawOneBet(bets_[idx]);
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
    if (playerId != players_[playerIndex_].playerId) return;

    for (auto& b : bets_)
    {
        if (b.betId == betId)
        {
            b.amount = 0;
            b.betId  = 0;
            drawOneBet(b);
            return;
        }
    }
    LOG_ERROR("PlayerAreaOnePlayer::onBetResolved(): "
              "did not find the resolved bet on the screen.");
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
        drawBets();
    }
}

//----------------------------------------------------------------
