#ifndef MOD_MENU_H
#define MOD_MENU_H

#include "LeviMod.h"
#include <string>
#include <vector>
#include <functional>

namespace LeviMod {

    struct MenuItem {
        std::string label;
        std::string description;
        std::function<std::string()> getValue;
        std::function<void()> toggleOrAdjust;
    };

    class ModMenu {
    public:
        ModMenu();

        void renderMenu() const;
        void selectNext();
        void selectPrevious();
        void toggleSelected();

        bool isVisible() const { return m_visible; }
        void setVisible(bool visible) { m_visible = visible; }
        void toggleVisibility() { m_visible = !m_visible; }

        int getSelectedIndex() const { return m_selectedIndex; }

    private:
        void setupMenuItems();

        bool m_visible = true;
        int m_selectedIndex = 0;
        std::vector<MenuItem> m_items;
    };

} // namespace LeviMod

#endif // MOD_MENU_H
