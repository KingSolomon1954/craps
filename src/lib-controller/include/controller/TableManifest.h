//----------------------------------------------------------------
//
// File: TableManifest.h
//
//----------------------------------------------------------------

#pragma once

#include <vector>
#include <craps/CrapsTypes.h>
#include <yaml-cpp/yaml.h>

namespace Ctrl {

class TableManifest
{
public:
    struct TableInfo
    {
        Craps::TableId tableId;
        std::string tableName;
        std::string shortDescription;
        std::string fullDescription;
        std::string fileName;

        YAML::Node toYAML() const;
        void fromYAML(const YAML::Node& node);
    };

    std::string getFileName(const Craps::TableId& tableId)   const;
    Craps::TableId getTableId(const std::string& fileName)   const;
    void loadFromFile();  // throws
    const std::vector<TableManifest::TableInfo>& getTables() const;
    void fromYAML(const YAML::Node& node);

private:
    std::vector<TableInfo> tables_;
};

/*-----------------------------------------------------------*//**

@class TableManifest

@brief Reads in table.yaml, populates tables_ vector.

*/

} // namespace Ctrl

//----------------------------------------------------------------
