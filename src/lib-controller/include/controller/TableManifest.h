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
    struct TableDescription
    {
        Craps::TableId tableId;
        std::string tableName;
        std::string shortDescription;
        std::string fullDescription;
        std::string fileName;

        YAML::Node toYAML() const;
        void fromYAML(const YAML::Node& node);
    };

    using TableList = std::vector<TableManifest::TableDescription>;
    
    std::string getFileName(const Craps::TableId& tableId) const;
    Craps::TableId getTableId(const std::string& fileName) const;
    const TableList& getTableList()                        const;
    void loadFromFile();  // throws
    void fromYAML(const YAML::Node& node);

private:
    TableList tables_;
};

/*-----------------------------------------------------------*//**

@class TableManifest

@brief Reads in table.yaml, populates tables_ vector.

*/

} // namespace Ctrl

//----------------------------------------------------------------
