#include "Filesystem.hpp"

#include "lauxlib.h"
#include "upm/Context.hpp"

#include "lua.h"
#include "upm/api/util/ArgHelper.hpp"
#include "upm/util/syscompat/Nproc.hpp"

#include <cstdlib>
#include <filesystem>
#include <stc/Environment.hpp>
#include <stc/StringUtil.hpp>

extern "C" {

int upmfilesystem_exists(lua_State* state) {
    if (lua_gettop(state) < 1) {
        return luaL_error(state, "Need an argument");
    }


    auto type = luaL_optinteger(state, 2, upm::filesystem::TYPE_ANY);

    auto path = std::filesystem::path(luaL_checklstring(state, 1, nullptr));
    switch (type) {
    case upm::filesystem::TYPE_ANY:
        lua_pushboolean(state,
            static_cast<int>(std::filesystem::exists(path)));
        break;
    case upm::filesystem::TYPE_DIRECTORY:
        lua_pushboolean(state,
            static_cast<int>(std::filesystem::is_directory(path)));
        break;
    case upm::filesystem::TYPE_FILE:
        // Not sure if is_regular_file is good enough. Using exists && !is_directory _might_ be better, but
        // I'm not sure
        lua_pushboolean(state,
            static_cast<int>(std::filesystem::is_regular_file(path)));
        break;
    default:
        return luaL_error(state, "You supplied an invalid value for the type. Please use fs.TYPE_ANY, fs.TYPE_DIRECTORY, or fs.TYPE_FILE");
    }
    lua_pushboolean(state,
        static_cast<int>(std::filesystem::exists(std::filesystem::path(luaL_checklstring(state, 1, nullptr))))
    );

    return 1;
}

int upmfilesystem_pwd(lua_State* L) {
    lua_pushstring(L, std::filesystem::current_path().string().c_str());;

    return 1;
}

int upmfilesystem_sharedLibInstalled(lua_State* state) {
    if (lua_gettop(state) < 2) {
        return luaL_error(state, "Need two arguments");
    }

    // Should preserve the cache.
    static std::string cache = stc::syscommand("ldconfig -p");

    std::string libs;
    bool errorIfMissing = lua_toboolean(state, 1) != 0;
    for (int i = 2; i <= lua_gettop(state); ++i) {
        std::string lib = luaL_checklstring(state, i, nullptr);
        if (cache.find(lib) == std::string::npos) {
            upm::filesystem::logger->info("{}: missing", lib);
            if (!libs.empty()) {
                libs += ", ";
            }
            libs += lib;
        } else {
            upm::filesystem::logger->info("{}: OK", lib);
        }

    }

    if (!libs.empty()) {
        upm::filesystem::logger->error("Failed to find system libraries: {}", libs); 
        if (errorIfMissing) {
            return luaL_error(state, "libraries missing");
        }
    } else {
        upm::filesystem::logger->debug("All shared libraries OK.");
    }
    lua_pushboolean(state, static_cast<int>(libs.empty()));


    return 1;
}

int upmfilesystem_installCopy(lua_State* state) {
    std::filesystem::path source = luaL_checkstring(state, 1);
    std::filesystem::path dest = upm::Context::inst->getPrefix();
    if (!std::filesystem::is_directory(dest)) { std::filesystem::create_directories(dest);
}

    // At least recursive copying is easy now
    std::filesystem::copy(source, dest, std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::copy_symlinks);
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

    if (result != 0) {
        return luaL_error(state, "Configure failed.");
}
    return 0;
}

int upmfilesystem_cmake(lua_State *state) {
    std::string path = luaL_checkstring(state, 1);

    upm::Context& ctx = *upm::Context::inst;

    // TODO: separate out to a utility function
    // (and maybe do some fancy template trickery?)
    std::map<std::string, std::optional<upm::ArgHelper::LuaField>> args;
    if (lua_gettop(state) >= 2) {
        luaL_checktype(state, 2, LUA_TTABLE);
        args = upm::ArgHelper::parseTable(state, 2, {
            {"prefixCommand", typeid(std::string)},
            {"buildType", typeid(std::string)},
            {"cd", typeid(bool)},
        });
    } 
    //spdlog::debug("cd = {}", std::get<bool>(args["cd"].value_or(true)));


    auto command = fmt::format(
        "{}cmake .. -DCMAKE_BUILD_TYPE={} -D{}={}",
        std::get<bool>(args["cd"].value_or(true)) ? fmt::format("cd {} && mkdir -p build && cd build && ", path) : "",
        std::get<std::string>(args["buildType"].value_or("RELEASE")),
        std::get<std::string>(args["prefixCommand"].value_or("CMAKE_INSTALL_PREFIX")),
        ctx.getPrefix()
    );

    int result = WEXITSTATUS(std::system(command.c_str()));
    if (result != 0) {
        return luaL_error(state, "CMake failed.");
    }

    // TODO: -j $(nproc)
    result = WEXITSTATUS(std::system(
            (
                (std::get<bool>(args["cd"].value_or(true)) ? fmt::format("cd {} && cd build && ", path) : "")
                + "cmake --build . --target install"
    ).c_str()));
    if (result != 0) {
        return luaL_error(state, "Install failed.");
    }

    return 0;
}

int upmfilesystem_make(lua_State* state) {
    if (lua_gettop(state) < 2) {
        return luaL_error(state, "Expected at least two arguments: <directory, arguments[, make command or nil for default, [max jobs, -1 for uncapped]>");
    }

    std::string sourceDir = lua_tostring(state, 1);
    std::string arguments = lua_tostring(state, 2);

    std::string make = lua_gettop(state) >= 3 && (lua_isstring(state, 3) != 0) ? lua_tostring(state, 3) : "make";
    long long maxJobs = lua_gettop(state) >= 4 && (lua_isinteger(state, 4) != 0) ? lua_tointeger(state, 4) : -1;

    long long hardwareThreads = upm::util::getHardwareConcurrency();
    // This calculates the number of threads to use, either capped by maxJobs (which, realistically, is either 1 or undefined),
    // or using the number of system threads
    //
    // This does force the number under hardwareThreads (min 1), but that's fine. There are apparently some advantages to running with more
    // threads than the hardware supports because of IO limits (though that advice was old, so fuck knows how NVMe affects that),
    // but this is an edge-case I don't feel like supporting.
    long long jobs = maxJobs > 0 ? std::min(maxJobs, hardwareThreads) : hardwareThreads;
    spdlog::debug("Running {} with {} jobs (detected hardware threads: {})", make, jobs, hardwareThreads);

    make += " -j " + std::to_string(jobs);

    // Looking back at this, this definitely opens for some vulnerabilities. Not sure how to best mitigate that though.
    // Also fairly low-risk, given that the lua scripts already execute in a non-sandboxed environment, so the scripts
    // have already been privilege escalated by the time this kicks in.
    // /shrug though
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
int upmfilesystem_makeInstallOnly(lua_State *state) {
    std::string sourceDir = lua_tostring(state, 1);

    std::string make = lua_gettop(state) >= 3 && (lua_isstring(state, 3) != 0) ? lua_tostring(state, 3) : "make";
    long long maxJobs = lua_gettop(state) >= 4 && (lua_isinteger(state, 4) != 0) ? lua_tointeger(state, 4) : -1;

    // TODO: get the system cap for threads
    long long hardwareThreads = upm::util::getHardwareConcurrency();
    // This calculates the number of threads to use, either capped by maxJobs (which, realistically, is either 1 or undefined)
    long long jobs = maxJobs > 0 ? std::min(maxJobs, hardwareThreads) : hardwareThreads;
    spdlog::debug("Running {} with {} jobs", make, jobs);

    make += " -j " + std::to_string(jobs);


    auto status = WEXITSTATUS(std::system(("cd " + sourceDir + " && " + make + " install ").c_str()));
    if (status != 0) {
        std::cout << "Status = " << status << std::endl;
        return luaL_error(state, "Failed to install");
    }
    return 0;
}

// TODO: Replace with libarchive
int upmfilesystem_untar(lua_State* state) {
    std::filesystem::path source = luaL_checkstring(state, 1);
    if (auto pos = source.string().find("/tmp/"); pos != 0) {
        throw std::runtime_error("Invalid path; must be a path to /tmp/");
    }
    long long stripComponents = luaL_optinteger(state, 2, -1);
    
    auto dest = source;
    dest.replace_extension().replace_extension();
    if (auto pos = dest.string().find("/tmp/"); pos != 0) {
        throw std::runtime_error("Fatal: resolved destination path is not a /tmp/ path");
    }
    std::string tarArg;
    if (stripComponents > 0) {
        tarArg += "--strip-components " + std::to_string(stripComponents);
    }

    tarArg += " -xf " + source.string();
    tarArg += " -C " + dest.string();
    upm::filesystem::logger->info("Unpacking {} to {}...", source.string(), dest.string());

    std::filesystem::create_directories(dest);
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
        {"cmake", upmfilesystem_cmake},
        {"makeInstallOnly", upmfilesystem_makeInstallOnly},
        {"untar", upmfilesystem_untar},
        {"installCopy", upmfilesystem_installCopy},
        {"pwd", upmfilesystem_pwd},
        {nullptr, nullptr}
    };

    luaL_newlib(state, functions);

    lua_pushnumber(state, upm::filesystem::TYPE_ANY);
    lua_setfield(state, -2, "TYPE_ANY");

    lua_pushnumber(state, upm::filesystem::TYPE_DIRECTORY);
    lua_setfield(state, -2, "TYPE_DIRECTORY");

    lua_pushnumber(state, upm::filesystem::TYPE_ANY);
    lua_setfield(state, -2, "TYPE_FILE");

    return 1;
}

}
