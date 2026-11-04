//----------------------------------------------------------------
//
// File: PlayerFile.cpp
//
//----------------------------------------------------------------

#include <controller/PlayerFile.h>
#include <gen/FileUtils.h>
#include <gen/Logger.h>
#include <fstream>
#include <stdexcept>
#include <string>
#include <utility>

using namespace Ctrl;

namespace {

constexpr const char* KeyPlayer = "Player";
constexpr const char* KeyPrefs  = "Prefs";

} // namespace

//----------------------------------------------------------------

PlayerFile::PlayerFile(std::filesystem::path path)
    : path_(std::move(path))
{
}

//----------------------------------------------------------------
//
// Reads the file and keeps its whole document. Throws if the file
// is missing, is not valid YAML, or has no "Player" section.
//
void
PlayerFile::load()
{
    const std::string fname("PlayerFile::load(): \"" + path_.string() + "\"; ");

    YAML::Node root;
    try
    {
        std::ifstream fin = Gen::FileUtils::openOrThrow(path_);  // throws
        root = YAML::Load(fin);                                  // throws
    }
    catch (const std::exception& e)
    {
        throw std::runtime_error(fname + e.what());
    }

    if (!root.IsMap())
    {
        throw std::runtime_error(fname + "file is not a YAML map");
    }
    if (!root[KeyPlayer].IsMap())
    {
        throw std::runtime_error(fname + "missing section: \"" +
                                 std::string(KeyPlayer) + "\"");
    }
    root_ = root;
}

//----------------------------------------------------------------
//
// Writes to <path>.tmp then renames over <path>. Rename within one
// directory is atomic on Linux.
//
void
PlayerFile::save() const
{
    const std::string fname("PlayerFile::save(): \"" + path_.string() + "\"; ");

    if (path_.empty())
    {
        throw std::runtime_error(fname + "no path");
    }

    namespace fs = std::filesystem;
    fs::path tmp = path_;
    tmp += ".tmp";

    LOG_DEBUG("PlayerFile::save(" + path_.string() + ")");

    {
        std::ofstream fout(tmp, std::ios::trunc);
        if (!fout.is_open())
        {
            throw std::runtime_error(fname + "unable to open \"" +
                                     tmp.string() + "\" for writing");
        }
        fout << root_ << "\n";
        fout.flush();
        if (!fout.good())
        {
            fout.close();
            std::error_code ignore;
            fs::remove(tmp, ignore);
            throw std::runtime_error(fname + "write failed");
        }
    }

    std::error_code ec;
    fs::rename(tmp, path_, ec);
    if (ec)
    {
        std::error_code ignore;
        fs::remove(tmp, ignore);
        throw std::runtime_error(fname + "rename failed: " + ec.message());
    }
}

//----------------------------------------------------------------

void
PlayerFile::setPlayerNode(const YAML::Node& node)
{
    root_[KeyPlayer] = YAML::Clone(node);
}

//----------------------------------------------------------------

void
PlayerFile::setPrefsNode(const YAML::Node& node)
{
    root_[KeyPrefs] = YAML::Clone(node);
}

//----------------------------------------------------------------

YAML::Node
PlayerFile::getPlayerNode() const
{
    const YAML::Node n = root_[KeyPlayer];
    return n.IsDefined() ? YAML::Clone(n) : YAML::Node();
}

//----------------------------------------------------------------

YAML::Node
PlayerFile::getPrefsNode() const
{
    const YAML::Node n = root_[KeyPrefs];
    return n.IsDefined() ? YAML::Clone(n) : YAML::Node();
}

//----------------------------------------------------------------

const std::filesystem::path&
PlayerFile::getPath() const
{
    return path_;
}

//----------------------------------------------------------------
