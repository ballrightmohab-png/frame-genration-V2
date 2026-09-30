#ifndef FRAMEGEN_MOD_HPP
#define FRAMEGEN_MOD_HPP

#include "LeviMod.h"
#include "pl/Mod.hpp"
#include "pl/Config.hpp"
#include "pl/ModMenu.hpp"
#include "RuntimeSettings.hpp"
#include "hooks/LeviHooks.h"
#include <memory>

namespace LeviMod {

    constexpr const char* ModuleId = "framegen_camera_smoothing";

    class FrameGenMod : public pl::mod::Mod {
    public:
        static FrameGenMod& getInstance() {
            static FrameGenMod instance;
            return instance;
        }

        bool load() override;
        bool unload() override;
        bool enable() override;
        bool disable() override;

        RuntimeSettings& getRuntime() { return m_runtime; }
        const RuntimeSettings& getRuntime() const { return m_runtime; }

        void saveConfig();

    private:
        FrameGenMod();
        ~FrameGenMod() override = default;

        void registerModMenu();
        void unregisterModMenu();

        RuntimeSettings m_runtime;
        pl::config::ConfigFile<FrameGenConfig> m_configFile;
        bool m_enabled = false;
    };

} // namespace LeviMod

#endif // FRAMEGEN_MOD_HPP
