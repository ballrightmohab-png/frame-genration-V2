#ifndef PL_MODMENU_HPP
#define PL_MODMENU_HPP

#include <string>
#include <string_view>
#include <vector>
#include <functional>
#include <memory>

namespace pl::modmenu {

    enum class ConfigType {
        BOOLEAN,
        INT,
        FLOAT,
        STRING,
        CHOICE
    };

    struct ConfigEntry {
        std::string id;
        std::string name;
        ConfigType type;
        std::string defaultValue;
        std::string value;
        std::string minValue;
        std::string maxValue;

        ConfigEntry() = default;
        ~ConfigEntry() = default;
        ConfigEntry(ConfigEntry&&) noexcept = default;
        ConfigEntry& operator=(ConfigEntry&&) noexcept = default;
        ConfigEntry(const ConfigEntry&) = default;
        ConfigEntry& operator=(const ConfigEntry&) = default;
    };

    struct ModuleInfo {
        std::string id;
        std::string name;
        std::string description;
        std::string modId = "framegen.core";
        bool defaultEnabled = true;
        bool hideInHudEditor = false;
        std::vector<ConfigEntry> configs;

        std::function<void(std::string_view, bool)> onToggleCallback;
        std::function<void(std::string_view, std::string_view, std::string_view)> onConfigChangedCallback;
        std::function<void(std::string_view, std::string_view, bool)> onKeybindCallback;

        ModuleInfo() = default;
        ~ModuleInfo() = default;
    };

    class ModuleBuilder {
    public:
        ModuleBuilder(std::string id, std::string name) {
            m_info.id = std::move(id);
            m_info.name = std::move(name);
        }

        ModuleBuilder& description(std::string desc) {
            m_info.description = std::move(desc);
            return *this;
        }

        ModuleBuilder& modId(std::string modId) {
            m_info.modId = std::move(modId);
            return *this;
        }

        ModuleBuilder& defaultEnabled(bool enabled) {
            m_info.defaultEnabled = enabled;
            return *this;
        }

        ModuleBuilder& hideInHudEditor(bool hide) {
            m_info.hideInHudEditor = hide;
            return *this;
        }

        ModuleBuilder& onToggle(std::function<void(std::string_view, bool)> cb) {
            m_info.onToggleCallback = std::move(cb);
            return *this;
        }

        ModuleBuilder& onConfigChanged(std::function<void(std::string_view key, std::string_view val, std::string_view oldVal)> cb) {
            m_info.onConfigChangedCallback = std::move(cb);
            return *this;
        }

        ModuleBuilder& onKeybind(std::function<void(std::string_view, std::string_view, bool)> cb) {
            m_info.onKeybindCallback = std::move(cb);
            return *this;
        }

        ModuleBuilder& config(std::string id, std::string name, ConfigType type,
                              std::string defaultVal, std::string minVal = "", std::string maxVal = "", std::string currentVal = "") {
            ConfigEntry entry;
            entry.id = std::move(id);
            entry.name = std::move(name);
            entry.type = type;
            entry.defaultValue = defaultVal;
            entry.value = currentVal.empty() ? entry.defaultValue : currentVal;
            entry.minValue = std::move(minVal);
            entry.maxValue = std::move(maxVal);
            m_info.configs.push_back(std::move(entry));
            return *this;
        }

        ModuleInfo build() {
            return m_info;
        }

        bool registerModule();

    private:
        ModuleInfo m_info;
    };

    // Global Preloader API functions (weak linkage / dynamic lookup ready)
    extern "C" {
        void registerModule(const ModuleInfo& info);
        void unregisterModule(std::string_view id);
    }

} // namespace pl::modmenu

#endif // PL_MODMENU_HPP
