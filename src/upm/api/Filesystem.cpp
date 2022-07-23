#include "Filesystem.hpp"

#include <stc/FS.hpp>
#include <stc/Environment.hpp>
#include <stc/StringUtil.hpp>

extern "C" {

int upmfilesystem_exists(lua_State* state) {
    if (lua_gettop(state) < 1) {
        return luaL_error(state, "Need an argument");
    }

    lua_pushboolean(state,
        fs::exists(fs::path(luaL_checklstring(state, 1, nullptr)))
    );

    return 1;
}

int upmfilesystem_sharedLibInstalled(lua_State* state) {
    if (lua_gettop(state) < 1) {
        return luaL_error(state, "Need an argument");
    }
    upm::filesystem::logger->info("Looking for required shared libraries...");

    std::string cache = stc::syscommand("ldconfig -p");
    std::string libs = "";
    for (int i = 1; i <= lua_gettop(state); ++i) {
        std::string lib = luaL_checklstring(state, i, nullptr);
        if (cache.find(lib) == std::string::npos) {
            upm::filesystem::logger->info("{}: missing", lib);
            if (libs.size() != 0) {
                libs += ", ";
            }
            libs += lib;
        } else {
            upm::filesystem::logger->info("{}: OK", lib);
        }

    }
    if (libs != "") {
        std::string err = "Failed to find system libraries: " + libs; 
        return luaL_error(state, err.c_str());
    }
    upm::filesystem::logger->info("All shared libraries OK.");

    return 0;
}


int luaopen_upmfilesystem(lua_State* state) {
    static const luaL_Reg functions[] = {
        {"exists", upmfilesystem_exists},
        {"sharedLibInstalled", upmfilesystem_sharedLibInstalled},
        {nullptr, nullptr}
    };

    luaL_newlib(state, functions);

    return 1;
}

}
