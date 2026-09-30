#include "pl/ModMenu.hpp"
#include <iostream>
#include <unordered_map>

namespace pl::modmenu {

    static std::unordered_map<std::string, ModuleInfo>& getRegistry() {
        static std::unordered_map<std::string, ModuleInfo> registry;
        return registry;
    }

    bool ModuleBuilder::registerModule() {
        pl::modmenu::registerModule(m_info);
        return true;
    }

    extern "C" {
        void registerModule(const ModuleInfo& info) {
            std::cout << "[pl::modmenu] Registered Mod Menu Module: " << info.name << " (" << info.id << ")" << std::endl;
            getRegistry()[info.id] = info;
        }

        void unregisterModule(std::string_view id) {
            std::cout << "[pl::modmenu] Unregistered Mod Menu Module: " << id << std::endl;
            getRegistry().erase(std::string(id));
        }
    }

} // namespace pl::modmenu
