#pragma once

#include <lua.hpp>
extern "C" {
#include "lualib.h"
}

#include "Network.hpp"

namespace upm {

class LuaHelper {
private:
    lua_State* state;
public:
    LuaHelper();
    ~LuaHelper();


    void registerLibrary(const char* libname, lua_CFunction func);
};

}
