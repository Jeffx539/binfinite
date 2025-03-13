#include "server.hpp"
#include "../console.hpp"
#include "../utils/memory.hpp"
#include <bitset>
#include <iostream>
#include <stdio.h>
#include <../patches.hpp>
#include <sstream>
#include "networking.hpp"
#include <toml++/toml.hpp>

#include <../command.hpp>

namespace engine::server {
utils::hooks::detour sv_set_lan_command_hook;
utils::hooks::detour sv_frame_info_hook;
utils::hooks::detour sv_printf_hook;

static int sv_fixemptyftl = 0;

// needs a better name
struct LanCommandRequest
{
    char pad_0000[8];// 0x0000
    uint64_t type;// 0x0008
};

struct ServerVariant
{
    uint8_t mapVariant[64];
    uint8_t mapVersion[64];
    uint8_t gameVariant[64];
    uint8_t gameVersion[64];
};


uint32_t GetTickRate()
{
    auto rater = GAME_PTR(0x50d5720); // mar13
    return *reinterpret_cast<uint32_t *>(rater);
}


void UpdateTickRate(uint64_t rate)
{

    auto en = GAME_PTR(0x4e05ee4); // mar13
    *reinterpret_cast<char *>(en) = 1;


    auto rater = GAME_PTR(0x50d5720); // mar13
    *reinterpret_cast<uint32_t *>(rater) = rate;

}


// naieve implementation
uint64_t check_set_bit(uint64_t val)
{

    for (uint64_t i = 0; i < 64; i++) {
        if ((val) & (1 << (i))) { return i; }
    }
    return 0;
}


uint64_t set_lan_command_stub(void *var_array, char arg2, char *command, int32_t arg4)
{

    LanCommandRequest *com = reinterpret_cast<LanCommandRequest *>(command);
    return sv_set_lan_command_hook.invoke<uint64_t>(var_array, arg2, command, arg4);
}





uint8_t *GetServerVar(const std::string var)
{
    struct MaybeDatum
    {
        uint8_t *data;
        char pad_0000[8];// 0x8000
        char *name;// 0x1000
    };

  
     uint8_t *addr = GAME_PTR(0x493ffa8);
    uint8_t *offs = reinterpret_cast<uint8_t *>((*reinterpret_cast<uint8_t **>(addr) + 0x53fd8)) + 0x18;

    for (size_t i = 0; i < 27; i++) {
        MaybeDatum **nice = reinterpret_cast<MaybeDatum **>((offs + i * 8));
        if (std::string((*nice)->name) == var) { return reinterpret_cast<uint8_t*>(nice); }
    }
    return nullptr;
}

void *GetServerFunc(uint8_t* address)
{ 

    auto addr = *reinterpret_cast<uint8_t ***>(address);
    return reinterpret_cast<void*>(* reinterpret_cast<void**>((*addr + 0x78)));
}



void UpdateFTL(uint64_t valu)
{
    auto var = GetServerVar("lanFTLXuid");
    auto func = (uint64_t * (__stdcall *)(uint8_t *, uint64_t *))(GetServerFunc(var));
    uint64_t val = valu;
    func(*reinterpret_cast<uint8_t **>(var), &val);
}



void FormatUUID(char *formatted, const std::string id)
{
    sscanf(id.c_str(),
      "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
      &formatted[0],
      &formatted[4],
      &formatted[6],
      &formatted[8],
      &formatted[9],
      &formatted[10],
      &formatted[11],
      &formatted[12],
      &formatted[13],
      &formatted[14],
      &formatted[15]);
}
void SetupVariant(const std::string map, const std::string gamemode)
{

    struct payload
    {
        uint32_t id;
        char mapVariant[16];
        char mapVersion[16];
        uint32_t id2;
        char gameVariant[16];
        char gameVersion[18];// lol im corrupting the stack so i just added two bytes at the end, doesnt make a dif
    };


    if (g_serverConfig.map_variants.find(map) == g_serverConfig.map_variants.end()
        || g_serverConfig.game_variants.find(gamemode) == g_serverConfig.game_variants.end()) {

        console::log("failed to find map, server variant combo %s, %s", map.c_str(), gamemode.c_str());
        return;
    }
    uint8_t **var = reinterpret_cast<uint8_t **>(GetServerVar("customPlaylist"));

    payload pl;

    pl.id = 2;
    FormatUUID(pl.mapVariant, g_serverConfig.map_variants[map].asset);
    FormatUUID(pl.mapVersion, g_serverConfig.map_variants[map].version);
    pl.id2 = 6;
    FormatUUID(pl.gameVariant, g_serverConfig.game_variants[gamemode].asset);
    FormatUUID(pl.gameVersion, g_serverConfig.game_variants[gamemode].version);


    if (var != nullptr) {

        console::log("%p %p", *var, *var + 0xc8);
        // reinterpret_cast<void*>((*var) + 0xc8) = 1;
        std::memset((*var) + 0xc0, 1, 1);
        std::memcpy((*var) + 0xc8, &pl, sizeof(pl));
        console::log("Setting up %s,%s, %p", map.c_str(), gamemode.c_str(), *var);
        // assert(false);
    }
}



void StartGame()
{
    auto var = GetServerVar("start-mode");
    auto func = (uint64_t * (__stdcall *)(uint8_t *, uint64_t *))(GetServerFunc(var));
    uint64_t val = 1;

    if (var != nullptr) {
        console::log("Starting Game");
        func(*reinterpret_cast<uint8_t **>(var), &val);
    }
}

void EndMode()
{
    auto var = GetServerVar("end-game");
    auto func = (uint64_t * (__stdcall *)(uint8_t *, uint64_t *))(GetServerFunc(var));
    uint64_t val = 1;

    if (var != nullptr) {
        console::log("Ending Game");
        func(*reinterpret_cast<uint8_t **>(var), &val);
    }
}


void FastRestart()
{
    console::log("Fast Restart");
    uint8_t *mod = reinterpret_cast<uint8_t *>(utils::memory::GetModuleInfo("").lpBaseOfDll);

    // 0x2465bcc
    auto func = (void *(__stdcall *)(uint64_t, uint64_t))(mod + 0x28455ac);
    // func(0x00000000FFFFFF00, 0);
    //     func(0, 1);
    //     func(1, 0);
    //     func(1, 1);
    ;
}



uint64_t Hook_FrameInfo(FILE *file, char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    return 1;
}

void ToggleFPSStats()
{
    auto stats = GAME_PTR(0x4d69760); // mar13
    *stats = ~*stats;
}




void Hook_ServerPrintf(char arg1, char arg2, char arg3, char arg4, int64_t arg5, char arg6)
{

    //console::log("%d %d %d %d %s %d", arg1, arg2, arg3, arg4, arg5, arg6);
    return;
}



// 343 is terrible, lifecycle is synced from life-cycle. It sends the wrong lifecycle to new connecting hosts. this is to run in a loop to look for 
// 00 00 00 00 01 00 01 00. if this is found it should zero it out to prevent the lobby from breaking lol. There's a bug in their lifecycle implementation
DWORD WINAPI FixLobbyLC(LPVOID params){ 
    while (true) {    
        auto lc_ptr = *reinterpret_cast<uint8_t **>(GetServerVar("life-cycle"));
        auto magic = reinterpret_cast<uint64_t *>(lc_ptr + 0xc8);

        if (*magic == 281479271677952) { 
            console::log("Fixing Lobby life-cycle to prevent players from breaking.");
            *magic = 0;
        }


        if (sv_fixemptyftl) { 
            auto ftl_ptr = *reinterpret_cast<uint8_t **>(GetServerVar("lanFTLXuid"));
            auto magic = reinterpret_cast<uint64_t *>(ftl_ptr + 0xc8);

            if (!engine::networking::SessionMembership::GetInstance()->PeerCount && *magic) {
                console::log("Peer Count 0 when FTL is set, fixing");
                UpdateFTL(0);
            }
                
        }

        Sleep(100);
    }
}


void RegisterCommands() {
    command::register_cmd(
      "sv_setftl", [](const std::vector<std::string> args) { engine::server::UpdateFTL(std::stoull(args[0])); });


    command::register_cmd("setteam", [](const std::vector<std::string> args) {
        *engine::networking::SessionMembership::TeamIdx1(std::stoi(args[0])) = std::stoi(args[1]);
        engine::networking::SessionMembership::GetInstance()->TotalPacketsSent++;
    });

     command::register_cmd(
      "sv_fixemptyftl", [](const std::vector<std::string> args) { sv_fixemptyftl = std::stoi(args[0]); });

    command::register_cmd("map_start", [](const std::vector<std::string> args) { engine::server::StartGame(); });
    command::register_cmd("map_end", [](const std::vector<std::string> args) { engine::server::EndMode(); });
    command::register_cmd(
      "map", [](const std::vector<std::string> args) { engine::server::SetupVariant(args[0], args[1]); });
    command::register_cmd("hostname", [](const std::vector<std::string> args) {
        engine::server::g_serverConfig.server_name = args[0];
    });


    command::register_cmd("add_map_variant", [](const std::vector<std::string> args) {
        engine::server::StartGame(); 
    });


    command::register_cmd("add_map_variant", [](const std::vector<std::string> args) {
        g_serverConfig.map_variants[args[0]] = Variant{ .asset = args[1], .version = args[2] };
    });

     command::register_cmd("add_game_variant", [](const std::vector<std::string> args) {
        g_serverConfig.game_variants[args[0]] = Variant{ .asset = args[1], .version = args[2] };
    });

    command::register_cmd(
      "rcon_password", [](const std::vector<std::string> args) { engine::server::g_serverConfig.rcon_password = args[0]; });


}


void ExecuteConfig(std::string path) {
    std::ifstream file(path);

    console::log("Executing %s", path.c_str());
    std::string line;

    while (std::getline(file, line)) {
        // strip new lines before feeding into command executor
        line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());

        if (line.empty()) { continue; }
        command::process_command(line);
    }



}



void Init() {
    console::log("Server Init");

    patches::server::GetServerOpModeFuncTable();
    g_serverConfig.server_name = "Binfinite Dedicated Server";
    g_serverConfig.rcon_password = "";


    RegisterCommands();
    InstallHooks();

    // exec server.cfg 
    ExecuteConfig("./server.cfg");


}




void InstallHooks()
{
    console::log("Installing Server Hooks/Patches");
    CreateThread(NULL, 0, FixLobbyLC, NULL, 0, NULL);
    sv_frame_info_hook.create(GAME_PTR(0x250cfcc), &Hook_FrameInfo);// mar13




    //sv_printf_hook.create(module_base + 0x08c9ebc, &Hook_ServerPrintf);
}

}// namespace engine::server
