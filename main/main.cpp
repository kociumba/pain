#include "main.h"

namespace l = spdlog;
namespace ig = ImGui;

GLFWwindow* initGLFW() {
	if (glfwInit() != GLFW_TRUE) throw std::runtime_error("failed to initialize glfw");
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_COMPAT_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
	glfwWindowHint(GLFW_REFRESH_RATE, 144);
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

void initGlew() {
	glewExperimental = GL_TRUE;
	if (const auto& err = glewInit(); err != GLEW_OK) {
		l::critical("glew failed to load opengl and initialize");
		throw std::runtime_error("glew failed to load opengl and initialize");
	}
}

ImGuiIO& initImGui(GLFWwindow* w) {
	IMGUI_CHECKVERSION();
	ig::CreateContext();
	auto& io = ig::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	k_theme(io);

	const char* glsl_version = "#version 450";
	ImGui_ImplGlfw_InitForOpenGL(w, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	l::info("✓ imgui initialized, with version: {}", ig::GetVersion());

	return io;
}

int main() {
	auto w = initGLFW();
	BOOST_SCOPE_DEFER[&w]{
		glfwDestroyWindow(w);
		glfwTerminate();
		l::info("glfw cleaned up");
	};

	initGlew();
	//glEnable(GL_FRAMEBUFFER_SRGB);
	l::info("✓ glew initialized, with version: {}", (const char*)glewGetString(GLEW_VERSION));

	l::info("OpenGL Version: {}", (const char*)glGetString(GL_VERSION));
	l::info("OpenGL Vendor: {}", (const char*)glGetString(GL_VENDOR));
	l::info("OpenGL Renderer: {}", (const char*)glGetString(GL_RENDERER));
	l::info("✓ OpenGL loaded");

	auto io = initImGui(w);
	BOOST_SCOPE_DEFER[]{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ig::DestroyContext();
		l::info("imgui cleaned up");
	};

	State ctx{ .w = w, .io = io, .clear_color = ImVec4(0.01f, 0.01f, 0.01f, 1.0f) };
	Registry registry;

	INIT_ALL_MODULES(registry, ctx);
	BOOST_SCOPE_DEFER[&registry]{
		for (auto& fn : registry.cleanups) fn();
		l::info("all modules cleaned up");
	};

	while (!glfwWindowShouldClose(w)) {
		glfwPollEvents();

		if (glfwGetKey(w, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(w, GLFW_TRUE);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ig::NewFrame();
		for (auto& panel : registry.ui_panels) panel();
		ig::Render();

		int display_w, display_h;
		glfwGetFramebufferSize(w, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(
			ctx.clear_color.x * ctx.clear_color.w,
			ctx.clear_color.y * ctx.clear_color.w,
			ctx.clear_color.z * ctx.clear_color.w,
			ctx.clear_color.w
		);
		glClear(GL_COLOR_BUFFER_BIT);
		for (auto& pass : registry.render_passes) pass();

		ImGui_ImplOpenGL3_RenderDrawData(ig::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			auto backup_current_context = glfwGetCurrentContext();
			ig::UpdatePlatformWindows();
			ig::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}

		glfwSwapBuffers(w);

	}
	return 0;
}