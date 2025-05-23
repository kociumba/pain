#pragma once

// clang-format off
#include "stb/stb_image.h"
#include "main.h"
// clang-format on
#include "opengl_helpers/program.hpp"
#include "opengl_helpers/shader_manager.hpp"
#include <glm/glm.hpp>
#include <memory>

struct Vertex {
    glm::vec3 pos;
    glm::vec2 uv;
};

const std::vector<Vertex> quadVerts = {
    {{-1, -1, 0}, {0, 0}}, {{1, -1, 0}, {1, 0}}, {{1, 1, 0}, {1, 1}},
    {{-1, -1, 0}, {0, 0}}, {{1, 1, 0}, {1, 1}},  {{-1, 1, 0}, {0, 1}},
};

class BackgroundRenderer {
  private:
    static constexpr const char *VERTEX_SHADER_SOURCE = R"(
#version 450 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;

out vec2 vUV;

void main() {
    vUV = uv;
    gl_Position = vec4(position, 1.0);
}
)";

    static constexpr const char *FRAGMENT_SHADER_SOURCE = R"(
#version 450 core
in vec2 vUV;
layout(location = 0) out vec4 outColor;

uniform sampler2D uTexture;

void main() {
    outColor = texture(uTexture, vUV);
}
)";

    std::unique_ptr<GLProgram> program;
    GLuint vao;
    GLuint vbo;
    GLuint tex;

  public:
    BackgroundRenderer() {
        auto vs =
            ShaderManager::get().getShader("quad_vertex", GL_VERTEX_SHADER, VERTEX_SHADER_SOURCE);
        auto fs = ShaderManager::get().getShader("quad_fragment", GL_FRAGMENT_SHADER,
                                                 FRAGMENT_SHADER_SOURCE);
        program = std::make_unique<GLProgram>(*vs, *fs);

        glCreateBuffers(1, &vbo);
        glNamedBufferStorage(vbo, quadVerts.size() * sizeof(Vertex), quadVerts.data(), 0);

        // --- VAO setup ---
        glCreateVertexArrays(1, &vao);
        glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(Vertex));
        // pos @ loc 0
        glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, pos));
        glEnableVertexArrayAttrib(vao, 0);
        glVertexArrayAttribBinding(vao, 0, 0);
        // uv @ loc 1
        glVertexArrayAttribFormat(vao, 1, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, uv));
        glEnableVertexArrayAttrib(vao, 1);
        glVertexArrayAttribBinding(vao, 1, 0);

        // --- load texture ---
        stbi_set_flip_vertically_on_load(true);
        int w, h, ch;
        auto *data = stbi_load("assets/background.png", &w, &h, &ch, 4);
        if (!data)
            throw std::runtime_error("Failed to load texture");

        glCreateTextures(GL_TEXTURE_2D, 1, &tex);
        glTextureStorage2D(tex, 1, GL_RGBA8, w, h);
        glTextureSubImage2D(tex, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateTextureMipmap(tex);

        glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(tex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(tex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        stbi_image_free(data);

        program->use();
        GLint loc = glGetUniformLocation(program->get(), "uTexture");
        if (loc < 0) {
            throw std::runtime_error("uTexture uniform not found");
        }
        glUniform1i(loc, 0);
    }

    ~BackgroundRenderer() {
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
    }

    void render() const {
        program->use();
        glBindTextureUnit(0, tex);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    BackgroundRenderer(const BackgroundRenderer &) = delete;
    BackgroundRenderer &operator=(const BackgroundRenderer &) = delete;
    BackgroundRenderer(BackgroundRenderer &&) = delete;
    BackgroundRenderer &operator=(BackgroundRenderer &&) = delete;
};

void background_module(Registry &reg, State &ctx) {
    auto renderer = std::make_shared<BackgroundRenderer>();
    reg.add_render_pass([renderer]() { renderer->render(); });
}
REGISTER_MODULE(background_module);