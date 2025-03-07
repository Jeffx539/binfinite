#include "menu.hpp"
#include <string>
#include <vector>
#include "../command.hpp"
#include "../engine/server.hpp"
#include "../engine/client.hpp"
namespace ui::menu {


struct Servers 
{
    std::string region;
    std::string name;
    std::string ip_addr;
} ;


std::vector<Servers> s = { { .region = "Australia", .name = "Sydney Supermax",.ip_addr = "103.214.222.1" },
    { .region = "Australia", .name = "Linux Melbourne SuperMAX! 128 Tick",  .ip_addr = "139.84.198.114" } };



void Draw()
{
    static int tickrate = 60;
    static bool client_nwperf = true;
    ImGui::SetNextWindowSizeConstraints(ImVec2(1200, 600), ImVec2(FLT_MAX, FLT_MAX));
    ImGui::Begin("Binfinite Alpha", &draw, ImGuiWindowFlags_MenuBar);
    ImGui::SeparatorText("Settings");
    ImGui::SliderInt("Game Tickrate", &tickrate, 60, 120, NULL, ImGuiSliderFlags_NoInput | ImGuiSliderFlags_AlwaysClamp);
    ImGui::Checkbox("Client Network Stats", &client_nwperf);

    if (tickrate != engine::server::GetTickRate()) { command::process_command("tickrate " + std::to_string(tickrate)); }
    if (client_nwperf != engine::client::GetNWPerf()) { engine::client::SetNWPerf(client_nwperf); }

    ImGui::SeparatorText("Server Browser");

    if (ImGui::BeginTable("server browser",3, ImGuiTableFlags_Borders)) {
        // Submit columns name with TableSetupColumn() and call TableHeadersRow() to create a row with a header in each
        // column. (Later we will show how TableSetupColumn() has other uses, optional flags, sizing weight etc.)
        ImGui::TableSetupColumn("Region");
        ImGui::TableSetupColumn("Server Name");
        ImGui::TableSetupColumn("");

        ImGui::TableHeadersRow();

        unsigned int uid = 0;
        for (auto& row : s) {
             ImGui::PushID(uid++);
             ImGui::TableNextRow();
            
             if(ImGui::TableSetColumnIndex(0))
                ImGui::TextUnformatted(row.region.c_str());

             if (ImGui::TableSetColumnIndex(1))
                 ImGui::TextUnformatted(row.name.c_str());

             if (ImGui::TableSetColumnIndex(2)) {
                 if (ImGui::SmallButton("Connect")) { command::process_command("connect " + row.ip_addr); }
             }

            ImGui::PopID();
        }



        ImGui::EndTable();
    }







    ImGui::End();
}


}// namespace ui::menu