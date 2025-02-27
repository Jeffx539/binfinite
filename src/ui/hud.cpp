#include "hud.hpp"
#include "imgui.h"
#include "version.h"
#include "../engine/server.hpp"
#include "../engine/client.hpp"
namespace ui::hud {



	void Draw() {
        ImGui::SetNextWindowPos(ImVec2(0, 0));// Set position of the window
        ImGui::SetNextWindowSize(ImVec2(0, 0));// Auto-resize the window
        ImGui::Begin("Top Left Text",
            nullptr,
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground  |ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
            | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_AlwaysAutoResize);

        ImGui::Text("Binfinite %s, tickrate: %d %s",
          GIT_DESCRIBE,
          engine::server::GetTickRate(), engine::client::GetRTTString());

        ImGui::End();
	}


}