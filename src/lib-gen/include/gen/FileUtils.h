//---------------------------------------------------------------
//
// File: FileUtils.h
//
//---------------------------------------------------------------

#pragma once

#include <fstream>
#include <filesystem>
#include <string>

namespace Gen
{

class FileUtils
{
public:
    static std::ifstream openOrThrow(const std::filesystem::path& path);
    static std::ifstream openOrThrow(const std::string& path);        
};

/*-----------------------------------------------------------*//**

@class FileUtils

@brief Some useful static functions for working with files.

Example usage:
@code
#include <yaml-cpp/yaml.h>

YAML::Node loadYamlFromFile(const std::filesystem::path& path)
{
    std::ifstream fin = Gen::FileUtils::openOrThrow(path);
    return YAML::Load(fin);
}
@endcode

*/

}  // namespace Gen

//---------------------------------------------------------------

