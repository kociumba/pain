#pragma once

#include "graphics.h"
#include <fmt/format.h>
#include <functional>
#include <stdexcept>
#include <vector>

using std::vector;

struct Registry {
    using RenderPass = std::function<void()>;
    using UIPanel = std::function<void()>;
    using CleanupFn = std::function<void()>;

    vector<RenderPass> render_passes;
    vector<UIPanel> ui_panels;
    vector<CleanupFn> cleanups;

    void add_render_pass(RenderPass cb) { render_passes.emplace_back(std::move(cb)); }
    void add_ui_panel(UIPanel cb) { ui_panels.emplace_back(std::move(cb)); }
    void add_cleanup(CleanupFn cb) { cleanups.emplace_back(std::move(cb)); }
};

struct State {
    GLFWwindow *w;
    ImVec4 clear_color;
    Registry registry;
};