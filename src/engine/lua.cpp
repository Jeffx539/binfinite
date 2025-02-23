#include "lua.hpp"
#include "../console.hpp"
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
utils::hooks::detour shared_lua_register_func;

int Hook_GetGlobal(uintptr_t state, char *string)
{
    LuaState = state;
    return shared_lua_get_global.invoke<int>(state, string);
}


int Hook_Print(uintptr_t state)
{
    uint8_t *mod = reinterpret_cast<uint8_t *>(utils::memory::GetModuleInfo("").lpBaseOfDll);
    //auto tostring = (char *(__stdcall *)(uintptr_t, int))(mod + 0x06fe61c);
    //console::log("LPrint: %s", tostring(state, -1));
    console::log("hook print");
    return 0;
}


void LuaOpen(const std::string path)
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



uint64_t Hook_RegisterLuaFunction(void *fnc_ptr, int32_t _mb_flag, void *fnc_name)
{
    
    // first elem of fnc_name is string, it's some sort of struct
    //console::log("Engine Registering LUA Function -> %s at %p", fnc_name, fnc_ptr);

         //__debugbreak();
    // no idea what the flags mean
    if (std::string(reinterpret_cast<char *>(fnc_name)) == "print" && _mb_flag == 0xffffd8ee) {
        console::log("Engine Registering LUA Function -> %s at %p", fnc_name, fnc_ptr);

        //__debugbreak();
    }


    return shared_lua_register_func.invoke<uint64_t>(fnc_ptr, _mb_flag, fnc_name);



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
    uint8_t *mod = reinterpret_cast<uint8_t *>(utils::memory::GetModuleInfo("").lpBaseOfDll);
    

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



void Init_Print_Hook()
{

 // we're going to use a stubbed location for our lua print
    // @ 0x0d1783b -> { 0x48, 0x8D, 0x05, 0x1F, 0x10, 0x8D, 0xFF }
    // lea  rax,[rip+0xffffffffff8d101f]

    std::uint8_t *module_base = reinterpret_cast<std::uint8_t *>(utils::memory::GetModuleInfo("").lpBaseOfDll);
    auto a = reinterpret_cast<uint8_t **>(module_base + 0x0d1783b);
    DWORD old_protect{};
    VirtualProtect(a, sizeof(a), PAGE_EXECUTE_READWRITE, &old_protect);
    uint8_t writeme[] = { 0x48, 0x8D, 0x05, 0x1F, 0x10, 0x8D, 0xFF };
    std::memcpy(a, &writeme, 7);
    VirtualProtect(a, sizeof(a), old_protect, &old_protect);

    auto jmp = utils::hooks::assemble([](utils::hooks::assembler &a) {
        a.mov(rax, &Hook_Print);
        a.jmp(rax);
    });
     a = reinterpret_cast<uint8_t **>(module_base + 0x05e8861);
     VirtualProtect(a, 12, PAGE_EXECUTE_READWRITE, &old_protect);
     std::memcpy(a,jmp , 12);
     VirtualProtect(a, 12, old_protect, &old_protect);


    console::log("%p", &Hook_Print);



    // utils::hooks::write<void *>(a, &Hook_Print);

    //*a = reinterpret_cast<void *>(LuaPrint);

    // console::log("%p", a);
}


void InstallHooks()
{
    std::uint8_t *module_base = reinterpret_cast<std::uint8_t *>(utils::memory::GetModuleInfo("").lpBaseOfDll);
    shared_lua_get_global.create(module_base + 0x0a70020, &Hook_GetGlobal);
    shared_lua_pcall.create(module_base + 0x0694634, &Hook_PCall);
    shared_lua_loadbuffer.create(module_base + 0x09f5400, &Hook_LoadBuffer);
    shared_lua_register_func.create(module_base + 0x069210c, &Hook_RegisterLuaFunction);


    console::log("Hook Print : %p", &Hook_Print);
    //Init_Print_Hook();

   
}
}// namespace engine::shared::lua
