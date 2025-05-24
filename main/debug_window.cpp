#pragma once

#include "graphics.h"
#include "main.h"
#include "module_registry.h"
#include "theme.h"

void debug_window_module(Registry &reg, State &ctx) {
    reg.add_ui_panel([&reg, &ctx]() {
        ig::Begin("debug");
        if (ig::Button("Close Window")) {
            glfwSetWindowShouldClose(ctx.w, GLFW_TRUE);
        }
        ig::Spacing();
        if (ig::Button("Reload theme")) {
            k_theme(ImGui::GetIO());
        }
        ig::Spacing();
        if (ig::Button("Reload modules")) {
            ctx.queue_reload = true;
        }
        ig::Spacing();
        if (ig::Button("display debug")) {
            ctx.display_debug = !ctx.display_debug;
        }
        ig::Spacing();
        ig::ColorEdit3("Clear Color", (float *)&ctx.clear_color);
        ig::End();
    });
}
REGISTER_MODULE(debug_window_module);