#pragma once
#include <string>
#include "WinSock2.h"

namespace networking {

	class RCON
	{

		  public:
            RCON(const std::string password) : password(password), buffer(){
                WSAData data;
                WSAStartup(MAKEWORD(2, 2), &data);
				this->sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

                struct sockaddr_in addr;
				addr.sin_family = AF_INET;
                addr.sin_port = htons(13433);
                addr.sin_addr.s_addr = INADDR_ANY;
				if (bind(this->sock, (struct sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR) { throw("cant attach RCON on port 13433");}



			};
			RCON() = delete;

			void listen();

			private:
				std::string password;
                char buffer[1024];
				SOCKET sock;

				
	};




}