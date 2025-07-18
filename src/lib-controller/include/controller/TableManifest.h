//----------------------------------------------------------------
//
// File: TableManifest.h
//
//----------------------------------------------------------------

#pragma once

#include <vector>
#include <craps/CrapsTable.h>
#include <yaml-cpp/yaml.h>

namespace Gen {
    class ErrorPass;  // fwd
}

namespace Ctrl {

class TableManifest
{
public:
    struct ManifestRecord
    {
        Craps::CrapsTable::TableId tableId;
        std::string tableName;
        std::string shortDescription;
        std::string fullDescription;
        std::string fileName;
        
        YAML::Node toYAML() const;
        void fromYAML(const YAML::Node& node);
    };
    
    /// @name Lifecycle
    /// @{
    /// @}

    /// @name Modifiers
    /// @{
    /// @}

    /// @name Observers
    /// @{
    const std::vector<ManifestRecord>& getManifest(Gen::ErrorPass& ep);
    /// @}

private:
    std::vector<ManifestRecord> records_;
    
    void loadFile(dir);
    YAML::Node toYAML() const;
    void fromYAML(const YAML::Node& node);
};

/*-----------------------------------------------------------*//**

@class CrapsGame

@brief Craps Game Top Level class.

Responsibiities:

* initialize Configuration
* initializes Globals

*/

} // namespace Ctrl

//----------------------------------------------------------------
