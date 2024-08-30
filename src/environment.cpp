
#include "environment.hpp"
#include "utils/memory.hpp"
#include "console.hpp";


namespace environment {

	static void *offs_getopmode = nullptr;

uint64_t FindEnv()
{
    if (offs_getopmode == nullptr) {
        // 0x0631468: 259371.24.08.08.2101
        offs_getopmode =
          static_cast<uint8_t *>(utils::memory::search(
            "", "\x8B\x05\xCC\xCC\xCC\xCC\xC3\x8A\x05\xCC\xCC\xCC\xCC\xF6\xD8\xC6\x05\xCC\xCC\x00\xCC\xCC", 0xCC))
          - 0x9;
    }

    auto func = (uint64_t(__stdcall *)())(offs_getopmode);


    return func();
}


}
