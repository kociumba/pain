#pragma once
#include "shader.hpp"

class GLProgram {
  private:
    GLuint id;

  public:
    GLProgram(const GLShader &vertexShader, const GLShader &fragmentShader)
        : id(linkModules(vertexShader.get(), fragmentShader.get())) {}

    ~GLProgram() {
        if (id != 0) {
            glDeleteProgram(id);
        }
    }

    void use() const { glUseProgram(id); }
    void setUniform(GLuint location, GLuint value) const { glUniform1ui(location, value); }

    GLuint get() const { return id; }

    // Allow moving
    GLProgram(GLProgram &&other) noexcept : id(other.id) { other.id = 0; }
    GLProgram &operator=(GLProgram &&other) noexcept {
        if (this != &other) {
            glDeleteProgram(id);
            id = other.id;
            other.id = 0;
        }
        return *this;
    }

    // Prevent copying
    GLProgram(const GLProgram &) = delete;
    GLProgram &operator=(const GLProgram &) = delete;
};