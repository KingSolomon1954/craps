//----------------------------------------------------------------
//
// File: YamlFile.h
//
//----------------------------------------------------------------

#pragma once

#include <filesystem>
#include <string>
#include <yaml-cpp/yaml.h>

namespace Ctrl {

class YamlFile
{
public:
    /// @name Lifecycle
    /// @{
    explicit YamlFile(std::filesystem::path path);
    /// @}

    /// @name Modifiers
    /// @{
    void load(const std::string& requiredSection);  // throws std::runtime_error
    void setSection(const std::string& key, const YAML::Node& node);
    /// @}

    /// @name Observers
    /// @{
    void save() const;                              // throws std::runtime_error
    YAML::Node getSection(const std::string& key) const;
    const std::filesystem::path& getPath() const;
    /// @}

private:
    std::filesystem::path path_;
    YAML::Node            root_;
};

/*-----------------------------------------------------------*//**

@class YamlFile

@brief One YAML file, held as a document made of named sections

Owns the path and the root YAML node of a single file. It knows how to
read and write the file and nothing about what is in the sections. A
file is a map of top-level sections, for example a player file:

@code
Player:     # Craps::Player::toYAML(); read by Craps::Player::fromYAML()
  playerId: ...
  Bank: ...
Prefs:      # PlayerPrefs::toYAML(); optional
  LastFills: [...]
@endcode

and a table file, which has just the one:

@code
Table:      # Craps::CrapsTable::toYAML()
  tableId: ...
  Rules: ...
@endcode

The model classes only ever see their own section. The managers
(PlayerManager, TableManager) own the YamlFile, decide which sections a
file has, and decide when to read and write it.

@li load() reads the file and keeps the whole document. The one section
    the caller names is required, anything else is optional.
@li setSection() replaces a section wholesale. Any other top-level
    section found in the file is kept and written back.
@li save() writes the document to a temporary file next to the real one
    and renames it over the real one, so a failed or interrupted write
    can never leave a half-written file behind.
@li getSection() returns a copy, since YAML::Node has reference
    semantics. A missing section comes back as a null node.

All failures are std::runtime_error with the file's path in the message.

*/

} // namespace Ctrl

//----------------------------------------------------------------
