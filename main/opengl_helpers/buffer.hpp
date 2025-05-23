#pragma once
#include "../main.h"
#include <GL/glew.h>
#include <span>
#include <vector>

template <typename T> class GLBuffer {
  private:
    GLuint id;

  public:
    GLBuffer() { glCreateBuffers(1, &id); }

    void store(std::span<const T> data, GLbitfield flags = 0) {
        glNamedBufferStorage(id, data.size_bytes(), data.data(), flags);
    }

    ~GLBuffer() { glDeleteBuffers(1, &id); }

    GLuint get() const { return id; }

    // Prevent copying, allow moving
    GLBuffer(const GLBuffer &) = delete;
    GLBuffer &operator=(const GLBuffer &) = delete;
    GLBuffer(GLBuffer &&other) noexcept : id(other.id) { other.id = 0; }
    GLBuffer &operator=(GLBuffer &&other) noexcept {
        if (this != &other) {
            glDeleteBuffers(1, &id);
            id = other.id;
            other.id = 0;
        }
        return *this;
    }
};