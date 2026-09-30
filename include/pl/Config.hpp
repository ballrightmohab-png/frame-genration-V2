#ifndef PL_CONFIG_HPP
#define PL_CONFIG_HPP

#include <string>
#include <fstream>
#include <iostream>

namespace pl::config {

    template <typename T>
    class ConfigFile {
    public:
        ConfigFile(std::string path, T defaultConfig)
            : m_path(std::move(path)), m_data(std::move(defaultConfig)) {}

        const T& get() const { return m_data; }
        T& get() { return m_data; }

        bool load() {
            std::ifstream file(m_path);
            if (!file.is_open()) return false;

            // Basic INI/KV parse into struct fields
            std::string line;
            while (std::getline(file, line)) {
                if (line.empty() || line[0] == '#' || line[0] == ';') continue;
                size_t eq = line.find('=');
                if (eq != std::string::npos) {
                    std::string key = line.substr(0, eq);
                    std::string val = line.substr(eq + 1);

                    // Trim whitespace
                    key.erase(0, key.find_first_not_of(" \t\r\n"));
                    key.erase(key.find_last_not_of(" \t\r\n") + 1);
                    val.erase(0, val.find_first_not_of(" \t\r\n"));
                    val.erase(val.find_last_not_of(" \t\r\n") + 1);

                    try {
                        m_data.parseField(key, val);
                    } catch (...) {}
                }
            }
            return true;
        }

        bool save() const {
            std::ofstream file(m_path);
            if (!file.is_open()) return false;

            m_data.writeFields(file);
            return true;
        }

    private:
        std::string m_path;
        T m_data;
    };

} // namespace pl::config

#endif // PL_CONFIG_HPP
