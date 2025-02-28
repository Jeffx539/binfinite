#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <functional>


namespace command {


	struct ConsoleCommand {
            const std::string name;
            std::function<void(const std::vector<std::string>&)>  callback;

	};
    
	inline std::unordered_map<std::string, std::shared_ptr<ConsoleCommand>> commands_;

	void register_cmd(const std::string &name, std::function<void(const std::vector<std::string> &)> callback);
	void process_command(const std::string command);





}