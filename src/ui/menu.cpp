#include "menu.hpp"
#include <string>
#include <vector>
#include "../command.hpp"
#include "../console.hpp"
#include "../engine/server.hpp"
#include "../engine/client.hpp"
#include "../engine/shared.hpp"
namespace ui::menu {


struct Servers 
{
    std::string region;
    std::string name;
    std::string ip_addr;
} ;




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

    if (ImGui::BeginTable("server browser",
          5,
          ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterV
            | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersOuterH
            | ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_Resizable)) {
        // Submit columns name with TableSetupColumn() and call TableHeadersRow() to create a row with a header in each
        // column. (Later we will show how TableSetupColumn() has other uses, optional flags, sizing weight etc.)
        ImGui::TableSetupColumn("Server Name", ImGuiTableColumnFlags_WidthStretch, 0.4f);
        ImGui::TableSetupColumn("Slots", ImGuiTableColumnFlags_WidthStretch, 0.1f);
        ImGui::TableSetupColumn("Game", ImGuiTableColumnFlags_WidthStretch, 0.2f);
        ImGui::TableSetupColumn("Ping", ImGuiTableColumnFlags_WidthStretch, 0.1f);
        ImGui::TableSetupColumn("",ImGuiTableColumnFlags_WidthStretch, 0.2f);

        ImGui::TableHeadersRow();

        unsigned int uid = 0;
        for (auto& row : engine::shared::networking::servers_) {
            if (!row.second.query_complete) continue;

             ImGui::PushID(uid++);
             ImGui::TableNextRow();

             if (ImGui::TableSetColumnIndex(0)) ImGui::TextUnformatted(row.second.props["hostname"].c_str());
             if (ImGui::TableSetColumnIndex(1)) ImGui::TextUnformatted("0/32");
             if (ImGui::TableSetColumnIndex(2)) ImGui::TextUnformatted("aquarius/slayer ctf");
             if (ImGui::TableSetColumnIndex(3)) ImGui::Text("%d ms", row.second.ping);
             if (ImGui::TableSetColumnIndex(4)) {
                 if (ImGui::SmallButton("Connect")) { command::process_command("connect " + row.second.addr.ip); }
             }

            ImGui::PopID();
        }

    

        ImGui::EndTable();
    }
    if (ImGui::SmallButton("Refresh Server List")) {
        engine::shared::GameSocketSend("binfinitemaster.lh2.au", 7676, "\xff\xff\xff\xffgetservers INFINITE");
    }






    ImGui::End();
}


}// namespace ui::menu