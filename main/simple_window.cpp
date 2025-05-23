#pragma once

#include "graphics.h"
#include "main.h"
#include "module_registry.h"
#include "theme.h"

void simple_window_module(Registry &reg, State &ctx) {
    reg.add_ui_panel([&reg, &ctx]() {
        ig::Begin("gabagool");
        ig::Text("gabagoolx2");
        if (ig::Button("Close Window")) {
            glfwSetWindowShouldClose(ctx.w, GLFW_TRUE);
        }
        if (ig::Button("Reload theme")) {
            k_theme(ImGui::GetIO());
        }
        // if (ig::Button("Reload modules")) {
        //     INIT_ALL_MODULES(reg, ctx);
        // }
        ig::ColorEdit3("Clear Color", (float *)&ctx.clear_color);
        ig::End();
    });
}
REGISTER_MODULE(simple_window_module);