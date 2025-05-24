#pragma once

#include "graphics.h"
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <functional>
#include <map>
#include <memory>
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

enum Fullscreen {
    no,
    fullscreen,
    borderless,
};

struct State {
    GLFWwindow *w;
    ImVec4 clear_color;
    Registry registry;
    Fullscreen fullscreen;
    std::unordered_map<int, bool> key_map;
    std::unordered_map<int, bool> prev_key_map;
    struct { // used for saving size and position
        int x, y, w, h;
    } saved;
    bool display_debug = false;
    bool queue_reload = false;
};

constexpr std::string fullscreen_to_string(Fullscreen f);
std::string state_to_string(const State &s);