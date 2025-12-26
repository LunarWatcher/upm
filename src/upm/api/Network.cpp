#include "Network.hpp"
#include "lauxlib.h"
#include "lua.hpp"
#include "spdlog/spdlog.h"
#include <string>

#include <filesystem>
#include <cpr/cpr.h>

// Do I really need this twice?
extern "C" {

int upmnetwork_request(lua_State* state) {
    if (lua_gettop(state) < 1) {
        return luaL_error(state, "Need an argument");
    }

    size_t len;
    auto rawUrl = luaL_checklstring(state, 1, &len);
    if (rawUrl == nullptr) {
        return luaL_typeerror(state, 1, "got null string");
    }
    std::string url { rawUrl, len };
    spdlog::debug("Requesting {}", url);

    auto response = cpr::Get(
        cpr::Url{url}
    );

    spdlog::debug(response.status_code);

    lua_newtable(state);
    lua_pushstring(state, response.text.c_str());
    lua_setfield(state, -2, "text");

    lua_pushinteger(state, response.status_code);
    lua_setfield(state, -2, "status_code");

    lua_pushstring(state, response.error.message.c_str());
    lua_setfield(state, -2, "error_message");

    return 1;
}

int upmnetwork_download(lua_State* state) {
    if (lua_gettop(state) < 1) {
        return luaL_error(state, "Missing argument: url");
    }

    std::string url = luaL_checklstring(state, 1, nullptr);

    spdlog::info("Downloading {}...", url);
    

    auto name = url.substr(url.rfind('/') + 1);
    std::filesystem::path tmpDir = std::filesystem::temp_directory_path();
    std::filesystem::path p = tmpDir / ("upm/" + name);
    if (std::filesystem::exists(p)) {
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



int luaopen_upmnetwork(lua_State* state) {

    static const luaL_Reg functions[] = {
        {"request", upmnetwork_request},
        {"download", upmnetwork_download},
        {nullptr, nullptr}
    };

    luaL_newlib(state, functions);

    return 1;
}

}
