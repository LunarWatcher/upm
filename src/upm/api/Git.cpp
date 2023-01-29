#include "Git.hpp"

#include <stc/FS.hpp>
#include <stc/Environment.hpp>
#include <spdlog/spdlog.h>

extern "C" {

int git_clone(lua_State* state) {
    if (lua_gettop(state) < 2) {
        return luaL_error(state, "Expected 2 arguments");
    }

    std::string repo = luaL_checklstring(state, 1, nullptr);
    std::string dest = luaL_checklstring(state, 2, nullptr);
    bool clean = lua_gettop(state) >= 3 ? lua_toboolean(state, 3) : false;
    bool absPath = lua_gettop(state) >= 4 ? lua_toboolean(state, 4) : false;
    fs::path p = absPath ? dest : ("/tmp/upm/" + dest);
    // TODO: we'll want to change the directories to differentiate between root and users. Maybe
    // append getuid or whatever to the path for good measure? Want to avoid permissions as much
    // as possible.
    if (fs::exists(p)) {
        if (clean) {
            spdlog::info("Already cloned. Reset policy forces cache deletion...");
            if (!fs::remove_all(p)) {
                return luaL_error(state, ("Failed to delete " + p.string()).c_str());
            }
        } else {
            spdlog::info("Cached clone found; reset policy doesn't require re-cloning");
            lua_pushboolean(state, false);
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

    spdlog::info("Successfully cloned {}", repo);
    lua_pushboolean(state, true);
    // TODO: this line also needs to be tweaked when adding the UID
    lua_pushstring(state, p.string().c_str());
    return 2;
}

int git_checkout(lua_State* state) {
    std::string repoLoc = luaL_checkstring(state, 1);
    std::string checkoutObj = luaL_checkstring(state, 2);
    bool masterMainFutureProofing = true;
    if (lua_gettop(state) >= 3) {
        masterMainFutureProofing = lua_toboolean(state, 3);
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
    std::string repo = luaL_checkstring(state, 1);

    int res = std::system(("cd " + repo + " && git pull $(git remote) $(git rev-parse --abbref-rev HEAD)").c_str());
    if (res != 0) {
        return luaL_error(state, "Failed to pull");
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
