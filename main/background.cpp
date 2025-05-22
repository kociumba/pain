#include "main.h"
#include <tuple>

void background_module(Registry& reg, State& ctx) {
	auto [prog, vao] = make_opengl_program();
	reg.add_render_pass([prog, vao]() {
		glUseProgram(prog);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		});
	reg.add_cleanup([prog, vao]() {
		glDeleteProgram(prog);
		glDeleteVertexArrays(1, &vao);
		});
}
REGISTER_MODULE(background_module);

std::tuple<GLuint, GLuint> make_opengl_program() {
	const GLuint vertexShader = createShaderModule(GL_VERTEX_SHADER, R"(
#version 450 core

layout(location = 0) in vec3 position;

void main() {
    gl_Position = vec4(position, 1.0);
}
)");
	const GLuint fragmentShader = createShaderModule(GL_FRAGMENT_SHADER, R"(
#version 450 core

layout(location = 0) out vec4 color;

void main() {
    color = vec4(1.0, 0.0, 0.0, 1.0);
}
)");

	const std::vector<glm::vec3> vertices{
		{-0.5f, -0.5f, 0.0f},
		{0.5f, -0.5f, 0.0f},
		{0.0f, 0.5f, 0.0f}
	};

	const GLuint shaderProgram = linkModules(vertexShader, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	GLuint vertexBuffer;
	glCreateBuffers(1, &vertexBuffer);

	glNamedBufferStorage(
		vertexBuffer,
		3 * sizeof(glm::vec3),
		vertices.data(),
		0
	);

	GLuint vertexArrayObject;
	glCreateVertexArrays(1, &vertexArrayObject);

	glVertexArrayVertexBuffer(
		vertexArrayObject,
		0,
		vertexBuffer,
		0,
		sizeof(glm::vec3)
	);

	glVertexArrayAttribFormat(
		vertexArrayObject,
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		0
	);

	glEnableVertexArrayAttrib(vertexArrayObject, 0);
	glVertexArrayAttribBinding(
		vertexArrayObject,
		0,
		0
	);

	glDeleteBuffers(1, &vertexBuffer);

	return { shaderProgram, vertexArrayObject };
}