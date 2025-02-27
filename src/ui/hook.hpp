#pragma once
#include <MinHook.h>
#include <Windows.h>
#include <cstdio>
#include <mutex>
#include <thread>


namespace ui::hook {

	void Init();
	void Free();


}