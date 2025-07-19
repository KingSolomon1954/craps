//----------------------------------------------------------------
//
// File: FileUtils.cpp
//
//----------------------------------------------------------------

#include <gen/FileUtils.h>
#include <stdexcept>

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

