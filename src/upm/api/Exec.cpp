#include "Exec.hpp"

#include <string>
#include <cstdlib>

extern "C" {

int upmexec_exec(lua_State* state) {
    if (lua_gettop(state) < 1) {
        return luaL_error(state, "Need an argument");
    }

    std::string command = luaL_checklstring(state, 1, nullptr);

    upm::exec::logger->info("Running '{}'...", command);

    int f = std::system(command.c_str());
    int ok = 0;
    if (lua_gettop(state) >= 2) {
        ok = luaL_checkinteger(state, 2);
    }
    if (f != ok) {
        upm::exec::logger->error("Failed to run {}", command);
        // There has to be a better way to do this, but I don't remember. Basic C-like string operations are a bitch
        // and don't really have easy syntactic sugar.
        std::string err = "Failed to run \"" + command + "\"";

        return luaL_error(state, err.c_str());
    }


    return 0;
}

int luaopen_upmexec(lua_State* state) {
    static const luaL_Reg functions[] = {
        {"exec", upmexec_exec},
        {nullptr, nullptr}
    };
    luaL_newlib(state, functions);

    return 1;
}

}
