#pragma once

#include <lua.hpp>

#define UPMLOG_INFO 1
#define UPMLOG_DEBUG 2
#define UPMLOG_ERROR 3
#define UPMLOG_WARN 4

extern "C" {

extern int upmlog_log(lua_State* state);

extern int luaopen_upmlog(lua_State* state);

}
