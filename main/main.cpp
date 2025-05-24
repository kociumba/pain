#pragma once

#include "main.h"
#include "config_manager.h"
#include "context.h"
#include "graphics.h"
#include "konfig/konfig.h"
#include "module_registry.h"
#include "theme.h"
#include "window_utils.h"
#include <boost/scope/defer.hpp>
#include <spdlog/spdlog.h>

namespace l = spdlog;
namespace ig = ImGui;

constexpr std::string fullscreen_to_string(Fullscreen f) {
    switch (f) {
    case no:
        return "no";
    case fullscreen:
        return "fullscreen";
    case borderless:
        return "borderless";
    };
    return "unknown";
};

// cursed abomination of manual indentation
std::string state_to_string(const State &s) {
    auto f = fullscreen_to_string(s.fullscreen);
    std::vector<std::string> kvs;
    kvs.reserve(s.key_map.size());
    for (auto const &[k, v] : s.key_map) {
        kvs.push_back(fmt::format("{}=>{}", k, v ? "on" : "off"));
    }
    return fmt::format("State {{\n \tfullscreen: {},\n \tkey_map: [\n\t\t{}\n\t],\n \tsaved: {{\n "
                       "\t\tx:{},\n \t\ty:{},\n \t\tw:{},\n \t\th:{}\n \t}}\n }}\n",
                       f, fmt::join(kvs, ",\n\t\t"), s.saved.x, s.saved.y, s.saved.w, s.saved.h);
};

GLFWwindow *initGLFW() {
    if (glfwInit() != GLFW_TRUE)
        throw std::runtime_error("failed to initialize glfw");
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_COMPAT_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    glfwWindowHint(GLFW_REFRESH_RATE, GLFW_DONT_CARE);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    auto w = glfwCreateWindow(800, 600, "gabagool", NULL, NULL);
    if (!w) {
        glfwTerminate();
        throw std::runtime_error("failed to create glfw window");
    }
    glfwMakeContextCurrent(w);
    glfwSwapInterval(1);
    l::info("✓ glfw initialized, with version: {}", glfwGetVersionString());

    return w;
}

void initGlad() {
    try {
        int version = gladLoadGL(glfwGetProcAddress);
        l::info("✓ GL loaded with glad: {}.{}", GLAD_VERSION_MAJOR(version),
                GLAD_VERSION_MINOR(version));
    } catch (const std::exception &e) {
        l::critical("Failed to load glad: {}", e.what());
        throw std::runtime_error("Failed to load glad: " + std::string(e.what()));
    }
}

ImGuiIO &initImGui(GLFWwindow *w) {
    IMGUI_CHECKVERSION();
    ig::CreateContext();
    auto &io = ig::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    k_theme(io);

    const char *glsl_version = "#version 450";
    ImGui_ImplGlfw_InitForOpenGL(w, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    l::info("✓ imgui initialized, with version: {}", ig::GetVersion());

    return io;
}

void render_frame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ig::NewFrame();
    for (auto &panel : ctx->registry.ui_panels)
        panel();
    ig::Render();

    int display_w, display_h;
    glfwGetFramebufferSize(ctx->w, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(ctx->clear_color.x * ctx->clear_color.w, ctx->clear_color.y * ctx->clear_color.w,
                 ctx->clear_color.z * ctx->clear_color.w, ctx->clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for (auto &pass : ctx->registry.render_passes)
        pass();

    ImGui_ImplOpenGL3_RenderDrawData(ig::GetDrawData());

    if (ig::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        auto backup_current_context = glfwGetCurrentContext();
        ig::UpdatePlatformWindows();
        ig::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }

    glfwSwapBuffers(ctx->w);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) { /* empty callback */ }
void window_refresh_callback(GLFWwindow *window) { render_frame(); }
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS)
        ctx->key_map[key] = true;
    else if (action == GLFW_RELEASE)
        ctx->key_map[key] = false;
}

bool is_key_pressed(int key) {
    bool current = ctx->key_map[key];
    bool previous = ctx->prev_key_map[key];
    return current && !previous;
}

int main() {
    auto w = initGLFW();
    BOOST_SCOPE_DEFER[&w] {
        glfwDestroyWindow(w);
        glfwTerminate();
        l::info("glfw cleaned up");
    };

    initGlad();

    l::debug("OpenGL Version: {}", (const char *)glGetString(GL_VERSION));
    l::debug("OpenGL Vendor: {}", (const char *)glGetString(GL_VENDOR));
    l::debug("OpenGL Renderer: {}", (const char *)glGetString(GL_RENDERER));

    auto io = initImGui(w);
    BOOST_SCOPE_DEFER[] {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ig::DestroyContext();
        l::info("imgui cleaned up");
    };

    ctx = std::make_shared<State>();
    ctx->w = w;
    ctx->clear_color = ImVec4(0.01f, 0.01f, 0.01f, 1.0f);

    mngr = std::make_shared<ConfigManager>("config.toml");

    INIT_ALL_MODULES(ctx->registry, *ctx);
    BOOST_SCOPE_DEFER[] {
        for (auto &fn : ctx->registry.cleanups)
            fn();
        l::info("all modules cleaned up");
    };

    glfwSetWindowUserPointer(w, &ctx);
    glfwSetFramebufferSizeCallback(w, framebuffer_size_callback);
    glfwSetWindowRefreshCallback(w, window_refresh_callback);
    glfwSetKeyCallback(w, key_callback);

    while (!glfwWindowShouldClose(w)) {
        glfwPollEvents();

        if (glfwGetKey(w, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(w, GLFW_TRUE);

        if (is_key_pressed(GLFW_KEY_F11)) {
            if (ctx->fullscreen == Fullscreen::no) {
                glfwGetWindowSize(w, &ctx->saved.w, &ctx->saved.h);
                glfwGetWindowPos(w, &ctx->saved.x, &ctx->saved.y);
                GLFWmonitor *mon = get_current_monitor(w);
                if (!mon) {
                    l::warn("failed to get current monitor");
                } else {
                    int xpos, ypos, width, height;
                    glfwGetMonitorWorkarea(mon, &xpos, &ypos, &width, &height);
                    glfwSetWindowAttrib(w, GLFW_DECORATED, GLFW_FALSE);
                    glfwSetWindowMonitor(w, nullptr, xpos, ypos, width, height, 0);
                    ctx->fullscreen = Fullscreen::borderless;
                }
            } else {
                glfwSetWindowAttrib(w, GLFW_DECORATED, GLFW_TRUE);
                glfwSetWindowMonitor(w, nullptr, ctx->saved.x, ctx->saved.y, ctx->saved.w,
                                     ctx->saved.h, 0);
                ctx->fullscreen = Fullscreen::no;
            }
        }

        render_frame();
        ctx->prev_key_map = ctx->key_map;

        // doesn't really work with hot reload but it can still rebuild shaders
        if (ctx->queue_reload) {
            for (auto &fn : ctx->registry.cleanups)
                fn();
            ctx->registry.ui_panels.clear();
            ctx->registry.render_passes.clear();
            ctx->registry.cleanups.clear();
            INIT_ALL_MODULES(ctx->registry, *ctx);
            ctx->queue_reload = false;
            l::info("all modules reloaded");
        }
    }
    return 0;
}