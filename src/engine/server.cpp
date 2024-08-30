#include "server.hpp"
#include "../utils/memory.hpp"
#include "../console.hpp"
#include <toml++/toml.hpp>
#include <bitset>
#include <iostream>
#include <stdio.h>


namespace engine::server {
	utils::hooks::detour sv_set_lan_command_hook;

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






	// naieve implementation
	uint64_t check_set_bit(uint64_t val) {
          
		for (uint64_t i = 0; i < 64; i++) {
            if ((val) & (1 << (i))) {
				return i;
			}
		}
      return 0; 

	}


	uint64_t set_lan_command_stub(void *var_array, char arg2, char *command, int32_t arg4) {

		LanCommandRequest *com = reinterpret_cast<LanCommandRequest *>(command);
         //   console::log("Set lan command %d %s", com->type, server_command_to_name(check_set_bit(com->type)).c_str());


		return sv_set_lan_command_hook.invoke<uint64_t>(var_array, arg2, command, arg4);
	}


	void InstallHooks() {
            console::log("Installing Server Hooks");
			void* set_lan_offs = static_cast<uint8_t*>(utils::memory::search(
              "", "\x73\x59\x48\x8B\x45\xCC\x8B\xCB\x48\x0F\xA3\xC8\x73\x43\x4C\x8B\x3E", 0xCC))  - 0x46;
            console::log("LAN Server %p", set_lan_offs);


           //sv_set_lan_command_hook.create(set_lan_offs, &set_lan_command_stub);


	}
        void *GetServerVar(const std::string var)
        {
            struct MaybeDatum
            {
                uint8_t *data;
                char pad_0000[8];// 0x8000
                char *name;// 0x1000
            };


            uint8_t *addr = static_cast<uint8_t *>(utils::memory::GetModuleInfo("").lpBaseOfDll) + 0x48e0568;
            uint8_t *offs = reinterpret_cast<uint8_t *>((*reinterpret_cast<uint8_t **>(addr) + 0x50ef8)) + 0x18;
            for (size_t i = 0; i < 27; i++) {
                MaybeDatum **nice = reinterpret_cast<MaybeDatum **>((offs + i * 8));
                if (std::string((*nice)->name) == var) { return nice; }
            }
            return nullptr;
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
    void SetupVariant(const std::string map, const std::string gamemode) {
        
        struct payload
        {
            uint32_t id;
            char mapVariant[16];
            char mapVersion[16];
            uint32_t id2;
            char gameVariant[16];
            char gameVersion[18]; // lol im corrupting the stack so i just added two bytes at the end, doesnt make a dif
        };


        


        if (g_serverConfig.map_variants.find(map) == g_serverConfig.map_variants.end()
            || g_serverConfig.game_variants.find(gamemode) == g_serverConfig.game_variants.end()) {

            console::log("failed to find map, server variant combo %s, %s",map.c_str(),gamemode.c_str());
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
  
        /// some weirdd shit happens with this stuff nfi
        //uint8_t *mod = reinterpret_cast<uint8_t *>(utils::memory::GetModuleInfo("").lpBaseOfDll);
        //auto func = (uint64_t * (__stdcall *)(void *, payload *))(mod + 0x2d8f2f0);
        //             func(*var, &pl);

        if (var != nullptr) {

            console::log("%p %p", *var, *var+0xc8);
            //reinterpret_cast<void*>((*var) + 0xc8) = 1;
            std::memset((*var) + 0xc0, 1, 1);
            std::memcpy((*var)+ 0xc8, &pl, sizeof(pl));
            console::log("Setting up %s,%s, %p", map.c_str(), gamemode.c_str(), *var);
           // assert(false);
        }

    }


    void StartGame() {
        uint8_t **var = reinterpret_cast<uint8_t**>(GetServerVar("start-mode"));
        static uint64_t val = 1;
        uint8_t *mod = reinterpret_cast<uint8_t*>(utils::memory::GetModuleInfo("").lpBaseOfDll);
        auto func = (uint64_t * (__stdcall *)(void *, uint64_t *))(mod + 0x2d8ef58);


        if (var != nullptr) { console::log("Starting Server");
            func(*var, &val);
        }
    }

        void EndMode()
    {
        uint8_t **var = reinterpret_cast<uint8_t **>(GetServerVar("end-game"));
        static uint64_t val = 1;
        uint8_t *mod = reinterpret_cast<uint8_t *>(utils::memory::GetModuleInfo("").lpBaseOfDll);
        auto func = (uint64_t * (__stdcall *)(void *, uint64_t *))(mod + 0x2d8f3a0);

        if (var != nullptr) {
            console::log("Ending Game");
            func(*var, &val);
        }
    }



    void FastRestart()
    {
        console::log("Fast Restart");
            uint8_t *mod =
          reinterpret_cast<uint8_t *>(utils::memory::GetModuleInfo("").lpBaseOfDll);
            
            //0x2465bcc
            auto func = (void *(__stdcall *)(uint64_t, uint64_t))(mod + 0x28455ac);
            //func(0x00000000FFFFFF00, 0);
       //     func(0, 1);
       //     func(1, 0);
       //     func(1, 1);
        ;
    }


    std::map<const std::string, Variant> _load_variants(toml::array arr) {

        std::map<const std::string, Variant> result;

        arr.for_each([&result](auto &&valtext) {
            auto node = toml::node_view(valtext);         
            result[node["name"].value_or("undefined")] =
              Variant{ .asset = node["asset"].value_or("undefined"), .version = node["version"].value_or("undefined") };
        });
    
        return result;

    }





    void LoadServerConfig(const std::string path)
    {

        auto config = toml::parse_file(path);

        //serverconfighost = config["server"]["hostname"].value_or("Halo Binfinite Dedicated Server");

        auto gamevars = config["server"]["gamevariants"].as_array();

        g_serverConfig.game_variants = _load_variants(*config["server"]["gamevariants"].as_array());
        g_serverConfig.map_variants = _load_variants(*config["server"]["mapvariants"].as_array());

   /*     return serverconfig;*/
    }


}
