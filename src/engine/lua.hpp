// https://raw.githubusercontent.com/Surasia/InfiniteExt/master/InfExt/src/HavokScript/HavokScript.hpp

#pragma once
#include <cstdint>

namespace engine::shared::lua {

void InstallHooks();
uint64_t DoString(const char *string);
static std::uintptr_t LuaState = 0;


struct CompilerSettings
{
    uint64_t emitStructCode;// hksBool
    const char **stripNames;
    uint32_t emitMemoCode;// hksBool
    uint32_t isMemoTestingMode;// hksBool
    uint32_t bytecodeSharingFormat;// BytecodeSharingFormat (0 off, 1 on, 2 secure)
    uint32_t enableIntLiterals;// IntLiteralsFormat (NONE, LUD, 32bit, UI64, 64bit, all)
    int unknown1;
    void *unknown2;
    int (*debugMap)(const char *filename, int lua_line);
};


};// namespace engine::shared::lua

