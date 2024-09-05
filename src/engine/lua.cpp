#include "lua.hpp"
#include "../console.hpp"
#include "../utils/hooks.hpp"
#include "../utils/memory.hpp"
namespace engine::shared::lua {
utils::hooks::detour shared_lua_get_global;
utils::hooks::detour shared_lua_pcall;
utils::hooks::detour shared_lua_loadbuffer;


int Hook_GetGlobal(uintptr_t state, char *string)
{
    LuaState = state;
    return shared_lua_get_global.invoke<int>(state, string);
}


 uint64_t Hook_PCall(uintptr_t state, int function, uint32_t i, int u)
{
     return shared_lua_pcall.invoke<uint64_t>(state, function,i,u);
 }

  uint64_t Hook_LoadBuffer(uintptr_t state,
   const struct CompilerSettings *settings,
   const char *buffer,
   __int64 length,
   const char *unknown1)
 {
     return shared_lua_loadbuffer.invoke<uint64_t>(state,settings, buffer, length, unknown1);
 }



  uint64_t DoString(const char *string)
 {
     size_t stringLength = strlen(string);
     uint8_t *mod = reinterpret_cast<uint8_t *>(utils::memory::GetModuleInfo("").lpBaseOfDll);

     //auto tostring = (char *(__stdcall *)(uintptr_t, int))(mod + 0x06fe61c);

     if (shared_lua_loadbuffer.invoke<uint64_t>(LuaState,
           (const struct CompilerSettings *)(*((uintptr_t *)LuaState + 2) + 1368),
           string,
           stringLength,
           string)
         != 0) {
         return 1;

     }

     uint64_t call_result = shared_lua_pcall.invoke<uint64_t>(LuaState, 0, -1, 0);

     //console::log("call res: %d", call_result);
     if (call_result != 0) { 



         return 1; }
     return call_result;
 }




void InstallHooks()
{
    std::uint8_t *module_base = reinterpret_cast<std::uint8_t *>(utils::memory::GetModuleInfo("").lpBaseOfDll);
    shared_lua_get_global.create(module_base + 0x80CB4C, &Hook_GetGlobal);
    shared_lua_pcall.create(module_base + 0x764304, &Hook_PCall);
    shared_lua_loadbuffer.create(module_base + 0xA484C0, &Hook_LoadBuffer);

}// namespace engine::shared::lua
