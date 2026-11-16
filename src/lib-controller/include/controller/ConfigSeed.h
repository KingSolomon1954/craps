//----------------------------------------------------------------
//
// File: ConfigSeed.h
//
//----------------------------------------------------------------

#pragma once

#include <filesystem>

namespace Gen {
    class MultiLayerConfig; // fwd
}

namespace Ctrl {

class ConfigSeed
{
public:
    /// @name Modifiers
    /// @{
    static bool seedUserFiles(const Gen::MultiLayerConfig& multiConfig);
    /// @}

    /// @name Observers
    /// @{
    static bool isFirstRun(const Gen::MultiLayerConfig& multiConfig);
    static bool needsSeeding(const Gen::MultiLayerConfig& multiConfig);
    /// @}

    /// @name Helpers (public so unit tests can reach them)
    /// @{
    static void copyMissing(const std::filesystem::path& srcDir,
                            const std::filesystem::path& dstDir);
    /// @}

    static constexpr auto DefaultConfigFile = "default-config.yaml";
    static constexpr auto UserConfigFile    = "config.yaml";
    static constexpr auto PlayersManifest   = "players.yaml";
    static constexpr auto TablesManifest    = "tables.yaml";
};

/*-----------------------------------------------------------*//**

@class ConfigSeed

@brief Populates the user's directories from the packaged defaults

The packaged files under dirs.sysshared are read-only and shared. The
game needs writable copies of its tables and players (they hold bank
balances and statistics), so on first startup the packaged files are
copied into the user's directories.

Seeding is needed when any of these is missing from the user's
directories:

@li config.yaml (user config directory)
@li players.yaml (user players directory)
@li tables.yaml (user tables directory)

Every startup after the first finds all three and does nothing. The
manifests count as markers too, so a user who removes their data
directory gets a working game back on the next start rather than an
error.

What seeding does:

@li Copies every regular file from the packaged tables, players, audio
and images directories into the matching user directory, but only the
files that are missing there. An existing file is never overwritten, so
a player's bank and statistics survive no matter what.
@li Each file is copied to a temporary name and renamed into place, so
an interrupted copy never leaves a partial file.
@li Copies config/default-config.yaml to the user's config.yaml, last.
A crash part way through therefore leaves the user without a config.yaml
and the next startup repeats the (idempotent) work.
@li Does nothing for a directory pair that is the same directory, which
is the case when the command line points the user directories at the
packaged ones (typical during development).

First run is defined as: user config.yaml did not exist at startup.

*/

} // namespace Ctrl

//----------------------------------------------------------------
