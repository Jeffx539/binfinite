
#include "patches.hpp"
#include "console.hpp"
#include "utils/memory.hpp"
#include "utils/hooks.hpp"

namespace patches {



    namespace server {
    
    void PatchAddFTL()

        {
            uint8_t *addr = static_cast<uint8_t *>(utils::memory::GetModuleInfo("").lpBaseOfDll) + 0x48e0568;
            uint8_t **cool = reinterpret_cast<uint8_t **>((*reinterpret_cast<uint8_t **>(addr) + 0x50ef8)) + 18;
            *(*cool + 0xc8) = 0x1;
            *(*cool + 0xc0) = 0x1;
        }


        void GetServerOpModeFuncTable()
        {

            struct MaybeDatum
            {
                uint8_t *data;
                char pad_0000[8];// 0x8000
                char *name;// 0x1000
            };


            uint8_t *addr = static_cast<uint8_t *>(utils::memory::GetModuleInfo("").lpBaseOfDll) + 0x48e0568;
            uint8_t *offs = reinterpret_cast<uint8_t *>((*reinterpret_cast<uint8_t **>(addr) + 0x50ef8)) + 0x18;

            for (size_t i = 0; i < 26; i++) {

                // lazy mult
                MaybeDatum **nice = reinterpret_cast<MaybeDatum **>((offs + i * 8));
                console::log("%p -> %s -> %p", nice, (*nice)->name, ((*nice)->data + 0x78));


                /*      MaybeDatum *data = reinterpret_cast<MaybeDatum *>((offs+i*8));
                      console::log("%s -> %p", data->name, data->data);*/
            }
        }
    
    };



    namespace common {


   



        void PatchEAC() { console::log("Patching EAC");
            uint8_t *addr = static_cast<uint8_t *>(utils::memory::GetModuleInfo("").lpBaseOfDll) + 0x504a278;
            uint8_t *addr1 = static_cast<uint8_t *>(utils::memory::GetModuleInfo("").lpBaseOfDll) + 0x504a230;
            uint8_t *addr2 = static_cast<uint8_t *>(utils::memory::GetModuleInfo("").lpBaseOfDll) + 0x0504a2c0;

           
            *addr = 0;
            *addr1 = 0;
            *addr2 = 0;
        }

    }


    namespace client {
        //
        // 
        // todo "\xE8\x00\x00\x00\x00\x90\x48\x8B\x05\x00\x00\x00\x00\x48\x8B\x58\x00\x48\x8D\x4B\x00\xE8\x00\x00\x00\x00", "x????xxxx????xxx?xxx?x????"
        // im about to get braindamage from this intro
        void PatchIntro() {

            uint8_t *addr = static_cast<uint8_t *>(utils::memory::GetModuleInfo("").lpBaseOfDll) + 0x528d650;
            *addr = 1;
        }


    }




}// namespace patches::server
