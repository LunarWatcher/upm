#include "Activators.hpp"

#include "lauxlib.h"
#include "lua.h"
#include "upm/Context.hpp"
#include "upm/package/Activators.hpp"
#include <filesystem>

extern "C" {

int activators_universalUNIX(lua_State* state) {
    std::vector<std::string> safeDirNames;
    if (lua_gettop(state) >= 1) {
        auto tableIdx = lua_absindex(state, -1);
        luaL_checktype(state, tableIdx, LUA_TTABLE);

        lua_pushnil(state);

        while (lua_next(state, tableIdx) != 0) {
            std::string value = luaL_checkstring(state, -1);
safeDirNames.push_back(value);
            lua_pop(state, 1);
        }

    }
    if (!upm::Activators::recursiveUniversalUNIXLink(safeDirNames)) {
        return luaL_error(state, "Activation function errored.");
    }
    return 0;
}

int activators_activateSingle(lua_State *L) {
    upm::Context& ctx = *upm::Context::inst;

    std::string src = luaL_checkstring(L, 1);
    std::string dest = luaL_checkstring(L, 2);

    std::string qualifiedDest = dest[0] != '/' ? "/usr/local/" + dest : dest;


    if (!std::filesystem::exists(src)) {
        return luaL_error(L, (src + "doesn't exist").c_str());
    }
    spdlog::info("Linking {} -> {}", src, qualifiedDest);

    ctx.cfg.data["package"][ctx.package].push_back(
        {
            {"target", qualifiedDest}, 
            {"source", src}
        }
    );

    if (std::filesystem::exists(qualifiedDest)) {
        std::filesystem::remove_all(qualifiedDest);
    }

    std::filesystem::create_symlink(src, qualifiedDest);

    ctx.cfg.save();

    return 0;
}

int luaopen_activators(lua_State* state) {
    static const luaL_Reg functions[] = {
        {"universalUNIX", activators_universalUNIX},
        {"activateSingle", activators_activateSingle},
        {nullptr, nullptr}
    };

    luaL_newlib(state, functions);

    return 1;
}

}
