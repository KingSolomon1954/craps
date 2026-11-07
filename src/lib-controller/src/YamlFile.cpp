//----------------------------------------------------------------
//
// File: YamlFile.cpp
//
//----------------------------------------------------------------

#include <controller/YamlFile.h>
#include <gen/FileUtils.h>
#include <gen/Logger.h>
#include <fstream>
#include <stdexcept>
#include <string>
#include <utility>

using namespace Ctrl;

//----------------------------------------------------------------

YamlFile::YamlFile(std::filesystem::path path)
    : path_(std::move(path))
{
}

//----------------------------------------------------------------
//
// Reads the file and keeps its whole document. Throws if the file
// is missing, is not valid YAML, or lacks the required section.
//
void
YamlFile::load(const std::string& requiredSection)
{
    const std::string fname("YamlFile::load(const std::string& requiredSection): \"" + path_.string() + "\"; ");

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
    if (!root[requiredSection].IsMap())
    {
        throw std::runtime_error(fname + "missing section: \"" +
                                 requiredSection + "\"");
    }
    root_ = root;
}

//----------------------------------------------------------------
//
// Writes to <path>.tmp then renames over <path>. Rename within one
// directory is atomic on Linux.
//
void
YamlFile::save() const
{
    const std::string fname("YamlFile::save(): \"" + path_.string() + "\"; ");

    if (path_.empty())
    {
        throw std::runtime_error(fname + "no path");
    }

    namespace fs = std::filesystem;
    fs::path tmp = path_;
    tmp += ".tmp";

    LOG_DEBUG("YamlFile::save(" + path_.string() + ")");

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
YamlFile::setSection(const std::string& key, const YAML::Node& node)
{
    root_[key] = YAML::Clone(node);
}

//----------------------------------------------------------------

YAML::Node
YamlFile::getSection(const std::string& key) const
{
    const YAML::Node n = root_[key];
    return n.IsDefined() ? YAML::Clone(n) : YAML::Node();
}

//----------------------------------------------------------------

const std::filesystem::path&
YamlFile::getPath() const
{
    return path_;
}

//----------------------------------------------------------------
