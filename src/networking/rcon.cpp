#include "rcon.hpp"
#include "../console.hpp"
#include "../command.hpp"

namespace networking {
	
	void RCON::listen() {


		struct sockaddr_in client_addr;
        int addrlen = sizeof(client_addr);	
		
		console::log("RCON Listening");
		
		while (true) {

			int recv_bytes = recvfrom(this->sock, this->buffer, sizeof(this->buffer), 0, (struct sockaddr *)&client_addr, &addrlen);
            if (recv_bytes == SOCKET_ERROR) {
				Sleep(1000);
                continue;
            }

			if (recv_bytes > 10){
                const char *rcon_command = "\xFF\xFF\xFF\xFFRCON";
                console::log("%s", this->buffer);

                if (std::memcmp(this->buffer, rcon_command, strlen(rcon_command) != 0)) continue;
				if (std::memcmp(static_cast<char*>(this->buffer) + strlen(rcon_command), password.c_str(), password.length()) != 0) {
                    console::log("invalid rcon password");
                    continue;
				}


				

				auto command_bytes = static_cast<char *>(this->buffer) + strlen(rcon_command) + password.length();
                std::string cmd(command_bytes, 0, recv_bytes - strlen(rcon_command) - password.length());

				console::log("rcon command %s", cmd.c_str());
				command::process_command(cmd);
				sendto(this->sock, this->buffer, recv_bytes, 0, (struct sockaddr *)&client_addr, addrlen);

			}

		}
	}


}