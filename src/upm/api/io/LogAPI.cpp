#include "LogAPI.hpp"

#include <sstream>
#include <string>

#include "spdlog/spdlog.h"

extern "C" {

int upmlog_log(lua_State* state) {
    auto argc = lua_gettop(state);
    if (argc == 0) {
        return luaL_error(state, "Need a message to log.");
    }
    auto level = UPMLOG_INFO;
    std::stringstream ss;
    int cont = 2;
    if (lua_isinteger(state, 1)) {
        // Level, message, ...
        level = lua_tointeger(state, 1);
        if (argc < 2) {
            return luaL_error(state, "Need a message");
        }
        ss << luaL_checkstring(state, 2);
        ++cont;
    } else {
        // message, ...
        ss << luaL_checkstring(state, 1);
    }

    for (int i = cont; i <= argc; ++i) {
        switch (lua_type(state, i)) {
        case LUA_TNUMBER:
            ss << " " << lua_tonumber(state, i);
            break;
        case LUA_TNIL:
            ss << " nil";
            break;
        case LUA_TBOOLEAN:
            ss << " " << (lua_toboolean(state, i) ? "true" : "false"); 
            break;
        case LUA_TSTRING:
            ss << " " << lua_tostring(state, i);
            break;
        default:
            ss << " " << luaL_typename(state, i) << "[" << lua_topointer(state, i) << "]";
            break;
        }
    }

    if (level < UPMLOG_INFO || level > UPMLOG_WARN) {
        return luaL_error(state, "Invalid log level; must be INFO (1), DEBUG (2), ERROR(3), or WARN (4). "
                          "Using the log constants instead of the integer representation is highly recommended");
    }

    auto fmt = ss.str();
    switch (level) {
    case UPMLOG_INFO:
        spdlog::info(fmt); 
        break;
    case UPMLOG_DEBUG:
        spdlog::debug(fmt);
        break;
    case UPMLOG_WARN:
        spdlog::warn(fmt);
        break;
    case UPMLOG_ERROR:
        spdlog::error(fmt);
        break;
    }

    return 0;
}

int luaopen_upmlog(lua_State* state) {
    static const luaL_Reg functions[] = {
        {"log", upmlog_log},
        {"info", upmlog_info},
        {"warn", upmlog_warn},
        {"debug", upmlog_debug},
        {"error", upmlog_error},
        {nullptr, nullptr}
    };

    luaL_newlib(state, functions);
    lua_pushinteger(state, UPMLOG_INFO);
    lua_setfield(state, -2, "INFO");

    lua_pushinteger(state, UPMLOG_DEBUG);
    lua_setfield(state, -2, "DEBUG");

    lua_pushinteger(state, UPMLOG_ERROR);
    lua_setfield(state, -2, "WARN");

    lua_pushinteger(state, UPMLOG_WARN);
    lua_setfield(state, -2, "ERROR");

    return 1;
}

}
