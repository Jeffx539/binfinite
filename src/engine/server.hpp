#pragma once
#include "../utils/hooks.hpp"

#include <map>




namespace engine::server {

void InstallHooks();
void StartGame();
void EndMode();
void FastRestart();
void SetupVariant(const std::string map, const std::string gamemode);

struct Variant
{
    std::string asset;
    std::string version;
};

struct ServerConfig
{

    std::string host;
    uint16_t maxplayers;


    std::map<const std::string, Variant> game_variants;
    std::map<const std::string, Variant> map_variants;

    std::vector<std::string> rotation;
};


static ServerConfig g_serverConfig;



void  LoadServerConfig(const std::string path);





enum LanServerCommand {
    LifeCycle,
    UIGameState,
    UNK_0,
    StartMode,
    EndGame,
    UNK_1,
    InitialParticipants,
    GameInstance,
    Language,
    SimProtocol,
    GameVariant,
    MapVariant,
    BakeContent,
    CustomPlaylist,
    CurrentGameIdx,
    LanFTLXuid,
    LanObserversAllowed,
    LanLobbyMaxPlayers,
    LanCustomGameParameter,
    VoiceNetworkHostXuid,
    GameStartStatus,
    CountdownTimer,
    IntroTimer,
    IntermissionTimer,
    ReadyRoomTimer,
    RequestCampaignQuit,
    ChatServerSessionState


};







static const std::string server_command_to_name(uint64_t t) {

    switch (t) {
    case LifeCycle:
        return "LifeCycle";
    case UIGameState:
        return "UIGameState";
    case UNK_0:
        return "UNK_0";
    case StartMode:
        return "StartMode";
    case EndGame:
        return "EndGame";
    case UNK_1:
        return "UNK_1";
    case InitialParticipants:
        return "InitialParticipants";
    case GameInstance:
        return "GameInstance";
    case Language:
        return "Language";
    case SimProtocol:
        return "SimProtocol";
    case GameVariant:
        return "GameVariant";
    case MapVariant:
        return "MapVariant";
    case BakeContent:
        return "BakeContent";
    case CustomPlaylist:
        return "CustomPlaylist";
    case CurrentGameIdx:
        return "CurrentGameIdx";
    case LanFTLXuid:
        return "LanFTLXuid";
    case LanObserversAllowed:
        return "LanObserversAllowed";                                                                                                                                                               
    case LanLobbyMaxPlayers:
        return "LanLobbyMaxPlayers";
    case LanCustomGameParameter:
        return "LanCustomGameParameter";
    case VoiceNetworkHostXuid:
        return "VoiceNetworkHostXuid";
    case CountdownTimer:
        return "CountdownTimer";
    case IntroTimer:
        return "IntroTimer";
    case IntermissionTimer:
        return "IntermissionTimer";
    case GameStartStatus:
        return "GameStartStatus";
    case ReadyRoomTimer:
        return "ReadyRoomTimer";                                                                                                                                                                                                                                                                                                 
    case RequestCampaignQuit:
        return "RequestCampaignQuit";
    case ChatServerSessionState:
        return "ChatServerSessionState";  
    default:
        return "no fuckin idea";
                                                                                                                                                                                                                                                                                        
    }

}





}