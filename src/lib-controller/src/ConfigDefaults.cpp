//----------------------------------------------------------------
//
// File: ConfigDefaults.cpp
//
//----------------------------------------------------------------

#include <controller/ConfigDefaults.h>
#include <cstdlib>
#include <filesystem>
#include <gen/MultiLayerConfig.h>
#include <controller/Globals.h>
#include <controller/CrapsGame.h>

using namespace Ctrl;

/*-----------------------------------------------------------*//**

Process defaults

@param[in,out] pCfg
    The multi-layer configuration structure to populate.
*/
void
ConfigDefaults::processDefaults(Gen::ConfigLayer& cfg)
{
    loadDefaultDirs(cfg);
    loadGameDefaults(cfg);
}

//----------------------------------------------------------------

void
ConfigDefaults::loadDefaultDirs(Gen::ConfigLayer& cfg)
{
    namespace fs = std::filesystem;

    // { dirs.shared  }
    // { dirs.config  }
    // { dirs.data    }
    // { dirs.cache   }
    // { dirs.temp    }
    // { dirs.log     }
    // { dirs.runtime }
    // { dirs.tables  }
    // { dirs.players }
    
    const std::string home(fs::path(getenv("HOME")));
    
    // 1. { dirs.shared } Shared (system-wide, read-only)
    cfg.set("dirs.shared", "/usr/share/" + Gbl::appNameExec);
  
    // 2. { dirs.config } Config dir ($XDG_CONFIG_HOME or ~/.config)
    std::string configDir = getEnvOrDefault("XDG_CONFIG_HOME", home + "/.config/");
    cfg.set("dirs.config", configDir + Gbl::appNameExec);

    // 3. { dirs.data } Data dir ($XDG_DATA_HOME or ~/.local/share)
    std::string dataDir = getEnvOrDefault("XDG_DATA_HOME", home + "/.local/share/");
    cfg.set("dirs.data", dataDir + Gbl::appNameExec);

    // 4. { dirs.cache } Cache/temp dir ($XDG_CACHE_HOME or ~/.cache/)
    std::string cacheDir = getEnvOrDefault("XDG_CACHE_HOME", home + "/.cache/");
    cfg.set("dirs.cache", cacheDir + Gbl::appNameExec);

    // 5. {"dirs.temp }
    cfg.set("dirs.temp", "/tmp/" + Gbl::appNameExec);

    // 6. { dirs.log }
    cfg.set("dirs.log",  home + "/.local/state/" + Gbl::appNameExec);

    // 7. { dirs.runtime }
    std::string u = getEnvOrDefault("XDG_RUNTIME_DIR",    "not found");
    
    if (u == "not found") u = getEnvOrDefault("LOGNAME",  "not found");
    if (u == "not found") u = getEnvOrDefault("USERNAME", "not found");
    if (u == "not found") u = getEnvOrDefault("USER",     "not found");
    if (u == "not found")
    {
        // Probably running in a container, so force a made up name
        u = Gbl::appNameExec + "-player-1";
        // throw std::invalid_argument("No user name found");
    }
    cfg.set("dirs.runtime", "run/user/" + u);

    // 8. {"dirs.tables }
    std::string v = cfg.get("dirs.shared").value();
    cfg.set("dirs.tables", v + "/tables");
    
    // 9. {"dirs.players }
    cfg.set("dirs.players", v + "/players");
}
    
//----------------------------------------------------------------

void
ConfigDefaults::loadGameDefaults(Gen::ConfigLayer& cfg)
{
    CrapsGame::loadGameDefaults(cfg);
}

//----------------------------------------------------------------

std::string
ConfigDefaults::getEnvOrDefault(const std::string& envVar,
                                const std::string& fallback)
{
    const char* val = std::getenv(envVar.c_str());
    return val ? std::string(val) : fallback;
}

//----------------------------------------------------------------

#if 0

/usr/share/craps/
//    ├── tables/
//    │   ├── vegas_rules.json
//    │   ├── atlantic_city.json
//    ├── players/
//    │   ├── demo_player1.json
//    │   ├── demo_player2.json
//    ├── themes/
//    │   └── default_theme.json
//    

#endif
