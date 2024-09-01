#include "console.hpp"
#include "engine/server.hpp"
#include "engine/client.hpp"
#include "environment.hpp"
#include "exports.hpp"
#include "patches.hpp"
#include "version.h"
#include <iostream>
#include <windows.h>


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

// main entrypoint for our client
void main()
{
    console::init();
    SetConsoleTitle("Binfinite");
    console::log("Binfinite %s", GIT_DESCRIBE);

    Sleep(1000);
    auto env = environment::IsServer() ? "SERVER" : "CLIENT";
    console::log("initialising mode: (%s) ...", env);



    // todo hoist this somewhere else
    patches::common::PatchEAC();

    if (environment::IsServer()) {
            patches::server::PatchAddFTL();
            patches::server::GetServerOpModeFuncTable();

            engine::server::InstallHooks();
            engine::server::LoadServerConfig("./game/server.toml"); // hoist somewhere and make it part of the CLI +exec server.toml


    } else {
        // client codepath
        patches::client::PatchIntro();
    
    }







    //engine::server::StartGame();

     for (std::string line; std::getline(std::cin, line);) {

        std::vector<std::string> spl = split(line, ' ');
       
        if (spl[0].compare(std::string("map_start")) == 0) { engine::server::StartGame(); }


        if (spl[0].compare(std::string("map_end")) == 0) { engine::server::EndMode(); }


        if (spl[0].compare(std::string("fast_restart")) == 0) {

            engine::server::FastRestart();
        }

           if (spl[0].compare(std::string("map")) == 0) { 
               
               engine::server::SetupVariant(spl[1], spl[2]); 
           
           }

     if (spl[0].compare(std::string("connect")) == 0) { engine::client::ServerConnect(spl[1]); }


        /*
        if (spl[0].compare(std::string("tickrate")) == 0) {

            if (spl.size() != 2) {
                std::cout << "invalid args..  tickrate <val>" << std::endl;
            }

            SetTickRate(std::stoi(spl[1]));
        }
        */


        if (spl[0].compare(std::string("unload")) == 0) { break; }

        std::cout << "> ";
    }



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
