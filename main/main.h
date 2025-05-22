#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS

#include <boost/scope/defer.hpp>
#include <spdlog/spdlog.h>
#include <fmt/format.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <stdexcept>
#include "theme.h"
#include "utils.h"
#include "module_registry.h"

using std::vector;

struct Registry {
	using RenderPass = std::function<void()>;
	using UIPanel = std::function<void()>;
	using CleanupFn = std::function<void()>;

	vector<RenderPass> render_passes;
	vector<UIPanel> ui_panels;
	vector<CleanupFn> cleanups;

	void add_render_pass(RenderPass cb) { render_passes.emplace_back(std::move(cb)); }
	void add_ui_panel(UIPanel    cb) { ui_panels.emplace_back(std::move(cb)); }
	void add_cleanup(CleanupFn  cb) { cleanups.emplace_back(std::move(cb)); }
};

struct State {
	GLFWwindow* w;
	ImGuiIO& io;
	ImVec4 clear_color;
};