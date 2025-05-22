#pragma once

#include "main.h"

void overlay_module(Registry &reg, State &ctx) {
    reg.add_ui_panel([&ctx]() {
        const ImGuiViewport *viewport = ig::GetMainViewport();
        ig::SetNextWindowPos(viewport->Pos);
        ig::SetNextWindowSize(viewport->WorkSize);
        ig::SetNextWindowViewport(viewport->ID);
        auto flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings |
                     ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
                     ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMouseInputs;
        ig::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ig::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ig::Begin("main", NULL, flags);
        ig::PopStyleVar(2);
        ig::Text("%.1f FPS", ImGui::GetIO().Framerate);
        ig::End();
    });
}
REGISTER_MODULE(overlay_module);