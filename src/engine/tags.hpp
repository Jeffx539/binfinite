#pragma once 

#include <cstdint>
#include <string>


// ripped from https://github.com/Surasia/InfiniteExt/blob/60c5d682d017b3c7d060ffd6fe019e8e5f676c41/InfExt/src/TagLoader/TagLoader.cpp

namespace engine::tags {

    // some help from https://github.com/ElDewrito/ElDorito/blob/a44c46d6edbfd4a54e314388a34e43c6e2363ddb/ElDorito/Source/Blam/BlamData.hpp#L69
    class DataArray
    {
      public:
        char Name[64];// 0x0000
        int32_t DatumSize;// 0x0040
        char pad_0044[36];// 0x0044
        int32_t NextIdx;// 0x0068
        int32_t FirtUnallocated;// 0x006C
        uint16_t N00000753;// 0x0070
        uint16_t N000009C3;// 0x0072
        uint16_t N00000754;// 0x0074
        uint16_t N000009C8;// 0x0076
        uint8_t *Data;// 0x0078
        char pad_0080[4056];// 0x0080
    };// Size: 0x1058
    


    class TagStruct
    {
      public:
        int32_t Datum;// 0x0000
        int32_t ObjectID;// 0x0004
        char **TagGroup;// 0x0008
        char **Data;// 0x0010
        char **Description;// 0x0018
        char pad_0020[96];// 0x0020
    };// Size: 0x0080




    // move this out lol we're just fixing broken shit rn

   class GrappleHookDefinition
    {
      public:
        int64_t N00000A51;// 0x0000
        int32_t N00000A52;// 0x0008
        char pad_000C[32];// 0x000C
        float N00000A57;// 0x002C
        char pad_0030[316];// 0x0030
        float MaxRange;// 0x016C
        float N00000BBA;// 0x0170
        char pad_0174[16];// 0x0174
        int32_t N00000A82;// 0x0184
        float N00000BCE;// 0x0188
        int32_t N00000A83;// 0x018C
        char pad_0190[4];// 0x0190
        float N00000A84;// 0x0194
        float N00000BC8;// 0x0198
        float N00000A85;// 0x019C
        float N00000BBC;// 0x01A0
        float N00000A86;// 0x01A4
        float N00000BBF;// 0x01A8
        float N00000A87;// 0x01AC
        char pad_01B0[20];// 0x01B0
        float OffScalGrounded;// 0x01C4
        float OffsScalarAirbourne;// 0x01C8
        float PullLaunchVerticalImpulse;// 0x01CC
        float PullAimImpulse;// 0x01D0
        float PullAccelDecayDuration;// 0x01D4
        float PullAccelerationPhase;// 0x01D8
        float PullBaseTargetVel;// 0x01DC
        float PullExitMaxVel;// 0x01E0
        char pad_01E4[1624];// 0x01E4
    };// Size: 0x083C



    void DumpTags();





}