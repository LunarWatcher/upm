#include "Network.hpp"
#include "lauxlib.h"
#include "lua.h"
#include "lua.hpp"

#include <iostream>
#include <stc/FS.hpp>
#include <cpr/cpr.h>

// Do I really need this twice?
extern "C" {

int upmnetwork_request(lua_State* state) {
    if (lua_gettop(state) < 1) {
        return luaL_error(state, "Need an argument");
    }

    std::string url = luaL_checklstring(state, 1, nullptr);
    upm::network::logger->debug("Requesting {}", url);

    auto response = cpr::Get(cpr::Url(url));

    lua_newtable(state);
    lua_pushstring(state, response.text.c_str());
    lua_setfield(state, -2, "text");

    lua_pushinteger(state, response.status_code);
    lua_setfield(state, -2, "status_code");

    return 1;
}

int upmnetwork_download(lua_State* state) {
    if (lua_gettop(state) < 1) {
        return luaL_error(state, "Missing argument: url");
    }

    std::string url = luaL_checklstring(state, 1, nullptr);

    upm::network::logger->info("Downloading {}...", url);
    

    auto name = url.substr(url.rfind('/') + 1);
    fs::path tmpDir = fs::temp_directory_path();
    fs::path p = tmpDir / ("upm/" + name);
    if (fs::exists(p)) {
        spdlog::info("Cache entry found at {}.", p.string());
        lua_pushinteger(state, 200);
        lua_pushstring(state, p.string().c_str());
        return 2;
    }

    std::ofstream of(
        p.string()
    );

    auto r = cpr::Download(
        of,
        cpr::Url{url}
    );
    spdlog::debug("Received {}Mb", ((double) r.downloaded_bytes) / (1024.0 * 1024.0));
    lua_pushinteger(state, r.status_code);
    if (r.status_code == 0) {
        return luaL_error(state, "Network error. Make sure you're still connected to the interweebs.");
    } else if (r.status_code != 200) {
        spdlog::error("Download failed with status code {}", r.status_code);
        lua_pushnil(state);
    } else {
        lua_pushstring(state, p.string().c_str());
    }

    return 2;
}

int upmnetwork_gitClone(lua_State* state) {
    if (lua_gettop(state) < 2) {
        return luaL_error(state, "Expected 2 arguments");
    }

    std::string repo = luaL_checklstring(state, 1, nullptr);
    std::string dest = luaL_checklstring(state, 2, nullptr);
    bool clean = lua_gettop(state) >= 3 ? lua_toboolean(state, 3) : false;

    // TODO: we'll want to change the directories to differentiate between root and users. Maybe
    // append getuid or whatever to the path for good measure? Want to avoid permissions as much
    // as possible.
    if (fs::exists("/tmp/upm/" + dest)) {
        if (clean) {
            upm::network::logger->info("Already cloned. Reset policy forces cache deletion...");
            if (!fs::remove_all("/tmp/upm/" + dest)) {
                return luaL_error(state, ("Failed to delete /tmp/upm/" + dest).c_str());
            }
        } else {
            upm::network::logger->info("Cached clone found; reset policy doesn't require re-cloning");
            lua_pushboolean(state, false);
            // There has to be a better way to push this rather than doing it in two separate places.
            // Probably organized my code awfully, lmao
            // For good measure;
            // TODO: also has to be tweaked when adding the UID
            lua_pushstring(state, ("/tmp/upm/" + dest).c_str());
            return 2;
        }
    }

    int status = std::system(("git -C /tmp/upm clone " + repo + " " + dest).c_str());
    if (status != 0) {
        upm::network::logger->info("Failed to run `git clone {}`", repo);
        return luaL_error(state, "Clone failed.");
    }

    upm::network::logger->info("Successfully cloned {}", repo);
    lua_pushboolean(state, true);
    // TODO: this line also needs to be tweaked when adding the UID
    lua_pushstring(state, ("/tmp/upm/" + dest).c_str());
    return 2;
}

int luaopen_upmnetwork(lua_State* state) {

    static const luaL_Reg functions[] = {
        {"request", upmnetwork_request},
        {"gitClone", upmnetwork_gitClone},
        {"download", upmnetwork_download},
        {nullptr, nullptr}
    };

    luaL_newlib(state, functions);

    return 1;
}

}
