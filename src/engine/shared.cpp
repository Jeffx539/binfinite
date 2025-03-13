#include "shared.hpp"

#include "../console.hpp"
#include "../utils/memory.hpp"
#include "../utils/hooks.hpp"
#include "../engine/server.hpp"
#include "../engine/networking.hpp"

#include "../command.hpp"
#include "../environment.hpp"



namespace engine::shared {

// socket is a dual stack implementation
void GameSocketSend(const std::string &ip, short port, std::string buffer)
{

    struct addrinfo hints, *addr_info, *ptr;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_V4MAPPED | AI_ALL;

    int result = getaddrinfo(ip.c_str(), std::to_string(port).c_str(), &hints, &addr_info);
    if (result != 0) {
        console::log("sock send getaddrinfo failed: %s", gai_strerror(result));
        return;
    }


    if (addr_info == NULL) { return; }

    auto socket = reinterpret_cast<SOCKET **>(reinterpret_cast<uint8_t *>(GAME_PTR(0x4f04880)));
    int retcode = sendto(**socket, buffer.data(), buffer.length(), 0, addr_info->ai_addr, (int)addr_info->ai_addrlen);
    if (retcode == SOCKET_ERROR) { console::log("sendto failed with error: %d\n", WSAGetLastError()); }




}


    namespace networking {





        utils::hooks::detour networking_recvfrom;
        struct OOBCommand
        {
            const std::string name;
            std::function<void(NetworkAddress &addr, const std::string &)> callback;
        };

        inline std::vector<std::shared_ptr<OOBCommand>> commands_;
        void OOBOn(const std::string &name, std::function<void(NetworkAddress &addr, const std::string &)> callback)
        {
            commands_.push_back(std::make_shared<OOBCommand>(OOBCommand{ .name = name, .callback = callback }));
        }


    

        void ProcessOOBCommand(NetworkAddress &addr, const std::string &command)
        {
           
            for (auto& cmd_map : commands_) { 
                if (command.length() < cmd_map->name.length()) {
                    continue;
                }
                if (std::memcmp(cmd_map->name.data(), command.data(), cmd_map->name.length()) != 0) {
                    continue;
                }

                cmd_map->callback(addr,
                  std::string(command, cmd_map->name.length()));



                return;


            }

        
        }

        // todo optimise
        auto game_socket = reinterpret_cast<SOCKET **>(GAME_PTR(0x4f04880)); // mar13
        int Hook_RecvFrom(SOCKET s, PSTR buf, int32_t len, int32_t flags, SOCKADDR *from, int32_t *fromlen)
        {            
            if (s != **game_socket) {
                return networking_recvfrom.invoke<int>(s, buf, len, flags, from, fromlen);
            }

            int result = networking_recvfrom.invoke<int>(s, buf, len, flags, from, fromlen);
            if (result > 4) {
               if (std::memcmp(buf, "\xff\xff\xff\xff", 4) == 0) {
                  char str_addr[INET6_ADDRSTRLEN];
                   auto in_addr = reinterpret_cast<sockaddr_in6 *>(from);
                  inet_ntop(AF_INET6, &in_addr->sin6_addr, str_addr, INET6_ADDRSTRLEN);
                  
                   NetworkAddress addr{
                      .ip = std::string(str_addr),
                      .port = ntohs(in_addr->sin6_port)
                   };
               
                   console::log("Processing OOB Command from %s %d", addr.ip.c_str(), addr.port);
                   ProcessOOBCommand(addr, std::string(buf+4, result - 4));
               }
            }

            if (result == SOCKET_ERROR) {
                int error = WSAGetLastError();
                if (error != WSAEWOULDBLOCK)
                    console::log("wsa error on game recv socket %d", WSAGetLastError());
            }

            return result;


        }

        std::vector<std::string> split(const std::string &s, char seperator)
        {
            std::vector<std::string> output;

            std::string::size_type prev_pos = 0, pos = 0;

            while ((pos = s.find(seperator, pos)) != std::string::npos) {
                std::string substring(s.substr(prev_pos, pos - prev_pos));
                output.push_back(substring);
                prev_pos = ++pos;
            }
            output.push_back(s.substr(prev_pos, pos - prev_pos));// Last word

            return output;
        }


