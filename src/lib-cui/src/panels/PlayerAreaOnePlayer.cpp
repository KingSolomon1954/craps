//----------------------------------------------------------------
//
// File: PlayerAreaOnePlayer.cpp
//
//----------------------------------------------------------------

#include <cui/panels/PlayerAreaOnePlayer.h>
#include <cui/CuiUtils.h>
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
PlayerAreaOnePlayer::init(
    WINDOW* pWin,
    const std::vector<Craps::PlayerId>& playerIds)
{
    pWin_ = pWin;
    buildPlayerInfo(playerIds);
}

//----------------------------------------------------------------

void
PlayerAreaOnePlayer::buildPlayerInfo(
    const std::vector<Craps::PlayerId>& playerIds)
{
    players_.clear();
    players_.reserve(MaxPlayers);

    Craps::PlayerId userPlayerId;
    Gen::ErrorPass ep;

    auto rc = Ctrl::CrapsReaders::getUserPlayer(userPlayerId, ep);
    assert(rc == Gen::ReturnCode::Success);

    std::vector<Craps::PlayerId> orderedIds;
    orderedIds.reserve(MaxPlayers);

    // UserPlayer always gets index 0.
    orderedIds.push_back(userPlayerId);

    // Preserve the existing order for everyone else.
    for (const auto& pid : playerIds)
    {
        if (pid != userPlayerId)
            orderedIds.push_back(pid);

        if (orderedIds.size() == MaxPlayers)
            break;
    }

    for (const auto& playerId : orderedIds)
    {
        players_.push_back(
        {
            .playerId  = playerId,
            .name      = playerName(playerId),
            .colorPair = playerColorPair(playerId)
        });
    }
}

//----------------------------------------------------------------

short
PlayerAreaOnePlayer::playerColorPair(const Craps::PlayerId& id) const
{
    const auto pColor = ColorManager::instance().getPlayerColor(id);
    return ColorManager::instance().pair(*pColor);
}

//----------------------------------------------------------------

std::string
PlayerAreaOnePlayer::playerName(const Craps::PlayerId& id) const
{
    Gen::ErrorPass ep;
    std::string name;

    const auto rc = Ctrl::CrapsReaders::readPlayerName(id, name, ep);
    assert(rc == Gen::ReturnCode::Success);
    return name;
}

//----------------------------------------------------------------

size_t
PlayerAreaOnePlayer::removePlayer(const Craps::PlayerId& playerId)
{
    for (size_t i = 0; i < players_.size(); ++i)
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

void
PlayerAreaOnePlayer::populate()
{
    // empty
    // Bets are already updated as events are processed.
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
    CuiBetName cuiBetName = betToCuiBetName(betName, pivot);

    for (size_t i = 0; i < bets_.size(); ++i)
    {
        if (bets_[i].betName == cuiBetName)
        {
            return i;
        }
    }
    
    return bets_.size();
}

//----------------------------------------------------------------

PlayerAreaOnePlayer::CuiBetName
PlayerAreaOnePlayer::betToCuiBetName(BetName betName, unsigned pivot) const
{
    switch (betName)
    {
        case BetName::PassLine:
            switch(pivot)
            {
                case 0: return CuiBetName::PassLine;
                case 4:
                case 5:
                case 6:
                case 8:
                case 9:
                case 10: return CuiBetName::PassLineOdds;
                default: assert(false);
            }
            
        case BetName::DontPass:
            switch(pivot)
            {
                case 0: return CuiBetName::DontPass;
                case 4:
                case 5:
                case 6:
                case 8:
                case 9:
                case 10: return CuiBetName::DontPassOdds;
                default: assert(false);
            }

        case BetName::Come:
            switch(pivot)
            {
                case 0:  return CuiBetName::Come;
                case 4:  return CuiBetName::Come4;
                case 5:  return CuiBetName::Come5;
                case 6:  return CuiBetName::Come6;
                case 8:  return CuiBetName::Come8;
                case 9:  return CuiBetName::Come9;
                case 10: return CuiBetName::Come10;
                default: assert(false);
            }

        case BetName::DontCome:
            switch(pivot)
            {
                case 0:  return CuiBetName::DontCome;
                case 4:  return CuiBetName::DontCome4;
                case 5:  return CuiBetName::DontCome5;
                case 6:  return CuiBetName::DontCome6;
                case 8:  return CuiBetName::DontCome8;
                case 9:  return CuiBetName::DontCome9;
                case 10: return CuiBetName::DontCome10;
                default: assert(false);
            }

        case BetName::Place:
            switch(pivot)
            {
                case 4:  return CuiBetName::Place4;
                case 5:  return CuiBetName::Place5;
                case 6:  return CuiBetName::Place6;
                case 8:  return CuiBetName::Place8;
                case 9:  return CuiBetName::Place9;
                case 10: return CuiBetName::Place10;
                default: assert(false);
            }

        case BetName::Buy:
            switch (pivot)
            {
                case 4:  return CuiBetName::Buy4;
                case 5:  return CuiBetName::Buy5;
                case 6:  return CuiBetName::Buy6;
                case 8:  return CuiBetName::Buy8;
                case 9:  return CuiBetName::Buy9;
                case 10: return CuiBetName::Buy10;
                default: assert(false);
            }

        case BetName::Lay:
            switch (pivot)
            {
                case 4:  return CuiBetName::Lay4;
                case 5:  return CuiBetName::Lay5;
                case 6:  return CuiBetName::Lay6;
                case 8:  return CuiBetName::Lay8;
                case 9:  return CuiBetName::Lay9;
                case 10: return CuiBetName::Lay10;
                default: assert(false);
            }

        case BetName::Hardway:
            switch(pivot)
            {
                case 4:  return CuiBetName::Hard4;
                case 6:  return CuiBetName::Hard6;
                case 8:  return CuiBetName::Hard8;
                case 10: return CuiBetName::Hard10;
                default: assert(false);
            }

        case BetName::CandE: return CuiBetName::CandE;

        case BetName::Field: return CuiBetName::Field;

        case BetName::AnyCraps: return CuiBetName::AnyCraps;

        case BetName::AnySeven: return CuiBetName::Any7;

        case BetName::Horn: return CuiBetName::Horn;

        default: assert(false);
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

    Gen::Money amount = pivot == 0 ? contractAmount : oddsAmount;
    bets_[idx].amount = amount;
    
    drawOneBet(bets_[idx]);
}

//----------------------------------------------------------------

void
PlayerAreaOnePlayer::onBetResolved()
{
    // TODO
}

//----------------------------------------------------------------

void
PlayerAreaOnePlayer::onPlayerJoinedTable(
    const Craps::PlayerId& playerId)
{
    for (size_t i = 0; i < players_.size(); ++i)
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
        .playerId = playerId,
        .name = playerName(playerId),
        .colorPair = playerColorPair(playerId),
    });
}

//----------------------------------------------------------------

void
PlayerAreaOnePlayer::onPlayerLeftTable(
    const Craps::PlayerId& playerId)
{
    size_t idx = removePlayer(playerId);
    if (idx == 0) return;

    // Is the leaving player displaying on screen
    if (playerIndex_ == idx)
    {
        playerIndex_ = 0;  // Revert to userPlayer
        refreshAllBets();
        drawBets();        // update backing store
    }
}

//----------------------------------------------------------------
