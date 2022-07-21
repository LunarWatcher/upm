#include "Exec.hpp"

#include "stc/"

extern "C" {

int upmexec_exec(lua_State* state) {
    if (lua_gettop(state) < 1) {
        return luaL_error(state, "Need an argument");
    }

    std::string command = luaL_checklstring(state, 1, nullptr);

    int f = std::system(command);
    int ok = 0;
    if (lua_gettop(state) >= 2) {
        ok = luaL_checklint(state, 2, nullptr);
    }
    if (f != ok) {
        return luaL_error(state, "Failed to run \"" + command + "\".");
    }

    return 0;
    
}

}
