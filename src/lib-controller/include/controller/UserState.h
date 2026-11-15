//----------------------------------------------------------------
//
// File: UserState.h
//
//----------------------------------------------------------------

#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace Gen {
    class ConfigLayer;       // fwd
    class MultiLayerConfig;  // fwd
}

namespace Ctrl {

class UserState
{
public:
    static constexpr auto FileName   = "state.yaml";
    static constexpr size_t MaxStartPlayers = 6;

    // What is remembered between runs.
    std::string              table;        // file name, e.g. las-vegas-strip.yaml
    std::string              userPlayer;   // file name of the user player
    std::vector<std::string> players;      // file names of players at the table

    /// @name Modifiers
    /// @{
    static void loadInto(const Gen::MultiLayerConfig& multiConfig,
                         Gen::ConfigLayer& cfg);
    /// @}

    /// @name Observers
    /// @{
    void save(const std::filesystem::path& stateDir) const; // throws
    static bool read(const std::filesystem::path& file,
                     const std::filesystem::path& tablesDir,
                     const std::filesystem::path& playersDir,
                     UserState& state,
                     std::string& whyNot);
    /// @}
};

/*-----------------------------------------------------------*//**

@class UserState

@brief What the game remembers between runs: last table and players

Kept in state.yaml in the user state directory
($XDG_STATE_HOME/royalcraps, usually ~/.local/state/royalcraps). The
game writes it at quit. It is never shipped, and its absence is normal:
the starting table and players then come from the user's config.yaml.

The file mirrors the keys of config.yaml so it can be loaded into the
same configuration layer, after config.yaml, and override it:

@code
table:
  startTable: las-vegas-strip.yaml
  userIsPlayer: arthur.yaml
  startPlayers:               # always six entries, "" for unused
    - arthur.yaml
    - paige.yaml
    - ""
    - ""
    - ""
    - ""
@endcode

A state file must never stop the game from starting. read() checks that
the table file and every player file exist in the user's directories and
that the user player is one of the players. If anything is wrong,
including unparseable YAML, the whole file is ignored and the user's
config.yaml values apply.

*/

} // namespace Ctrl

//----------------------------------------------------------------
