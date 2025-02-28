#include "command.hpp"
#include "console.hpp"
#include "engine/client.hpp"
#include "engine/lua.hpp"
#include "engine/server.hpp"
#include "networking/rcon.hpp"
#include "environment.hpp"
#include "exports.hpp"
#include "patches.hpp"
#include "ui/hook.hpp"
#include "version.h"
#include <WinSock2.h>
#include <iostream>
#include <windows.h>
#include <ws2tcpip.h>
#include <thread>


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
    console::log("Starting Probe Thread");
    // move WSA Startup Somewhere else
    WSAData data;
    WSAStartup(MAKEWORD(2, 2), &data);

    unsigned long non_blocking = 1;

    SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    ioctlsocket(s, FIONBIO, &non_blocking);

    addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    if (getaddrinfo(MASTER_SERVER, NULL, &hints, &res) != 0) {
        throw("Failed to Get DNS for %s", MASTER_SERVER);
        return 1;
    }

    sockaddr_in *ipv4 = (sockaddr_in *)res->ai_addr;
    char ipstr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(ipv4->sin_addr), ipstr, INET_ADDRSTRLEN);

    sockaddr_in dest;
    dest.sin_family = AF_INET;
    dest.sin_port = htons(7676);
    dest.sin_addr = ipv4->sin_addr;
    sockaddr_in recvdfrom;

    while (true) {
        const char *getservers = "\xff\xff\xff\xffgetservers ";

        if (environment::IsServer()) {


            std::string hb = std::string("\xff\xff\xff\xff\heartbeat ") + engine::server::g_serverConfig.host;
            sendto(s, hb.c_str(), strlen(hb.c_str()), 0, (struct sockaddr *)&dest, sizeof(struct sockaddr_in));

        } else {


            int fromlen = sizeof(struct sockaddr_in);
            char buffer[1024];

            if (!engine::client::IsServerSelectionOpen()) {

                Sleep(200);
                continue;
            }

            // move out
            if (!is_hooked) {
                is_hooked = true;
                ui::hook::Init();
            }


            sendto(s, getservers, strlen(getservers), 0, (struct sockaddr *)&dest, sizeof(struct sockaddr_in));

            int bytes_recv = recvfrom(s, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&recvdfrom, &fromlen);
            if (bytes_recv == SOCKET_ERROR) {
                Sleep(2000);
                continue;
            }


            const char *response = "\xff\xff\xff\xffgetserversResponse ";
            if (std::memcmp(response, buffer, strlen(response) != 0)) {
                console::log("invalid Packet");
                continue;
            }


            // +1 remove first backslash
            auto spl = split(std::string(buffer + strlen(response) + 1, buffer + bytes_recv), '\\');

            struct
            {
                uint32_t ip_addr;
                uint16_t port;
                char hostname[256];

            } host;


            for (const auto &entry : spl) {
                if (entry.rfind("EOT", 0) == 0) { break; }
                std::memcpy(&host.ip_addr, entry.data(), sizeof(host.ip_addr));
                std::memcpy(&host.port, entry.data() + 4, sizeof(host.port));
                std::memcpy(&host.hostname, entry.data() + 6, entry.length() - 6);
                engine::client::AddServerToList(host.hostname, host.ip_addr);
            }
        }


        Sleep(2000);
    }
}


// main entrypoint for our client
void main()
{
    console::init();
    SetConsoleTitle("Binfinite");
    console::log("Binfinite %s", GIT_DESCRIBE);

    Sleep(500);

    auto env = environment::IsServer() ? "SERVER" : "CLIENT";
    console::log("initialising mode: (%s) ...", env);

    patches::common::PatchEAC();
    // engine::shared::lua::InstallHooks();


    if (environment::IsServer()) {
        

        patches::server::GetServerOpModeFuncTable();
        engine::server::InstallHooks();
        engine::server::LoadServerConfig(
          "./game/server.toml");// hoist somewhere and make it part of the CLI +exec server.toml
       

        if (engine::server::g_serverConfig.rcon_password != "") {
            std::thread t([]() {
                networking::RCON rcon(engine::server::g_serverConfig.rcon_password);
                rcon.listen();
            });

            t.detach();
        }


    } else {
        // client codepath
        // patches::client::PatchIntro();
        engine::server::UpdateTickRate(60);
    }



    CreateThread(NULL, 0, ProbeThread, NULL, 0, NULL);
    command::register_cmd("map_start", [](const std::vector<std::string> args) { engine::server::StartGame(); });
    command::register_cmd("map_end", [](const std::vector<std::string> args) { engine::server::EndMode(); });
    command::register_cmd(
      "map", [](const std::vector<std::string> args) { engine::server::SetupVariant(args[0], args[1]); });
    command::register_cmd(
      "hostname", [](const std::vector<std::string> args) { engine::server::g_serverConfig.host = args[0]; });
    command::register_cmd(
      "tickrate", [](const std::vector<std::string> args) { engine::server::UpdateTickRate(std::stoi(args[0])); });
    command::register_cmd(
      "fps", [](const std::vector<std::string> args) { engine::client::SetFrameRate(std::stof(args[0])); });
    command::register_cmd("fps_stats", [](const std::vector<std::string> args) { engine::server::ToggleFPSStats(); });
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
        mutex = CreateMutexA(NULL, TRUE, "UniqueDLLInstanceMutex");
        if (mutex == nullptr || GetLastError() == ERROR_ALREADY_EXISTS) {
            if (mutex != nullptr) {
                CloseHandle(mutex);
                mutex = nullptr;
            }
            return FALSE;
        }


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
