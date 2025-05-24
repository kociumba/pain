#pragma once
#ifndef KONFIG_HEADER
#define KONFIG_HEADER

#include <fstream>
#include <iostream>
#include <memory>
#include <spdlog/spdlog.h>
#include <string>
#include <toml++/toml.hpp>
#include <unordered_map>

namespace l = spdlog;

struct Section {
    virtual bool load(const toml::table &tbl) = 0;
    virtual bool save(toml::table &tbl) const = 0;
    virtual ~Section() = default;
}; // namespace l=spdlogstruct Section

template <typename T> struct ConfigSectionTraits {
    static void marshal(toml::table &tbl, const T &data);
    static void unmarshal(const toml::table &tbl, T &data);
};

template <typename T> struct ConfigSection : Section {
    T data;

    bool load(const toml::table &tbl) override {
        ConfigSectionTraits<T>::unmarshal(tbl, data);
        return true;
    }

    bool save(toml::table &tbl) const override {
        ConfigSectionTraits<T>::marshal(tbl, data);
        return true;
    }

    T &get() { return data; }
    const T &get() const { return data; }

    T *operator->() { return &data; }
    const T *operator->() const { return &data; }
    T &operator*() { return data; }
    const T &operator*() const { return data; }
};

class ConfigManager {
    std::unordered_map<std::string, std::shared_ptr<Section>> sections;
    std::string path;

  public:
    explicit ConfigManager(std::string filepath);

    template <typename T> std::shared_ptr<ConfigSection<T>> addSection(const std::string &name) {
        if (sections.contains(name)) {
            l::error("Config section '{}' already exists", name);
            return nullptr;
        }
        auto sec = std::make_shared<ConfigSection<T>>();
        sections[name] = sec;
        return sec;
    }

    template <typename T> std::shared_ptr<ConfigSection<T>> getSection(const std::string &name) {
        auto it = sections.find(name);
        if (it == sections.end()) {
            l::warn("Config section '{}' not found", name);
            return nullptr;
        }
        return std::dynamic_pointer_cast<ConfigSection<T>>(it->second);
    }

    bool load();
    bool save() const;
};

#define FIELD_MARSHAL(field, key) tbl.insert(key, data.field);

#define FIELD_UNMARSHAL(field, key)                                                                \
    if (auto v = tbl[key].value<decltype(data.field)>()) {                                         \
        data.field = *v;                                                                           \
    } else {                                                                                       \
        l::warn("Config field '{}' not found", key);                                               \
    }

#define MAKE_SECTION(TYPE, FIELDS_MACRO)                                                           \
    template <> struct ConfigSectionTraits<TYPE> {                                                 \
        static void marshal(toml::table &tbl, const TYPE &data) { FIELDS_MACRO(FIELD_MARSHAL) }    \
        static void unmarshal(const toml::table &tbl, TYPE &data) {                                \
            FIELDS_MACRO(FIELD_UNMARSHAL)                                                          \
        }                                                                                          \
    }

#endif // KONFIG_HEADER

#ifdef KONFIG_IMPLEMENTATION

ConfigManager::ConfigManager(std::string filepath) : path(std::move(filepath)) {}

bool ConfigManager::load() {
    toml::table doc;
    try {
        doc = toml::parse_file(path);
    } catch (const toml::parse_error &e) {
        l::error("Config load error (TOML parse): {} at line {}, column {}", e.what(),
                 e.source().begin.line, e.source().begin.column);
        return false;
    } catch (const std::exception &e) {
        l::error("Config load error: {}", e.what());
        return false;
    }

    for (auto &[name, sec_ptr] : sections) {
        if (auto tbl_section = doc[name].as_table()) {
            if (!sec_ptr->load(*tbl_section)) {
                l::warn("Failed to load section: {}", name);
            }
        } else {
            l::warn("Config section '{}' not found in file '{}'", name, path);
        }
    }
    return true;
}

bool ConfigManager::save() const {
    toml::table doc;
    for (const auto &[name, sec_ptr] : sections) {
        toml::table tbl_section;
        if (!sec_ptr->save(tbl_section)) {
            l::error("Failed to prepare section for saving: {}", name);
            return false;
        }
        doc.insert(name, tbl_section);
    }

    try {
        std::ofstream ofs(path);
        if (!ofs) {
            l::error("Config save error: Could not open file for writing: {}", path);
            return false;
        }
        ofs << doc;
    } catch (const std::exception &e) {
        l::error("Config save error (file write): {}", e.what());
        return false;
    }
    return true;
}

#endif // KONFIG_IMPLEMENTATION
