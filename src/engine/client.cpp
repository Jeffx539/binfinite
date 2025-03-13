
#include "../console.hpp"
#include "../utils/hooks.hpp"
#include "../ui/hook.hpp"
#include "../utils/memory.hpp"
#include <WinSock2.h>

#include "client.hpp"
#include "server.hpp"
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")


namespace engine::client {

    void SetNWPerf(bool state) {
        auto addrt = GAME_PTR(0x4f364a4); // mar13
        *reinterpret_cast<bool *>(addrt) = state;
    }

    bool GetNWPerf() {
        auto addrt = GAME_PTR(0x4f364a4);// mar13
        return *reinterpret_cast<bool *>(addrt);
    }

    bool IsServerSelectionOpen()
    {
        auto addr = GAME_PTR(0x4D75324); // mar13
        return *reinterpret_cast<bool *>(addr);
    }

    float GetInputLat()
    {
        auto addrt = GAME_PTR(0x4bd2c5c); // mar13
        return *reinterpret_cast<float *>(addrt);
    }


    char *GetRTTString()
    {
        auto addr = GAME_PTR(0x4d3d8f0);
        return reinterpret_cast<char *>(addr);
    }


    void SetFrameRate(float frame_rate)
    {
        auto rate = GAME_PTR(0x4bd2c5c); // mar13
        *reinterpret_cast<float *>(rate) = frame_rate;
    }


    float GetFrameRate()
    {
        auto rate = GAME_PTR(0x4bd2c5c); // mar13
        return *reinterpret_cast<float *>(rate);
    }


void Init() {

    // wait for sn05 main menu to load using jank
    auto wait_offs = GAME_PTR(0x507CAA7); // mar13
    while (strcmp(reinterpret_cast<char *>(wait_offs), "levels\\ui\\mainmenu_sn05") < 0) {
        //console::log("%d", strcmp(reinterpret_cast<char *>(wait_offs), "levels\\ui\\mainmenu_sn05"));
        Sleep(10);
    }

    console::log("Initialising client");
    ui::hook::Init();
}


uint64_t GetLobbyActivityState() {
    auto get_lob_activity_offs = GAME_PTR(0x04eaba8);// mar13
    auto get_lob_activity = (int64_t(__stdcall *)())(get_lob_activity_offs);
    return get_lob_activity();
}


void ConnectToServer(std::string ip_addr) {

        auto offs = GAME_PTR(0x2e6c59c); // mar 13
        auto test = (void(__stdcall *)(int32_t arg1, int32_t arg2, char arg3, char arg4))(offs);


        auto connect_to_server_offs = GAME_PTR(0x3541d84); // mar13
        auto connect_to_server = (void(__stdcall *)(uint64_t arg1))(connect_to_server_offs);

       auto can_lan_offs = GAME_PTR(0x04f06ec);  // mar13
        auto can_lan = (int64_t(__stdcall *)())(can_lan_offs);


        auto req_lob_activity_offs = GAME_PTR(0x346fbe4);// mar13
        auto req_lob_activity = (int64_t(__stdcall *)(int64_t arg1, int32_t arg2))(req_lob_activity_offs);

        sockaddr_in sa;
        inet_pton(AF_INET, ip_addr.c_str(), &(sa.sin_addr));

        req_lob_activity(0, 0);

        // bound the loop
        for (unsigned int i = 0; i < 100; i++) {

            if (GetLobbyActivityState() == 2) { break; }
            console::log("Still transitioning");
            Sleep(100);
        }



        engine::client::AddServerToList("Binfinite Server", sa.sin_addr.s_addr);
        Sleep(20);
        console::log("connecting to id %s", ip_addr.c_str());
        if (can_lan()) {
            connect_to_server(0);
        }


}


void AddServerToList(const std::string &server_name, unsigned long ip_address)
{
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

    static char ipAddr[] = { 0xc0,
        0xa8,
        0xc9,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0xCD,
        0X1B,
        0x04,
        0x00,
        0xFF,
        0x00,
        0x00,
        0x00 };


    char *t = (char *)&ip_address;

    ipAddr[0] = t[0];
    ipAddr[1] = t[1];
    ipAddr[2] = t[2];
    ipAddr[3] = t[3];

    auto query_perf_ctr = (uint32_t(__stdcall *)())(query_perf_counter_offs);
    auto add_to_server_list = (void(__stdcall *)(
      uint8_t *nw_struct, void *ip, const wchar_t *computer_name, uint32_t update_time))(add_to_server_list_offs);
    add_to_server_list(GAME_PTR(0x4d75270), // mar13
      &ipAddr,
      serv_name.c_str(),
      query_perf_ctr());
}

}// namespace engine::client