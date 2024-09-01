

#include "../utils/hooks.hpp"
#include "../utils/memory.hpp"
#include "../console.hpp"
#include <WinSock2.h>

#include "client.hpp"
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")


namespace engine::client {

void ServerConnect(const std::string &address)
{
    // old code from prev project, will gut soon
    static wchar_t computer_name[] = L"\xd\0BINFINITEBINFIN";
    uint8_t *comp = (uint8_t *)malloc(sizeof(computer_name) + 4);

    // 0x05c5e34
    void *query_perf_counter_offs = static_cast<uint8_t *>(utils::memory::search("",
      "\x40\x53\x48\x83\xEC\xCC\x48\x8D\x4C\x24\xCC\xFF\x15\xCC\xCC\xCC\xCC\x48\x8B\x5C\x24\xCC"
      "\x48\x8D\x4C\x24\xCC\xFF\x15\xCC\xCC\xCC\xCC\x33\xD2\x8B\xCB\x48\x8B\xC3\x48\xC1\xE8\xCC",
      0xCC));
    auto query_perf_ctr = (uint32_t(__stdcall *)())(query_perf_counter_offs);
    console::log("%p", query_perf_counter_offs);

    // 0x2e15e0c
    void *add_to_server_list_offs = static_cast<uint8_t *>(utils::memory::search("",
      "\x40\x53\x48\x81\xEC\xCC\xCC\xCC\xCC\x4D\x8B\xD0\x48\x8B\xD9\x48\x8B\x09\x45\x33\xDB\x48\x3B\x4B\x08\x74\x33\xE8"
      "\xCC\xCC\xCC\xCC\x84\xC0\x75\x06\x48\x83\xC1\xCC\xEB\xEB",
      0xCC));

        console::log("%p", add_to_server_list_offs);

    auto add_to_server_list = (void(__stdcall *)(
      uint8_t * nw_struct, void *ip, void *computer_name, uint32_t update_time))(add_to_server_list_offs);


    // 0x2015340
    void *connect_to_server_offs = static_cast<uint8_t *>(utils::memory::search("",
      "\x48\x89\x5C\x24\xCC\x48\x89\x6C\x24\xCC\x48\x89\x74\x24\xCC\x57\x41\x56\x41\x57\x48\x83\xEC\xCC\x4C\x8B\xF1"
      "\x0F\x57\xC0\xF3\x0F\x7F\x44\x24\xCC\x48\xC7\x44\x24\xCC\xCC\xCC\xCC\xCC\x48\x8B\x2D\xCC\xCC\xCC\xCC\x48\x8B"
      "\x35\xCC\xCC\xCC\xCC",
      0xCC));
    auto connect_to_server = (void(__stdcall *)(uint64_t * server_Id))(connect_to_server_offs);
    console::log("%p", connect_to_server);





    unsigned long ip = inet_addr(address.c_str());
    ip = ntohl(ip);


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

    char *t = (char *)&ip;

    ipAddr[0] = t[3];
    ipAddr[1] = t[2];
    ipAddr[2] = t[1];
    ipAddr[3] = t[0];
    *comp = 0xD;

    static uint64_t id = 0;

    add_to_server_list(reinterpret_cast<uint8_t *>(utils::memory::GetModuleInfo("").lpBaseOfDll) + 0x4d154f0,
      ipAddr,
      computer_name,
      query_perf_ctr());
    connect_to_server(&id);
}

}// namespace engine::client