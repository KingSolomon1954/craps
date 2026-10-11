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
    static std::string nameToFileName(const std::string& name);
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

std::cout << nameToFileName("Nathan") << "\n"; 
// → "nathan.yaml"

std::cout << nameToFileName("  Arthur  McQueen ") << "\n"; 
// → "arthur-mcqueen.yaml"

std::cout << nameToFileName("Lenny #42") << "\n"; 
// → "lenny-42.yaml"

@endcode

*/

}  // namespace Gen

//---------------------------------------------------------------

