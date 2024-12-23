#include "Git.hpp"
#include "upm/util/String.hpp"

#include <stc/Environment.hpp>
#include <spdlog/spdlog.h>

extern "C" {

int git_clone(lua_State* state) {
    if (lua_gettop(state) < 2) {
        return luaL_error(state, "Expected 2 arguments");
    }

    std::string repo = upm::String::escapeShellArg(luaL_checklstring(state, 1, nullptr));
    std::string dest = luaL_checklstring(state, 2, nullptr);
    bool clean = (lua_gettop(state) >= 3 ? lua_toboolean(state, 3) : 0) != 0;
    bool absPath = (lua_gettop(state) >= 4 ? lua_toboolean(state, 4) : 0) != 0;
    std::filesystem::path p = absPath ? dest : ("/tmp/upm/" + dest);

    spdlog::debug("Dest. path: {}", p.string());

    if (std::filesystem::exists(p)) {
        if (clean) {
            spdlog::info("Already cloned. Reset policy forces cache deletion...");
            if (std::filesystem::remove_all(p) == 0u) {
                return luaL_error(state, ("Failed to delete " + p.string()).c_str());
            }
        } else {
            spdlog::info("Cached clone found; reset policy doesn't require re-cloning. Running git fetch for good measure");

            int _ = std::system(("cd " + p.string() + " && git fetch").c_str());
            lua_pushboolean(state, 0);
            // There has to be a better way to push this rather than doing it in two separate places.
            // Probably organized my code awfully, lmao
            // For good measure;
            // TODO: also has to be tweaked when adding the UID
            lua_pushstring(state, p.string().c_str());
            return 2;
        }
    }

    int status = std::system(("git clone " + repo + " " + p.string()).c_str());
    if (status != 0) {
        spdlog::info("Failed to run `git clone {}`", repo);
        return luaL_error(state, "Clone failed.");
    }

    spdlog::info("Successfully cloned {} to {}", repo, p.string());
    lua_pushboolean(state, 1);
    // TODO: this line also needs to be tweaked when adding the UID
    lua_pushstring(state, p.string().c_str());
    return 2;
}

int git_checkout(lua_State* state) {
    std::string repoLoc = upm::String::escapeShellArg(luaL_checkstring(state, 1));
    std::string checkoutObj = upm::String::escapeShellArg(luaL_checkstring(state, 2));
    bool masterMainFutureProofing = true;
    if (lua_gettop(state) >= 3) {
        masterMainFutureProofing = (lua_toboolean(state, 3) != 0);
    }

    std::string cd = "cd " + repoLoc + " && ";
    int status = std::system((cd +  "git checkout " + checkoutObj).c_str());
    if (status != 0) {
        if (checkoutObj == "master" && masterMainFutureProofing) {
            status = std::system((cd + "git checkout main").c_str());
        } else if (checkoutObj == "main" && masterMainFutureProofing) {
            status = std::system((cd + "git checkout master").c_str());
        } else {
            return luaL_error(state, "Failed to run checkout.");
        }
        if (status != 0) {
            return luaL_error(state, "Failed to run checkout for both main and master");
        }

    }
    return 0;
}

int git_pull(lua_State* state) {
    std::string repo = upm::String::escapeShellArg(luaL_checkstring(state, 1));

    int res = std::system(("cd " + repo + " && git pull $(git remote) $(git rev-parse --abbref-rev HEAD)").c_str());
    if (res != 0) {
        return luaL_error(state, "Failed to pull");
    }
    return 0;
}

int git_fetch(lua_State *state) {
    std::string repo = upm::String::escapeShellArg(luaL_checkstring(state, 1));

    int res = std::system(fmt::format("cd {} && git fetch", repo).c_str());
    if (res != 0) {
        return luaL_error(state, "Failed to fetch");
    }
    return 0;
}

int luaopen_git(lua_State* state) {
    static const luaL_Reg functions[] = {
        {"clone", git_clone},
        {"checkout", git_checkout},
        {"pull", git_pull},
        {nullptr, nullptr}
    };

    luaL_newlib(state, functions);
    return 1;
}

}
