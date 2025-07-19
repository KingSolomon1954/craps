//----------------------------------------------------------------
//
// File: TableManifestTest.cpp
//
//----------------------------------------------------------------

#include <controller/TableManifest.h>
#include <string>
#include <sstream>
#include <yaml-cpp/yaml.h>
#include <doctest/doctest.h>

using Ctrl::TableManifest;

//----------------------------------------------------------------

TEST_CASE("TableManifest:fromYAML")
{
    const std::string yamlText = R"(
tables:
  - tableId: 1
    tableName: Las Vegas
    shortDescription: Fast-paced table
    fullDescription: Full description here...
    fileName: Table-1.yaml
  - tableId: 2
    tableName: Atlantic City
    shortDescription: Slower table
    fullDescription: Another description...
    fileName: Table-2.yaml
)";

    YAML::Node root = YAML::Load(yamlText);

    TableManifest manifest;
    manifest.fromYAML(root["tables"]);

    const auto& tables = manifest.getTables();
    REQUIRE(tables.size() == 2);

    SUBCASE("First table entry is correct")
    {
        const auto& t1 = tables[0];
        CHECK(t1.tableId          == "1");
        CHECK(t1.tableName        == "Las Vegas");
        CHECK(t1.shortDescription == "Fast-paced table");
        CHECK(t1.fullDescription  == "Full description here...");
        CHECK(t1.fileName         == "Table-1.yaml");
    }

    SUBCASE("Second table entry is correct")
    {
        const auto& t2 = tables[1];
        CHECK(t2.tableId          == "2");
        CHECK(t2.tableName        == "Atlantic City");
        CHECK(t2.shortDescription == "Slower table");
        CHECK(t2.fullDescription  == "Another description...");
        CHECK(t2.fileName         == "Table-2.yaml");
    }
}


//----------------------------------------------------------------
