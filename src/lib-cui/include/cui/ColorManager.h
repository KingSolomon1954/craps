//----------------------------------------------------------------
//
// File: ColorManager.h
//
//----------------------------------------------------------------

#pragma once

#include <array>
#include <cstddef>
#include <optional>
#include <craps/CrapsTypes.h>

namespace Cui
{

enum class Color
{
    DefaultScreen,
    SevenOut,
    PassLineWinner,
    Point
};

enum class PlayerColor : std::size_t
{
    One = 0,
    Two,
    Three,
    Four,
    Five,
    Six
};

class ColorManager
{
public:
    ColorManager();
    static ColorManager& instance();

    ColorManager(const ColorManager&) = delete;
    ColorManager& operator=(const ColorManager&) = delete;

    // Must be called after ncurses start_color().
    void initialize();

    // Return the ncurses color-pair for a general CUI color.
    short pair(Color color) const;

    // Return the ncurses color-pair for a player color.
    short pair(PlayerColor color) const;

    // Return the player's currently assigned color, if any.
    std::optional<PlayerColor> getPlayerColor(
        const Craps::PlayerId& playerId) const;

    // Release all player color assignments.
    void clearPlayerColors();

    void onPlayerJonedTable(const Craps::PlayerId& playerId);
    void onPlayerLeftTable (const Craps::PlayerId& playerId);

    
private:
    static constexpr std::size_t MaxPlayerColors = 6;

    static constexpr short DefaultScreen  = 1;
    static constexpr short SevenOut       = 2;
    static constexpr short PassLineWinner = 3;
    static constexpr short Point          = 4;

    static constexpr short PlayerColor1 = 5;
    static constexpr short PlayerColor2 = 6;
    static constexpr short PlayerColor3 = 7;
    static constexpr short PlayerColor4 = 8;
    static constexpr short PlayerColor5 = 9;
    static constexpr short PlayerColor6 = 10;

    static constexpr std::array<short, MaxPlayerColors>
        PlayerColorPairs{
            PlayerColor1,
            PlayerColor2,
            PlayerColor3,
            PlayerColor4,
            PlayerColor5,
            PlayerColor6
        };

    struct PlayerColorAssignment
    {
        Craps::PlayerId playerId;
        PlayerColor color;
    };

    std::array<std::optional<PlayerColorAssignment>,
               MaxPlayerColors> playerAssignments_{};

private:
    void initColorPairs();
    void initPlayerColors();
    std::optional<PlayerColor> assignPlayerColor(
        const Craps::PlayerId& playerId);
    void releasePlayerColor(const Craps::PlayerId& playerId);
};

} // namespace Cui

//----------------------------------------------------------------
