#include "command.hpp"
#include "console.hpp"
#include "engine/client.hpp"
#include "engine/lua.hpp"
#include "engine/shared.hpp"
#include "engine/server.hpp"
#include "engine/networking.hpp"
#include "engine/tags.hpp"
#include "environment.hpp"


#include "exports.hpp"
#include "patches.hpp"
#include "ui/hook.hpp"
#include "version.h"
#include <WinSock2.h>
#include <iostream>
#include <thread>
#include <windows.h>
#include <ws2tcpip.h>


HANDLE mutex = nullptr;
bool is_hooked = false;
#define MASTER_SERVER "binfinitemaster.lh2.au"


namespace client {

std::vector<std::string> split(const std::string &s, char seperator)
{
    std::vector<std::string> output;

    std::string::size_type prev_pos = 0, pos = 0;

    while ((pos = s.find(seperator, pos)) != std::string::npos) {
        std::string substring(s.substr(prev_pos, pos - prev_pos));
        output.push_back(substring);
        prev_pos = ++pos;
    }
    output.push_back(s.substr(prev_pos, pos - prev_pos));// Last word

    return output;
}


DWORD WINAPI ProbeThread(LPVOID params)
{
    sockaddr_in recvdfrom;

    while (true) {

        if (environment::IsServer()) {
            std::string hb =
            std::string("\xff\xff\xff\xff\heartbeat ") + engine::server::g_serverConfig.server_name.c_str();
            engine::shared::GameSocketSend(std::string(MASTER_SERVER), 7676, hb);
            console::log("sending server heartbeat to %s", MASTER_SERVER);
           
        }
        Sleep(30000);

    }
}



void RegisterSharedCommands() {
    // shared commands currently live here we need to move them
    command::register_cmd(
      "tickrate", [](const std::vector<std::string> args) { 
            auto rate = std::stoi(args[0]);
            engine::server::UpdateTickRate(rate); 

            // lol
            if (environment::IsClient) engine::client::FixGrapple(-1.f / 120.f * static_cast<float>(rate) + 1.5f);
        
        });
    command::register_cmd(
      "fps", [](const std::vector<std::string> args) { engine::client::SetFrameRate(std::stof(args[0])); });
    command::register_cmd("fps_stats", [](const std::vector<std::string> args) { engine::server::ToggleFPSStats(); });
    command::register_cmd(
      "connect", [](const std::vector<std::string> args) { engine::client::ConnectToServer(args[0]); });
    command::register_cmd(
      "lua_run", [](const std::vector<std::string> args) { engine::shared::lua::DoString(args[0].c_str()); });


     command::register_cmd("dump_tags", [](const std::vector<std::string> args) { engine::tags::DumpTags(); });



    command::register_cmd("status", [](const std::vector<std::string> args) {
        setlocale(LC_ALL, "");


        console::log("status %d connected peers", engine::networking::SessionMembership::GetInstance()->PeerCount);
        console::log("lobby session packet counter %d %d",
          engine::networking::SessionMembership::GetInstance()->TotalPacketsSent,
          *engine::networking::SessionMembership::NetworkCounter());
        console::log("");

        console::log("ID  XUID             TeamIdx Name");
        console::log("--- ---------------- ------- --------------------");
        int32_t peer_idx = engine::networking::SessionMembership::GetFirstPeer();
        while (peer_idx != -1) {
            console::log("%01d %llu %d %d %ls ",
              peer_idx,
              *engine::networking::SessionMembership::PeerXUID(peer_idx),
              *engine::networking::SessionMembership::TeamIdx1(peer_idx),
              *engine::networking::SessionMembership::TeamIdx2(peer_idx),
              engine::networking::SessionMembership::PeerName(peer_idx));


            peer_idx = engine::networking::SessionMembership::GetNextPeer(peer_idx);
        }
    });



    command::register_cmd("rt_offsdebug", [](const std::vector<std::string> args) {
        console::log("Network Session Global %p", engine::networking::SessionMembership::GetInstance() - 0x60);
        console::log("Session Membership %p", engine::networking::SessionMembership::GetInstance());

    });




}

// main entrypoint for our client
void main()
{
    console::init();
    SetConsoleTitle("Binfinite");
    console::log("Binfinite %s", GIT_DESCRIBE);
    console::log("Waiting for game to load into memory");
    Sleep(2000);

    auto env = environment::IsServer() ? "SERVER" : "CLIENT";
    console::log("initialising mode: (%s) ...", env);

    patches::common::PatchEAC();
    //engine::shared::lua::InstallHooks();
    engine::shared::networking::InstallHooks();



    RegisterSharedCommands();
    if (environment::IsServer()) {
        engine::server::Init();
    } else {
        engine::client::Init();
        engine::server::UpdateTickRate(60);
    }


    CreateThread(NULL, 0, ProbeThread, NULL, 0, NULL);
  

   


    for (std::string line; std::getline(std::cin, line);) { command::process_command(line); }


  
}
}// namespace client


BOOL WINAPI DllMain(HINSTANCE hinstDLL,// handle to DLL module
  DWORD fdwReason,// reason for calling function
  LPVOID lpvReserved)// reserved
{
    // Perform actions based on the reason for calling.
    switch (fdwReason) {
    case DLL_PROCESS_ATTACH:
        // call main
        DisableThreadLibraryCalls(hinstDLL);
        /*mutex = CreateMutexA(NULL, TRUE, "UniqueDLLInstanceMutex");
        if (mutex == nullptr || GetLastError() == ERROR_ALREADY_EXISTS) {
            if (mutex != nullptr) {
                CloseHandle(mutex);
                mutex = nullptr;
            }
            return FALSE;
        }*/


        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)client::main, hinstDLL, 0, nullptr);
        break;

    case DLL_THREAD_ATTACH:
        // Do thread-specific initialization.
        break;

    case DLL_THREAD_DETACH:
        // Do thread-specific cleanup.
        break;

    case DLL_PROCESS_DETACH:

        if (lpvReserved != nullptr) {
            break;// do not do cleanup if process termination scenario
        }

        // Perform any necessary cleanup.
        break;
    }
    return TRUE;// Successful DLL_PROCESS_ATTACH.
}
