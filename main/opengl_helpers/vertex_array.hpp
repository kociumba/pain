#pragma once
#include "buffer.hpp"

class GLVertexArray {
  private:
    GLuint id;

  public:
    GLVertexArray() { glCreateVertexArrays(1, &id); }

    template <typename T>
    void setVertexBuffer(const GLBuffer<T> &buffer, GLuint binding = 0, GLsizei stride = sizeof(T),
                         GLuint offset = 0) {
        glVertexArrayVertexBuffer(id, binding, buffer.get(), offset, stride);
    }

    void setAttribFormat(GLuint attribIndex, GLint size, GLenum type,
                         GLboolean normalized = GL_FALSE, GLuint relativeOffset = 0) {
        glVertexArrayAttribFormat(id, attribIndex, size, type, normalized, relativeOffset);
        glEnableVertexArrayAttrib(id, attribIndex);
        glVertexArrayAttribBinding(id, attribIndex, attribIndex);
    }

    void bind() const { glBindVertexArray(id); }

    ~GLVertexArray() { glDeleteVertexArrays(1, &id); }

    GLuint get() const { return id; }

    // Prevent copying, allow moving
    GLVertexArray(const GLVertexArray &) = delete;
    GLVertexArray &operator=(const GLVertexArray &) = delete;
    GLVertexArray(GLVertexArray &&other) noexcept : id(other.id) { other.id = 0; }
    GLVertexArray &operator=(GLVertexArray &&other) noexcept {
        if (this != &other) {
            glDeleteVertexArrays(1, &id);
            id = other.id;
            other.id = 0;
        }
        return *this;
    }
};