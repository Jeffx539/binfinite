
#include "console.hpp"
#include <cstdarg>
#include <stdexcept>
#include <windows.h>


namespace console {

void init()
{

    AllocConsole();
    AttachConsole(GetCurrentProcessId());

    if (freopen_s(&dummy,"CONIN$", "r", stdin) != 0 || freopen_s(&dummy, "CONOUT$", "w", stdout) != 0) {
        throw std::runtime_error("Failed to allocate consoles");
    }

}


void shutdown()
{


    freopen_s(&dummy, "NUL", "w", stdout);
    freopen_s(&dummy, "NUL", "r", stdin);
    freopen_s(&dummy, "NUL", "r", stderr);

    FreeConsole();
}


}// namespace console