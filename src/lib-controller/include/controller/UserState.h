//----------------------------------------------------------------
//
// File: UserState.h
//
//----------------------------------------------------------------

#pragma once

#include <controller/AnimationSpeed.h>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace Gen {
    class MultiLayerConfig;  // fwd
}

namespace Ctrl {

class UserState
{
public:
    static constexpr auto   FileName        = "state.yaml";
    static constexpr size_t MaxStartPlayers = 6;                 // table seats
    static constexpr size_t MaxBuddies      = MaxStartPlayers - 1;

    // What is remembered between runs.
    std::string              table;        // file name, e.g. las-vegas-strip.yaml
    std::string              userPlayer;   // file name of the user player
    std::vector<std::string> buddies;      // file names of the other players

    // Settings the person changed while playing. Absent if never changed.
    std::optional<AnimationSpeed> animationSpeed;

    /// @name Observers
    /// @{
    static std::optional<UserState> load(
        const Gen::MultiLayerConfig& multiConfig);
    static bool read(const std::filesystem::path& file,
                     UserState& state,
                     std::string& whyNot);
    void save(const std::filesystem::path& stateDir) const; // throws
    /// @}
};

/*-----------------------------------------------------------*//**

@class UserState

@brief What the game remembers between runs: last table and players

Kept in state.yaml in the user state directory
($XDG_STATE_HOME/royalcraps, usually ~/.local/state/royalcraps). The
game writes it at quit. It is never shipped, and its absence is normal.

@code
last:
  table: las-vegas-strip.yaml
  userPlayer: arthur.yaml
  buddyPlayers:
    - paige.yaml
    - nathan.yaml
settings:
  animationSpeed: fast
@endcode

The settings section is optional. A setting in it is only used when
config.yaml does not state that setting.

state.yaml is only a memory. It says what happened last time and it
does not override anything the user wrote in config.yaml; see
StartChoices for how the two combine. This class only reads and writes
the file. It does not check that the files named exist, which is
StartChoices' job, because what to do about a missing file depends on
where the name came from.

load() never throws. A state file that can't be parsed or is missing
its sections is reported in the log and treated as absent.

*/

} // namespace Ctrl

//----------------------------------------------------------------
