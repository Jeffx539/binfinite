
#pragma once
#include <WinSock2.h>
#include <string>

namespace engine::client {

void AddServerToList(const std::string &server_name, unsigned long ip_address);
float GetFrameRate();
float GetInputLat();
bool IsServerSelectionOpen();
void Init();
bool GetNWPerf();
void FixGrapple(float frac);
void SetNWPerf(bool state);

char *GetRTTString();

void ConnectToServer(std::string ip_addr);
uint64_t GetLobbyActivityState();
void SetFrameRate(float frame_rate);
// void AddServerToList(const std::string &server_name, const std::string &ip_address)
//{
//     unsigned long ip = inet_addr(ip_address.c_str());
//     ip = ntohl(ip);
//     AddServerToList(server_name, ip);
// };
}// namespace engine::client