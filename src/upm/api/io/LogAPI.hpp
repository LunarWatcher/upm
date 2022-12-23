#pragma once

#include <lua.hpp>

#define UPMLOG_INFO 1
#define UPMLOG_DEBUG 2
#define UPMLOG_ERROR 3
#define UPMLOG_WARN 4

#define UPMLOG_LEVEL(name, levelNum) inline int upmlog_##name(lua_State* state) { \
        lua_pushinteger(state, levelNum); \
        lua_insert(state, 1); \
        return upmlog_log(state); \
    }

extern "C" {

extern int upmlog_log(lua_State* state);

UPMLOG_LEVEL(info, UPMLOG_INFO);
UPMLOG_LEVEL(warn, UPMLOG_WARN);
UPMLOG_LEVEL(debug, UPMLOG_DEBUG);
UPMLOG_LEVEL(error, UPMLOG_ERROR);

extern int luaopen_upmlog(lua_State* state);

}
