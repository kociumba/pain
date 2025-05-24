#pragma once

#include "config_manager.h"
#include "graphics.h"
#include "konfig/konfig.h"
#include "main.h"
#include "module_registry.h"
#include "theme.h"
#include <spdlog/spdlog.h>

namespace l = spdlog;

struct test_config {
    int x = 10;
    std::string y = "gabagool";
};

#define TEST_FIELDS(X)                                                                             \
    X(x, "x")                                                                                      \
    X(y, "y")

MAKE_SECTION(test_config, TEST_FIELDS);

void debug_window_module(Registry &reg, State &ctx) {
    auto cfg = mngr->addSection<test_config>("test");

    reg.add_ui_panel([&reg, &ctx, cfg]() {
        ig::Begin("debug##Main", NULL, ImGuiWindowFlags_AlwaysAutoResize);

        if (ig::BeginTabBar("debug")) {
            if (ig::BeginTabItem("Debugging")) {
                if (ig::Button("Reload modules")) {
                    ctx.queue_reload = true;
                }
                ig::SameLine();
                ig::Checkbox("Display Debug Info", &ctx.display_debug);
                ig::EndTabItem();
            }

            if (ig::BeginTabItem("Config")) {
                ig::Text("Test Configuration:");

                if (ig::InputInt("Value X", &cfg->data.x)) {
                }

                char y_buffer[256];
                strncpy_s(y_buffer, cfg->data.y.c_str(), sizeof(y_buffer) - 1);
                y_buffer[sizeof(y_buffer) - 1] = '\0';

                if (ig::InputText("Value Y", y_buffer, sizeof(y_buffer))) {
                    cfg->data.y = std::string(y_buffer);
                }

                ig::Separator();

                if (ig::Button("Save Config")) {
                    mngr->save();
                    l::info("Config saved. X: {}, Y: '{}'", cfg->data.x, cfg->data.y);
                }
                ig::SameLine();
                if (ig::Button("Load Config")) {
                    mngr->load();
                    l::info("Config loaded. X: {}, Y: '{}'", cfg->data.x, cfg->data.y);
                }

                ig::TextWrapped(
                    "Note: 'Load Config' will overwrite any unsaved changes made here.");

                ig::EndTabItem();
            }

            if (ig::BeginTabItem("Window & Theme")) {
                if (ig::Button("Close Window")) {
                    glfwSetWindowShouldClose(ctx.w, GLFW_TRUE);
                }
                ig::SameLine();
                if (ig::Button("Reload theme")) {
                    k_theme(ImGui::GetIO());
                }
                ig::ColorEdit3("Clear Color", (float *)&ctx.clear_color);
                ig::EndTabItem();
            }

            ig::EndTabBar();
        }

        ig::End();
    });
}
REGISTER_MODULE(debug_window_module);