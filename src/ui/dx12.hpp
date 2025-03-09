#pragma once

#include <MinHook.h>
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_4.h>


#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")




namespace DX12 {

inline int wwidth = 1200.f;
inline int wheight = 600.f;


	
void Hook(HWND hwnd);
void Unhook();
}// namespace DX12
