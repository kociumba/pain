#pragma once
#include "../main.h"
#include <GL/glew.h>
#include <memory>
#include <string>

class GLShader {
  private:
    GLuint id;

  public:
    GLShader(GLenum type, const std::string &source) : id(createShaderModule(type, source)) {}

    ~GLShader() {
        if (id != 0) {
            glDeleteShader(id);
        }
    }

    GLuint get() const { return id; }

    // Allow moving
    GLShader(GLShader &&other) noexcept : id(other.id) { other.id = 0; }
    GLShader &operator=(GLShader &&other) noexcept {
        if (this != &other) {
            glDeleteShader(id);
            id = other.id;
            other.id = 0;
        }
        return *this;
    }

    // Prevent copying
    GLShader(const GLShader &) = delete;
    GLShader &operator=(const GLShader &) = delete;
};