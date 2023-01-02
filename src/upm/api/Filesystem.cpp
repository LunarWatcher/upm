#include "Filesystem.hpp"

#include "lauxlib.h"
#include "upm/Context.hpp"

#include "lua.h"
#include "upm/api/Constants.hpp"

#include <cstdlib>
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
    if (lua_gettop(state) < 2) {
        return luaL_error(state, "Need two arguments");
    }

    // Should preserve the cache.
    static std::string cache = stc::syscommand("ldconfig -p");

    std::string libs = "";
    bool errorIfMissing = lua_toboolean(state, 1);
    for (int i = 2; i <= lua_gettop(state); ++i) {
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
        upm::filesystem::logger->error("Failed to find system libraries: {}", libs); 
        if (errorIfMissing) {
            return luaL_error(state, "libraries missing");
        }
    } else {
        upm::filesystem::logger->debug("All shared libraries OK.");
    }
    lua_pushboolean(state, libs == "");


    return 1;
}

int upmfilesystem_installCopy(lua_State* state) {
    fs::path source = luaL_checkstring(state, 1);
    fs::path dest = upm::Context::inst->getPrefix();
    if (!fs::is_directory(dest)) fs::create_directories(dest);

    // At least recursive copying is easy now
    fs::copy(source, dest, fs::copy_options::recursive | fs::copy_options::overwrite_existing);
    return 0;
}

int upmfilesystem_configure(lua_State* state) {

    if (lua_gettop(state) < 2) {
        return luaL_error(state, "Expected at least two arguments: <source directory, configure arguments[, configure command>");
    }

    std::string relativeSourceDir = lua_tostring(state, 1);
    std::string arguments = lua_tostring(state, 2);

    std::string configure = lua_gettop(state) >= 3 ? lua_tostring(state, 3) : "./configure";

    upm::Context& ctx = *upm::Context::inst;

    int result = WEXITSTATUS(std::system(("cd " + relativeSourceDir
                              + " && " + configure + " " + arguments
                              + " --prefix=" + ctx.getPrefix()).c_str()));

    if (result != 0)
        return luaL_error(state, "Configure failed.");
    return 0;
}

int upmfilesystem_make(lua_State* state) {
    if (lua_gettop(state) < 2) {
        return luaL_error(state, "Expected at least two arguments: <directory, arguments[, make command]>");
    }

    std::string sourceDir = lua_tostring(state, 1);
    std::string arguments = lua_tostring(state, 2);

    // TODO: make not only adjustable, but automatically set to the core count if not overridden
    // -k seems to be necessary to silence "nothing to be done for ...".
    // Not sure why that's an error to begin with? TODO: Fix
    std::string make = lua_gettop(state) >= 3 ? lua_tostring(state, 3) : "make -j 8";

    int status = WEXITSTATUS(std::system(("cd " + sourceDir + " && " + make + " " + arguments).c_str()));
    if (status != 0) {
        std::cout << "Status = " << status << std::endl;
        return luaL_error(state, "Failed to make");
    }
    status = WEXITSTATUS(std::system(("cd " + sourceDir + " && " + make + " install").c_str()));
    if (status != 0) {
        std::cout << "Status = " << status << std::endl;
        return luaL_error(state, "Failed to install");
    }
    return 0;
}

int upmfilesystem_untar(lua_State* state) {
    fs::path source = luaL_checkstring(state, 1);
    if (auto pos = source.string().find("/tmp/"); pos == std::string::npos || pos != 0) {
        throw std::runtime_error("Invalid path; must be a path to /tmp/");
    }
    int stripComponents = luaL_optinteger(state, 2, -1);
    
    auto dest = source;
    dest.replace_extension().replace_extension();
    if (auto pos = dest.string().find("/tmp/"); pos == std::string::npos || pos != 0) {
        throw std::runtime_error("Fatal: resolved destination path is not a /tmp/ path");
    }
    std::string tarArg = "";
    if (stripComponents > 0) {
        tarArg += "--strip-components " + std::to_string(stripComponents);
    }

    tarArg += " -xf " + source.string();
    tarArg += " -C " + dest.string();
    upm::filesystem::logger->info("Unpacking {} to {}...", source.string(), dest.string());

    fs::create_directories(dest);
    auto res = std::system(("tar " + tarArg).c_str());
    if (res != 0) {
        return luaL_error(state, "Failed to untar");
    }

    lua_pushstring(state, dest.string().c_str());
    return 1;
}

int luaopen_upmfilesystem(lua_State* state) {
    static const luaL_Reg functions[] = {
        {"exists", upmfilesystem_exists},
        {"sharedLibInstalled", upmfilesystem_sharedLibInstalled},
        {"configure", upmfilesystem_configure},
        {"make", upmfilesystem_make},
        {"untar", upmfilesystem_untar},
        {"installCopy", upmfilesystem_installCopy},
        {nullptr, nullptr}
    };

    luaL_newlib(state, functions);

    return 1;
}

}
