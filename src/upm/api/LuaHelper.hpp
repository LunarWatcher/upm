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

    void init();
    void runFile(const std::string& fn);

    // Why the fuck do I need this??
    void registerLibrary(const char* libname, lua_CFunction func);
};

}
