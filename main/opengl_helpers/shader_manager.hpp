#pragma once
#include "shader.hpp"
#include <memory>
#include <string>
#include <unordered_map>

class ShaderManager {
  private:
    std::unordered_map<std::string, std::shared_ptr<GLShader>> shaders;

  public:
    static ShaderManager &get() {
        static ShaderManager instance;
        return instance;
    }

    std::shared_ptr<GLShader> getShader(const std::string &name, GLenum type,
                                        const std::string &source) {
        auto it = shaders.find(name);
        if (it != shaders.end()) {
            return it->second;
        }

        auto shader = std::make_shared<GLShader>(type, source);
        shaders[name] = shader;
        return shader;
    }

  private:
    ShaderManager() = default;
};