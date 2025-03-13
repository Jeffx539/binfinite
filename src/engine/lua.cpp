#include "../console.hpp"
#include "lua.hpp"
#include "../utils/hooks.hpp"
#include "../utils/memory.hpp"
#include <fstream>
#include <sstream>
#include <string>
namespace engine::shared::lua {
utils::hooks::detour shared_lua_get_global;
utils::hooks::detour shared_lua_pcall;
utils::hooks::detour shared_lua_loadbuffer;
utils::hooks::detour shared_lua_print;

int Hook_GetGlobal(uintptr_t state, char *string)
{
    LuaState = state;
    return shared_lua_get_global.invoke<int>(state, string);
}




void LuaOpen(const std::string &path)
{
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();

    console::log("Loading Lua File : %s", path.c_str());
    DoString(buffer.str().c_str());
}


uint64_t Hook_PCall(uintptr_t state, int function, uint32_t i, int u)
{
    return shared_lua_pcall.invoke<uint64_t>(state, function, i, u);
}



uint64_t Hook_LoadBuffer(uintptr_t state,
  const struct CompilerSettings *settings,
  const char *buffer,
  __int64 length,
  const char *unknown1)
{
    return shared_lua_loadbuffer.invoke<uint64_t>(state, settings, buffer, length, unknown1);
}


uint64_t DoString(const char *string)
{
    size_t stringLength = strlen(string);
    

    if (shared_lua_loadbuffer.invoke<uint64_t>(LuaState,
          (const struct CompilerSettings *)(*((uintptr_t *)LuaState + 2) + 1368),
          string,
          stringLength,
          string)
        != 0) {
        return 1;
    }

    uint64_t call_result = shared_lua_pcall.invoke<uint64_t>(LuaState, 0, -1, 0);

    if (call_result != 0) { return 1; }
    return call_result;
}





void InstallHooks()
{
    shared_lua_get_global.create(GAME_PTR(0x0a75ed0), & Hook_GetGlobal);
    shared_lua_pcall.create(GAME_PTR(0x0699e24), &Hook_PCall);
   
}
}// namespace engine::shared::lua
