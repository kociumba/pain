#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS

#include "module_registry.h"
// clang-format off
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "utils.h"
// clang-format on
#include "theme.h"
#include <boost/scope/defer.hpp>
#include <fmt/format.h>
#include <glm/glm.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <spdlog/spdlog.h>
#include <stdexcept>

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
};