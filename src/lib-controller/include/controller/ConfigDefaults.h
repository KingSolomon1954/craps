//----------------------------------------------------------------
//
// File: ConfigDefaults.h
//
//----------------------------------------------------------------

#pragma once

#include <string>

namespace Gen {
    class ConfigLayer;      // fwd
    class MultiLayerConfig; // fwd
}
    
namespace Ctrl {

class ConfigDefaults
{
public:
    /// @name Lifecycle
    /// @{
    /// @}

    /// @name Modifiers
    /// @{
    static void processDefaults(Gen::MultiLayerConfig& multiConfig);
    /// @}

    /// @name Observers
    /// @{
    /// @}

private:
    static void loadDefaultDirs(Gen::ConfigLayer& cfg);
    static std::string getEnvOrDefault(const std::string& envVar,
                                       const std::string& fallback);
    static void loadGameDefaults(Gen::ConfigLayer& cfg);
};

/*-----------------------------------------------------------*//**

@class ConfigDefaults

@brief Process startup defaults

Populates the MultiLayerConfig "default" layer with values.

*/

} // namespace Ctrl

//----------------------------------------------------------------

#if 0

Util/include/Poco/Util/SystemConfiguration.h:	///   - system.osName: the operating system name
Util/include/Poco/Util/SystemConfiguration.h:	///   - system.osVersion: the operating system version
Util/include/Poco/Util/SystemConfiguration.h:	///   - system.osArchitecture: the operating system architecture
Util/include/Poco/Util/SystemConfiguration.h:	///   - system.nodeName: the node (or host) name
Util/include/Poco/Util/SystemConfiguration.h:	///   - system.nodeId: system ID, based on the Ethernet address (format "xxxxxxxxxxxx")
Util/include/Poco/Util/SystemConfiguration.h:	///     of the first Ethernet adapter found on the system.
Util/include/Poco/Util/SystemConfiguration.h:	///   - system.currentDir: the current working directory
Util/include/Poco/Util/SystemConfiguration.h:	///   - system.homeDir: the user's home directory
Util/include/Poco/Util/SystemConfiguration.h:	///   - system.configHomeDir: the base directory relative to which user specific configuration files should be stored
Util/include/Poco/Util/SystemConfiguration.h:	///   - system.cacheHomeDir: the base directory relative to which user specific non-essential data files should be stored
Util/include/Poco/Util/SystemConfiguration.h:	///   - system.dataHomeDir: the base directory relative to which user specific data files should be stored
Util/include/Poco/Util/SystemConfiguration.h:	///   - system.tempHomeDir: the base directory relative to which user-specific temporary files and other file objects should be placed
Util/include/Poco/Util/SystemConfiguration.h:	///   - system.tempDir: the system's temporary directory
Util/include/Poco/Util/SystemConfiguration.h:	///   - system.configDir: the system's configuration directory
Util/include/Poco/Util/SystemConfiguration.h:	///   - system.dateTime: the current UTC date and time, formatted in ISO 8601 format.
Util/include/Poco/Util/SystemConfiguration.h:	///   - system.pid: the current process ID.
Util/include/Poco/Util/SystemConfiguration.h:	///   - system.env.<NAME>: the environment variable with the given <NAME>.
Util/include/Poco/Util/SystemConfiguration.h:	/// An attempt to call keys("system.env") will return an empty range.

Upon initialization, Poco::Util::Application populates the configuration with the following keys:

    application.path

        The absolute path to the application executable.

    application.name

        The file name of the application executable.

    application.baseName

        The file name of the application executable without the extension.

    application.dir

        The directory where the application executable resides.

    application.configDir

        The directory where user-specific configuration files should be stored. If loadConfiguration() has not been called, this defaults to application.dir.

    application.cacheDir

        The directory for user-specific non-essential data files.

    application.dataDir

        The directory for user-specific data files.

    application.tempDir

        The directory for user-specific temporary files.

#endif    
