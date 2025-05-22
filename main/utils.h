#pragma once

#include "main.h"
#include <stdexcept>

/**
 * \brief Creates a new shader program and links a vertex and fragment shader to it.
 * \param vertexModule The vertex shader to link to.
 * \param fragmentModule The fragment shader to link to.
 * \return Returns a shader program with vertex and fragment shader modules linked. The modules may
 * now be destroyed.
 */
inline GLuint linkModules(const GLuint vertexModule, const GLuint fragmentModule) {
    // Creates a new shader program.
    const GLuint program = glCreateProgram();

    // These calls attach shader modules to a shader program which will be used when linking.
    glAttachShader(program, vertexModule);
    glAttachShader(program, fragmentModule);

    // Links the shader modules together to create a shader program.
    glLinkProgram(program);

    // Same as the code in createShaderModule but checks for linking errors this time.
    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLint size;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &size);
        char *log = new char[size];
        glGetProgramInfoLog(program, size, nullptr, log);
        throw std::runtime_error("Failed to link GL shader program.\n" + std::string(log));
    }

    return program;
}

/**
 * \brief Creates and compiles a shader module.
 * \param type The type of shader to create, e.g. GL_VERTEX_SHADER or GL_FRAGMENT_SHADER.
 * \param source The source code for the shader.
 * \return Returns a newly created shader module which can be used to link a program.
 */
inline GLuint createShaderModule(const GLenum type, const std::string &source) {
    // Creates a new shader module.
    const GLuint module = glCreateShader(type);

    const auto &src = source.c_str();
    // This call sets the source code for a shader module.
    glShaderSource(module, 1, &src, nullptr);
    // This call actually compiles the shader module.
    glCompileShader(module);

    // This code checks for compilation errors. If there were errors, runtime error is thrown with
    // the error message.
    GLint status;
    glGetShaderiv(module, GL_COMPILE_STATUS, &status);
    if (!status) {
        GLint size;
        glGetShaderiv(module, GL_INFO_LOG_LENGTH, &size);
        char *log = new char[size];
        glGetShaderInfoLog(module, size, nullptr, log);
        throw std::runtime_error("Failed to compile GL shader module.\n" + std::string(log));
    }

    return module;
}
