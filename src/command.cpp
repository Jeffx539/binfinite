#include "command.hpp"
#include <algorithm>
#include <string>

namespace command {


   void register_cmd(const std::string &name, std::function<void(const std::vector<std::string> &)> callback)
   {
        commands_[name] = std::make_shared<ConsoleCommand>(ConsoleCommand{ .name = name, .callback = callback });
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
   
    
    void process_command(const std::string command) {

        std::vector<std::string> arguments;
        //bool in_arg = false;
        bool inquote = false;
        unsigned int arg_start = 0;

        for (unsigned int i = 0; i < command.length(); i++) {

            if (command[i] == '"') { 
                inquote = !inquote;
                continue;
            }

            if (command[i] == ' ') {

                if (inquote) {
                    continue;
                }


                auto str = command.substr(arg_start, i - arg_start);
                str.erase(std::remove(str.begin(), str.end(), '"'), str.end());
                str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
                arguments.push_back(str);
                arg_start = i + 1;

            }
        }

        auto str = command.substr(arg_start, command.length());
        str.erase(std::remove(str.begin(), str.end(), '"'), str.end());
        str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
        arguments.push_back(str);


        auto cmd = commands_.find(arguments[0]);
        if (cmd == commands_.end()) { return; }
        arguments.erase(arguments.begin());
        cmd->second->callback(arguments);

    }


 




}

