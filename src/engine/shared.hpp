#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <optional>
#include <vector>
#include <chrono>
#include <unordered_map>

namespace engine::shared {


	void GameSocketSend(const std::string &ip, short port, std::string buffer);


	namespace networking {

		   struct NetworkAddress
            {
                std::string ip;
                unsigned short port;
            };


            struct ServerQuery
            {

                NetworkAddress addr;
                bool query_complete;
                std::unordered_map<std::string, std::string> props;
                std::chrono::steady_clock::time_point query_start_time;
                std::chrono::milliseconds ping;
            };

            inline std::unordered_map<std::string, ServerQuery> servers_;

            void InstallHooks();

	}


}