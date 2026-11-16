//----------------------------------------------------------------
//
// File: ConfigSeed.cpp
//
//----------------------------------------------------------------

#include <controller/ConfigSeed.h>
#include <stdexcept>
#include <string>
#include <system_error>
#include <controller/ConfigManager.h>
#include <gen/Logger.h>
#include <gen/MultiLayerConfig.h>

using namespace Ctrl;
namespace fs = std::filesystem;

namespace {

fs::path
dirKey(const Gen::MultiLayerConfig& cfg, const char* key)
{
    return fs::path(cfg.getString(key).value());
}

} // namespace

/*-----------------------------------------------------------*//**

True if the user has no config.yaml, meaning this is the first startup.

*/
bool
ConfigSeed::isFirstRun(const Gen::MultiLayerConfig& multiConfig)
{
    std::error_code ec;
    return !fs::exists(
        dirKey(multiConfig, ConfigManager::KeyDirsUsrConfig) / UserConfigFile,
        ec);
}

/*-----------------------------------------------------------*//**

True if any of the files that mark a fully populated user area is
missing.

*/
bool
ConfigSeed::needsSeeding(const Gen::MultiLayerConfig& multiConfig)
{
    std::error_code ec;
    return isFirstRun(multiConfig) ||
        !fs::exists(dirKey(multiConfig, ConfigManager::KeyDirsUsrPlayers)
                    / PlayersManifest, ec) ||
        !fs::exists(dirKey(multiConfig, ConfigManager::KeyDirsUsrTables)
                    / TablesManifest, ec);
}

/*-----------------------------------------------------------*//**

Populates the user directories from the packaged ones if needed.

@param[in] multiConfig
    Fully resolved directory keys must be present, and the user
    directories must exist.
@return
    true if this is a first run (no user config.yaml at startup).
@throw std::runtime_error
    If a packaged file is missing or a copy fails.
*/
bool
ConfigSeed::seedUserFiles(const Gen::MultiLayerConfig& multiConfig)
{
    const bool firstRun = isFirstRun(multiConfig);
    if (!needsSeeding(multiConfig))
    {
        return firstRun;
    }

    using K = ConfigManager;
    LOG_INFO(std::string(firstRun ? "First startup" : "Repairing user files") +
             ": populating user directories from " +
             multiConfig.getString(K::KeyDirsSysShared).value_or("?"));
    copyMissing(dirKey(multiConfig, K::KeyDirsSysTables),
                dirKey(multiConfig, K::KeyDirsUsrTables));
    copyMissing(dirKey(multiConfig, K::KeyDirsSysPlayers),
                dirKey(multiConfig, K::KeyDirsUsrPlayers));
    copyMissing(dirKey(multiConfig, K::KeyDirsSysAudio),
                dirKey(multiConfig, K::KeyDirsUsrAudio));
    copyMissing(dirKey(multiConfig, K::KeyDirsSysImages),
                dirKey(multiConfig, K::KeyDirsUsrImages));

    // The user's config.yaml comes last. It is the marker that
    // everything else is in place.
    const fs::path src = dirKey(multiConfig, K::KeyDirsSysConfig)
                         / DefaultConfigFile;
    const fs::path dst = dirKey(multiConfig, K::KeyDirsUsrConfig)
                         / UserConfigFile;
    std::error_code ec;
    if (!fs::exists(src, ec))
    {
        throw std::runtime_error("Packaged default configuration not "
            "found: " + src.string() + ". The installation is incomplete; "
            "use --sys-config-dir to point at the packaged files.");
    }
    if (!fs::exists(dst, ec))
    {
        const fs::path tmp = dst.string() + ".seed.tmp";
        fs::copy_file(src, tmp, fs::copy_options::overwrite_existing, ec);
        if (!ec)
        {
            fs::rename(tmp, dst, ec);
        }
        if (ec)
        {
            throw std::runtime_error("Unable to create " + dst.string() +
                                     "; " + ec.message());
        }
        LOG_INFO("Created " + dst.string());
    }
    return firstRun;
}

/*-----------------------------------------------------------*//**

Copies each regular file in srcDir to dstDir unless dstDir already has
one of that name. Files whose names begin with '.' are skipped. A source
directory that does not exist is not an error, and nothing is done if
both directories are the same directory.

@throw std::runtime_error
    If a copy fails.
*/
void
ConfigSeed::copyMissing(const fs::path& srcDir, const fs::path& dstDir)
{
    std::error_code ec;
    if (!fs::is_directory(srcDir, ec))
    {
        return;
    }
    fs::create_directories(dstDir, ec);
    if (ec)
    {
        throw std::runtime_error("Unable to create " + dstDir.string() +
                                 "; " + ec.message());
    }
    if (fs::equivalent(srcDir, dstDir, ec))
    {
        return;
    }

    for (const auto& entry : fs::directory_iterator(srcDir, ec))
    {
        if (!entry.is_regular_file())
        {
            continue;
        }
        const std::string name = entry.path().filename().string();
        if (name.empty() || name[0] == '.' ||
            name.ends_with(".seed.tmp"))
        {
            continue;
        }

        const fs::path dst = dstDir / name;
        std::error_code e2;
        if (fs::exists(dst, e2))
        {
            continue;                       // Never overwrite
        }

        const fs::path tmp = dst.string() + ".seed.tmp";
        fs::copy_file(entry.path(), tmp,
                      fs::copy_options::overwrite_existing, e2);
        if (!e2)
        {
            fs::rename(tmp, dst, e2);
        }
        if (e2)
        {
            throw std::runtime_error("Unable to copy " +
                entry.path().string() + " to " + dst.string() +
                "; " + e2.message());
        }
        LOG_INFO("Created " + dst.string());
    }
    if (ec)
    {
        throw std::runtime_error("Unable to read " + srcDir.string() +
                                 "; " + ec.message());
    }
}

//----------------------------------------------------------------