        void InstallHooks() { 

            HMODULE ws2 = LoadLibrary("ws2_32.dll");
            networking_recvfrom.create(GetProcAddress(ws2, "recvfrom"), &Hook_RecvFrom);
            // space is intentional
            OOBOn("rcon ", [](NetworkAddress &addr, const std::string data) { 
            
                if (!environment::IsServer()) { return; }
                if (engine::server::g_serverConfig.rcon_password == "") { return; }
                if (data.length() < engine::server::g_serverConfig.rcon_password.length()) { return; }
                if (std::memcmp(static_cast<const char *>(data.data()),
                      engine::server::g_serverConfig.rcon_password.c_str(),
                      engine::server::g_serverConfig.rcon_password.length())
                    != 0) {
                    return;
                }

                auto command_bytes =
                  static_cast<const char *>(data.data()) + engine::server::g_serverConfig.rcon_password.length();


                std::string cmd(
                  command_bytes, 0, data.length() - engine::server::g_serverConfig.rcon_password.length()-1);

                console::log("processing rcon command: %s", cmd.c_str());
                command::process_command(cmd);
                engine::shared::GameSocketSend(addr.ip, addr.port, cmd);




            });


            OOBOn("getInfo", [](NetworkAddress &addr,  const std::string data) { 
                
                std::string info_response = "\xFF\xFF\xFF\xFFinfoResponse ";
                info_response += "\\game\\binfinite";
                info_response += "\\challenge\\fixme";
                info_response += "\\hostname\\"+server::g_serverConfig.server_name;
                info_response += "\\clients\\" +std::to_string(engine::networking::SessionMembership::GetInstance()->PeerCount);
                info_response += "\\sv_maxclients\\" + std::to_string(32);
                info_response += "\\mapname\\mapvarianthere";
                info_response += "\\playmode\\gamevairanthere";
                engine::shared::GameSocketSend(addr.ip, addr.port, info_response + "\n");
                
                
            });




            OOBOn("infoResponse", [](NetworkAddress &addr, const std::string data) {
                if (!servers_.contains(addr.ip + std::to_string(addr.port))) {
                    console::log("server not in map");
                    return;
                }


                auto &server = servers_[addr.ip + std::to_string(addr.port)];
                auto query_end = std::chrono::high_resolution_clock::now();

                auto duration =
                  std::chrono::duration_cast<std::chrono::milliseconds>(query_end - server.query_start_time);
                if (server.query_complete) { return; }



                
                auto spl = split(std::string(data.data() + 2), '\\');

                for (size_t i = 0; !spl.empty() && i < (spl.size() - 1); i += 2) {
                    const auto &key = spl[i];
                    const auto &value = spl[i + 1];

                    server.props[key] = value;
                }
                console::log("ping %s duration %d ms ", server.props["hostname"].c_str(), duration.count());


                server.ping = duration;
                server.query_complete = true;
                
            });


            OOBOn("getserversResponse", [](NetworkAddress &addr, const std::string data) {

                console::log("Master Server Response");

              std::string::size_type pos = 0;
                while ((pos = data.find('\\', pos)) != std::string::npos) {
                    pos++;

                    if (std::memcmp("EOT", data.data() + pos, 3) == 0) {
                        break;
                    }

                    NetworkAddress server;
                    char str_addr[INET_ADDRSTRLEN];
                    std::memset(str_addr, 0, INET_ADDRSTRLEN);
                    inet_ntop(AF_INET, data.data() + pos, str_addr, INET_ADDRSTRLEN);
                    std::memcpy(&server.port, data.data() + pos + 4 , 2);
                    server.port = ntohs(server.port);
                    server.ip = std::string(str_addr);


                    ServerQuery query{
                        .addr = server,
                        .query_complete = false,
                        .query_start_time = std::chrono::high_resolution_clock::now(),
                    };
                    servers_["::ffff:" + server.ip + std::to_string(server.port)] = query;





                    engine::shared::GameSocketSend(server.ip, server.port, "\xFF\xFF\xFF\xFFgetInfo");

                    
                }
        
                
            });




        }

    }

}// namespace engine::shared