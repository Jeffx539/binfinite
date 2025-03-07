
#include "dx12.hpp"
#include "hook.hpp"
#include "menu.hpp"
#include "../console.hpp"

namespace ui::hook {


static HWND g_hWindow = NULL;
static std::mutex g_mReinitHooksGuard;


static BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam)
{
    const auto isMainWindow = [handle]() { return GetWindow(handle, GW_OWNER) == nullptr && IsWindowVisible(handle); };

    DWORD pID = 0;
    GetWindowThreadProcessId(handle, &pID);

    if (GetCurrentProcessId() != pID || !isMainWindow() || handle == GetConsoleWindow()) return TRUE;

    *reinterpret_cast<HWND *>(lParam) = handle;

    return FALSE;
}

HWND GetProcessWindow()
{
    HWND hwnd = nullptr;
    EnumWindows(EnumWindowsCallback, reinterpret_cast<LPARAM>(&hwnd));

    while (!hwnd) {
        EnumWindows(EnumWindowsCallback, reinterpret_cast<LPARAM>(&hwnd));
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    char name[128];
    GetWindowTextA(hwnd, name, RTL_NUMBER_OF(name));
    return hwnd;
}

static WNDPROC oWndProc;
static LRESULT WINAPI WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_KEYDOWN) {
        if (wParam == VK_OEM_3 || wParam == VK_INSERT) {
            ui::menu::draw = !ui::menu::draw;
            return 0;
        }
    }

    LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    if (ui::menu::draw) { ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam); }

    return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}


 void Init()
{
    g_hWindow = GetProcessWindow();
    DX12::Hook(g_hWindow);
    oWndProc =
      reinterpret_cast<WNDPROC>(SetWindowLongPtr(g_hWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProc)));

    ImGuiIO &io = ImGui::GetIO();
    //io.Fonts->AddFontDefault();
    auto font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 32.0f);





    io.Fonts->Build();

    //ImGui::PushFont(font);



}

void Free()
{
    if (oWndProc) { SetWindowLongPtr(g_hWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(oWndProc)); }


        // io.Fonts->AddFontDefault();
    // 
    //MH_DisableHook(MH_ALL_HOOKS);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    DX12::Unhook();
}


}

