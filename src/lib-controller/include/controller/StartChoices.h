//----------------------------------------------------------------
//
// File: StartChoices.h
//
//----------------------------------------------------------------

#pragma once

#include <filesystem>
#include <optional>
#include <map>
#include <string>
#include <vector>
#include <controller/PlayerManifest.h>
#include <controller/UserState.h>

namespace Gen {
    class ConfigLayer;       // fwd
    class MultiLayerConfig;  // fwd
}

namespace Ctrl {

class StartChoices
{
public:
    // Everything the decision depends on.
    struct Request
    {
        // From the user's config.yaml. Absent means the user did not say.
        std::optional<std::string>              table;
        std::optional<std::string>              userPlayer;
        std::optional<bool>                     useBuddies;
        std::optional<std::vector<std::string>> buddies;

        // From state.yaml, if there is one.
        std::optional<UserState>                last;

        // Packaged defaults, used when neither of the above applies.
        std::string                             defaultTable;
        std::string                             defaultUserPlayer;
        std::vector<std::string>                defaultBuddies;

        // Where the files named above must exist.
        std::filesystem::path                   tablesDir;
        std::filesystem::path                   playersDir;

        // File name -> kind, from players.yaml. When not empty, the user
        // player must be a user player, buddies must be buddy players,
        // and all must be in it. When empty nothing is checked.
        std::map<std::string, PlayerKind>       kinds;
    };

    // The answer.
    struct Result
    {
        std::string              table;
        std::string              userPlayer;
        std::vector<std::string> buddies;  // never includes userPlayer
    };

    /// @name Modifiers
    /// @{
    static void apply(const Gen::MultiLayerConfig& multiConfig,
                      Gen::ConfigLayer& cfg);
    /// @}

    /// @name Observers
    /// @{
    static Result resolve(const Request& request);
    /// @}
};

/*-----------------------------------------------------------*//**

@class StartChoices

@brief Decides which table and players the game starts with

Three sources can speak, and they have a fixed order of authority:

@li The user's config.yaml: what the user wants, always.
@li state.yaml: what the last game left, used for whatever config.yaml
did not say.
@li The packaged defaults in sys-config.yaml: used on a fresh install,
when neither of the others has an opinion.

Each setting is decided independently:

@li Table: startWithTable, else last table, else default table.
@li User player: startWithUserPlayer, else last user player, else
default user player.
@li Buddies: if startWithBuddyPlayers is false there are none.
Otherwise startWithTheseBuddyPlayers, else the last buddies, else the
default buddies. A state file that exists remembers an empty list
faithfully, it is not replaced by the defaults.

A name that is not a file in the user's tables or players directory is
skipped with a warning in the log, and the next source is tried (for
buddies, the one buddy is dropped). The user player is never also a
buddy and there are at most UserState::MaxBuddies buddies. The game must
always be able to start, so the worst a wrong name in a hand-edited
config.yaml can do is cost the user what they asked for.

apply() puts the answer under the keys the rest of the game reads:
table.startTable, table.userIsPlayer and table.startPlayers[0..5], the
user player first.

*/

} // namespace Ctrl

//----------------------------------------------------------------
