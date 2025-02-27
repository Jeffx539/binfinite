
#pragma once
#include <string>
#include <WinSock2.h>

namespace engine::client {

void AddServerToList(const std::string &server_name, unsigned long ip_address);
float GetFrameRate();
bool IsServerSelectionOpen();
char *GetRTTString();

void SetFrameRate(float frame_rate);
//void AddServerToList(const std::string &server_name, const std::string &ip_address)
//{
//    unsigned long ip = inet_addr(ip_address.c_str());
//    ip = ntohl(ip);
//    AddServerToList(server_name, ip);
//};
}// namespace engine::client