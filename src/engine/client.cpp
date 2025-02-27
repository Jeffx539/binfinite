

#include "../utils/hooks.hpp"
#include "../utils/memory.hpp"
#include "../console.hpp"
#include <WinSock2.h>

#include "client.hpp"
#include "server.hpp"
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")


namespace engine::client {



    bool IsServerSelectionOpen() {
        auto addrt = reinterpret_cast<uint8_t *>(utils::memory::GetModuleInfo("").lpBaseOfDll) + 0x4f31492;
        *reinterpret_cast<bool *>(addrt) = true;

        auto addr = reinterpret_cast<uint8_t *>(utils::memory::GetModuleInfo("").lpBaseOfDll) + 0x4D6F2A4;
        return *reinterpret_cast<bool *>(addr);


        // data_144f31492



    }


    char* GetRTTString() {
        auto addr = reinterpret_cast<uint8_t *>(utils::memory::GetModuleInfo("").lpBaseOfDll) + 0x4d3d8f0;

       return reinterpret_cast<char *>(addr);

    }


    void SetFrameRate(float frame_rate) {
        auto rate = reinterpret_cast<uint8_t*>(utils::memory::GetModuleInfo("").lpBaseOfDll) + 0x4bccbdc;
        *reinterpret_cast<float*>(rate) = frame_rate;

    }

    
    float GetFrameRate()
    {
        auto rate = reinterpret_cast<uint8_t*>(utils::memory::GetModuleInfo("").lpBaseOfDll) + 0x4bccbdc;
        return *reinterpret_cast<float *>(rate);
        
    }



	void AddServerToList(const std::string &server_name, unsigned long ip_address) {
		std::wstring serv_name = std::wstring(L"\xd\xd\0") + std::wstring(server_name.begin(), server_name.end());


        void *query_perf_counter_offs = static_cast<uint8_t *>(utils::memory::search("",
          "\x40\x53\x48\x83\xEC\xCC\x48\x8D\x4C\x24\xCC\xFF\x15\xCC\xCC\xCC\xCC\x48\x8B\x5C\x24\xCC"
          "\x48\x8D\x4C\x24\xCC\xFF\x15\xCC\xCC\xCC\xCC\x33\xD2\x8B\xCB\x48\x8B\xC3\x48\xC1\xE8\xCC",
          0xCC));

        void *add_to_server_list_offs = static_cast<uint8_t *>(utils::memory::search("",
          "\x40\x53\x48\x81\xEC\xCC\xCC\xCC\xCC\x4D\x8B\xD0\x48\x8B\xD9\x48\x8B\x09\x45\x33\xDB\x48\x3B\x4B\x08\x74\x33"
          "\xE8"
          "\xCC\xCC\xCC\xCC\x84\xC0\x75\x06\x48\x83\xC1\xCC\xEB\xEB",
          0xCC));

        static char ipAddr[] = { 0xc0, 0xa8, 0xc9,0x00,0x00, 0x00,0x00,0x00, 0x00, 0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0xCD,0X1B, 0x04,0x00,0xFF,0x00, 0x00, 0x00 };


        char *t = (char *)&ip_address;

        ipAddr[0] = t[0];
        ipAddr[1] = t[1];
        ipAddr[2] = t[2];
        ipAddr[3] = t[3];

        auto query_perf_ctr = (uint32_t(__stdcall *)())(query_perf_counter_offs);
        auto add_to_server_list = (void(__stdcall *)(
          uint8_t * nw_struct, void* ip, const wchar_t *computer_name, uint32_t update_time))(
          add_to_server_list_offs);
        add_to_server_list(reinterpret_cast<uint8_t *>(utils::memory::GetModuleInfo("").lpBaseOfDll) + 0x4d6f1f0,
          &ipAddr,
          serv_name.c_str(),
        query_perf_ctr());

	}

}// namespace engine::client