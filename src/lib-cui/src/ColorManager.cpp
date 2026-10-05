//----------------------------------------------------------------
//
// File: ColorManager.cpp
//
//----------------------------------------------------------------

#include <cui/ColorManager.h>
#include <controller/CrapsReaders.h>
#include <ncurses.h>
#include <cassert>

using namespace Cui;

//----------------------------------------------------------------

ColorManager::ColorManager()
{
    initColorPairs();
    initPlayerColors();
}

//----------------------------------------------------------------

ColorManager&
ColorManager::instance()
{
    static ColorManager cm;
    return cm;
}

//----------------------------------------------------------------

void
ColorManager::initialize()
{
    initColorPairs();
    initPlayerColors();
}

//----------------------------------------------------------------

void
ColorManager::initColorPairs()
{
    init_pair(DefaultScreen,  COLOR_GREEN,  COLOR_BLACK);
    init_pair(SevenOut,       COLOR_RED,     -1);
    init_pair(PassLineWinner, COLOR_YELLOW,  -1);
    init_pair(Point,          COLOR_MAGENTA, -1);

    init_pair(PlayerColor1, COLOR_WHITE,   -1);
    init_pair(PlayerColor2, COLOR_MAGENTA, -1);
    init_pair(PlayerColor3, COLOR_YELLOW,  -1);
    init_pair(PlayerColor4, COLOR_BLUE,    -1);
    init_pair(PlayerColor5, COLOR_RED,     -1);
    init_pair(PlayerColor6, COLOR_CYAN,    -1);

    bkgd(' ' | COLOR_PAIR(DefaultScreen));
}

//----------------------------------------------------------------

void
ColorManager::initPlayerColors()
{
    Gen::ErrorPass ep;
    Craps::TableId tableId;
    
    auto rc = Ctrl::CrapsReaders::getActiveCrapsTable(tableId, ep);
    if (rc == Gen::ReturnCode::Fail)
    {
        ep.prepend("ColorManager::initPlayerColors(1) unable to init; ");
        throw std::runtime_error(ep.description());
    }

    std::vector<Craps::PlayerId> ids;
    rc = Ctrl::CrapsReaders::readTablePlayers(tableId, ids, ep);
    if (rc == Gen::ReturnCode::Fail)
    {
        ep.prepend("ColorManager::initPlayerColors(2) unable to init; ");
        throw std::runtime_error(ep.description());
    }

    for (const auto& id : ids)
    {
        assignPlayerColor(id);
    }
}

//----------------------------------------------------------------

short
ColorManager::pair(PlayerColor color) const
{
    return PlayerColorPairs[static_cast<std::size_t>(color)];
}

//----------------------------------------------------------------

short
ColorManager::pair(Color color) const
{
    switch (color)
    {
        case Color::DefaultScreen:
            return DefaultScreen;

        case Color::SevenOut:
            return SevenOut;

        case Color::PassLineWinner:
            return PassLineWinner;

        case Color::Point:
            return Point;
    }

    return DefaultScreen;
}

//----------------------------------------------------------------
//
// Assign the next available player color.
//
// If the player already has a color, that color is returned.
// Returns std::nullopt if all player colors are currently assigned.
//
std::optional<PlayerColor>
ColorManager::assignPlayerColor(const Craps::PlayerId& playerId)
{
    // Already assigned?
    if (const auto color = getPlayerColor(playerId);
        color.has_value())
    {
        return color;
    }

    // Find the first available color slot.
    for (std::size_t i = 0; i < playerAssignments_.size(); ++i)
    {
        if (!playerAssignments_[i].has_value())
        {
            const auto color = static_cast<PlayerColor>(i);

            playerAssignments_[i] = PlayerColorAssignment{
                .playerId = playerId,
                .color = color
            };

            return color;
        }
    }

    // All six colors are currently assigned.
    return std::nullopt;
}

//----------------------------------------------------------------

std::optional<PlayerColor>
ColorManager::getPlayerColor(const Craps::PlayerId& playerId) const
{
    for (const auto& assignment : playerAssignments_)
    {
        if (assignment.has_value() &&
            assignment->playerId == playerId)
        {
            return assignment->color;
        }
    }

    return std::nullopt;
}

//----------------------------------------------------------------
//
// Release the player's color assignment.
//
void
ColorManager::releasePlayerColor(const Craps::PlayerId& playerId)
{
    for (auto& assignment : playerAssignments_)
    {
        if (assignment.has_value() &&
            assignment->playerId == playerId)
        {
            assignment.reset();
            return;
        }
    }
}

//----------------------------------------------------------------

void
ColorManager::clearPlayerColors()
{
    for (auto& assignment : playerAssignments_)
        assignment.reset();
}

//----------------------------------------------------------------

void
ColorManager::onPlayerJonedTable(const Craps::PlayerId& playerId)
{
    auto pc = assignPlayerColor(playerId);
    assert(pc != std::nullopt);
}

//----------------------------------------------------------------

void
ColorManager::onPlayerLeftTable(const Craps::PlayerId& playerId)
{
    releasePlayerColor(playerId);
}

//----------------------------------------------------------------
