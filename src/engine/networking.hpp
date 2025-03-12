#pragma once
#include <stdint.h>
#include "../utils/memory.hpp"
#include "../console.hpp"
namespace engine::networking {


class SessionMembership
{
  public:
    void *Unk01;// 0x0000
    int32_t TotalPacketsSent;// 0x0008
    char pad_000C[52];// 0x000C
    int32_t PeerCount;// 0x0040
    int32_t PeerCount2;// 0x0044
    char pad_0048[248];// 0x0048


    static SessionMembership *GetInstance()
    {
        return reinterpret_cast<SessionMembership *>((*reinterpret_cast<uint8_t **>(GAME_PTR(0x4d725c0))) + 0x60);
    }

    static int32_t GetFirstPeer()
    {
        auto get_first_peer = (int32_t(__stdcall *)(SessionMembership *))(GAME_PTR(0x0528d94));
        return get_first_peer(SessionMembership::GetInstance());
    }

    static int32_t GetNextPeer(int32_t last)
    {
        auto get_next_peer = (int32_t(__stdcall *)(SessionMembership *, int32_t last))(GAME_PTR(0x0529808));
        return get_next_peer(SessionMembership::GetInstance(), last);
    }


    static uint64_t *PeerXUID(int32_t peer) {
        // c style casts look better here
       return (uint64_t *)(reinterpret_cast<uint8_t *>(SessionMembership::GetInstance()) + (peer * 0x1308) + 0x2508);
    }

    static wchar_t *PeerName(int32_t peer)
    {
        return (wchar_t *)(reinterpret_cast<uint8_t *>(SessionMembership::GetInstance()) + (peer * 0x1308) + 0x2508 + 0x8);
    }

    static uint8_t *TeamIdx1(int32_t peer)
    {
        return (uint8_t *)(reinterpret_cast<uint8_t *>(SessionMembership::GetInstance()) + (peer * 0x1308) + 0x2b0e);
    }

    // not sure which one to use
    static uint8_t *TeamIdx2(int32_t peer)
    {
        return (uint8_t *)(reinterpret_cast<uint8_t *>(SessionMembership::GetInstance()) + (peer * 0x1308) + 0x2b0c);
    }

    // not really aprt of sessionmembership
    static uint32_t* NetworkCounter() {
        return (uint32_t *)(reinterpret_cast<uint8_t *>(SessionMembership::GetInstance()) - 0x60 + 0x4f314);
    }





};// Size: 0x0140

}

