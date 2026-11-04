//----------------------------------------------------------------
//
// File: PlayerFile.h
//
//----------------------------------------------------------------

#pragma once

#include <filesystem>
#include <yaml-cpp/yaml.h>

namespace Ctrl {

class PlayerFile
{
public:
    /// @name Lifecycle
    /// @{
    explicit PlayerFile(std::filesystem::path path);
    /// @}

    /// @name Modifiers
    /// @{
    void load();                                   // throws std::runtime_error
    void setPlayerNode(const YAML::Node& node);
    void setPrefsNode (const YAML::Node& node);
    /// @}

    /// @name Observers
    /// @{
    void save() const;                             // throws std::runtime_error
    YAML::Node getPlayerNode() const;
    YAML::Node getPrefsNode()  const;
    const std::filesystem::path& getPath() const;
    /// @}

private:
    std::filesystem::path path_;
    YAML::Node            root_;
};

/*-----------------------------------------------------------*//**

@class PlayerFile

@brief One player's YAML file, held as a document

Owns the path and the root YAML node of a single player's file. It
knows how to read and write the file and nothing about what is in the
sections. The file has two top-level sections:

@code
Player:     # Craps::Player::toYAML(); read by Craps::Player::fromYAML()
  playerId: ...
  Bank: ...
Prefs:      # PlayerPrefs::toYAML(); optional
  LastFills: [...]
@endcode

@li load() reads the file and keeps the whole document. The "Player"
    section is required; "Prefs" is optional.
@li setPlayerNode() and setPrefsNode() replace a section wholesale. Any
    other top-level section found in the file is kept and written back.
@li save() writes the document to a temporary file next to the real one
    and renames it over the real one, so a failed or interrupted write
    can never leave a half-written player file behind.
@li getPlayerNode() and getPrefsNode() return copies, since
    YAML::Node has reference semantics. A missing section comes back as
    a null node.

All failures are std::runtime_error with the file's path in the message.

*/

} // namespace Ctrl

//----------------------------------------------------------------
