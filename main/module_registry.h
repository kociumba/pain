#pragma once

#include "main.h"
#include <functional>
#include <spdlog/spdlog.h>
#include <vector>

namespace l = spdlog;

using InitFn = std::function<void(Registry &, State &)>;
inline std::vector<InitFn> &getGlobalInitList() {
    static std::vector<InitFn> g_inits;
    return g_inits;
}

struct ModuleRegistry {
    using InitFn = ::InitFn;

    static ModuleRegistry &get() {
        static ModuleRegistry instance;
        return instance;
    }

    void registerInit(InitFn fn) { inits.emplace_back(std::move(fn)); }

    void clear() { inits.clear(); }

    void initAll(Registry &reg, State &ctx) {
        for (auto &fn : inits) {
            try {
                fn(reg, ctx);
            } catch (const std::exception &e) {
                l::error("failed to initialize module: {}", e.what());
            }
        }
    }

  private:
    std::vector<InitFn> inits;
};

inline void populateRegistryWithAllModules(ModuleRegistry &mr) {
    for (auto &fn : getGlobalInitList())
        mr.registerInit(fn);
}

#define REGISTER_MODULE(fn)                                                                        \
    namespace {                                                                                    \
    struct Registrar_##fn {                                                                        \
        Registrar_##fn() { getGlobalInitList().push_back(fn); }                                    \
    } registrar_##fn;                                                                              \
    }

#define INIT_ALL_MODULES(registry, ctx)                                                            \
    do {                                                                                           \
        auto &mr = ModuleRegistry::get();                                                          \
        mr.clear();                                                                                \
        populateRegistryWithAllModules(mr);                                                        \
        mr.initAll(registry, ctx);                                                                 \
    } while (0)
