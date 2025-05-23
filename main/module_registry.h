#pragma once

#include "main.h"
#include <functional>
#include <spdlog/spdlog.h>
#include <vector>

namespace l = spdlog;

struct Registry;
struct State;

struct ModuleRegistry {
    using InitFn = std::function<void(Registry &, State &)>;

    static ModuleRegistry &get() {
        static ModuleRegistry instance;
        return instance;
    }

    void registerInit(InitFn fn) { inits.emplace_back(std::move(fn)); }

    void initAll(Registry &reg, State &ctx) {
        for (auto &fn : inits)
            try {
                fn(reg, ctx);
            } catch (const std::exception &e) {
                l::error("failed to initialize module: {}", e.what());
            }
    }

  private:
    std::vector<InitFn> inits;
};

#define REGISTER_MODULE(fn)                                                                        \
    namespace {                                                                                    \
    struct Registrar_##fn {                                                                        \
        Registrar_##fn() { ModuleRegistry::get().registerInit(fn); }                               \
    } registrar_##fn;                                                                              \
    }

#define INIT_ALL_MODULES(reg, ctx) ModuleRegistry::get().initAll((reg), (ctx))