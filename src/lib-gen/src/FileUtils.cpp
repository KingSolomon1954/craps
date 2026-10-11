//----------------------------------------------------------------
//
// File: FileUtils.cpp
//
//----------------------------------------------------------------

#include <gen/FileUtils.h>
#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <string>

using namespace Gen;

//----------------------------------------------------------------

std::ifstream
FileUtils::openOrThrow(const std::filesystem::path& path)
{
    using namespace std;
    namespace fs = std::filesystem;

    if (!fs::exists(path))
    {
        throw std::runtime_error("File does not exist: " + path.string());
    }

    std::ifstream fin(path);
    if (!fin.is_open())
    {
        throw std::runtime_error("Failed to open file: " + path.string());
    }

    return fin;  // uses move semantics
}

//----------------------------------------------------------------

std::ifstream
FileUtils::openOrThrow(const std::string& path)
{
    namespace fs = std::filesystem;
    if (!fs::exists(path))
    {
        throw std::runtime_error("File does not exist: " + path);
    }

    std::ifstream fin(path);
    if (!fin.is_open())
    {
        throw std::runtime_error("Failed to open file: " + path);
    }

    return fin;  // moved to caller
}

//----------------------------------------------------------------

std::string
FileUtils::nameToFileName(const std::string& name)
{
    std::string fileName;
    fileName.reserve(name.size());

    // 1. Convert to lowercase and replace spaces with hyphens
    bool lastWasHyphen = false;
    for (char ch : name)
    {
        if (std::isspace(static_cast<unsigned char>(ch)))
        {
            if (!lastWasHyphen)
            {
                fileName.push_back('-');
                lastWasHyphen = true;
            }
        }
        else if (std::isalnum(static_cast<unsigned char>(ch)))
        {
            fileName.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(ch))));
            lastWasHyphen = false;
        }
        // Ignore all other punctuation/special characters
    }

    // 2. Trim leading/trailing hyphens
    while (!fileName.empty() && fileName.front() == '-')
    {
        fileName.erase(fileName.begin());
    }
    while (!fileName.empty() && fileName.back() == '-')
    {
        fileName.pop_back();
    }

    // 3. Append .yaml extension
    fileName += ".yaml";

    return fileName;
}

//----------------------------------------------------------------
