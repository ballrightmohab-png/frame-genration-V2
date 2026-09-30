#ifndef PL_MOD_HPP
#define PL_MOD_HPP

#include <string>
#include <iostream>

namespace pl::mod {

    class Mod {
    public:
        virtual ~Mod() = default;
        virtual bool load() = 0;
        virtual bool unload() = 0;
        virtual bool enable() = 0;
        virtual bool disable() = 0;

        const std::string& getId() const { return m_id; }
        const std::string& getName() const { return m_name; }

    protected:
        std::string m_id = "framegen_camera_smoothing";
        std::string m_name = "Frame Generation + Camera Smoothing";
    };

} // namespace pl::mod

#endif // PL_MOD_HPP
