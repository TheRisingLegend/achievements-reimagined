#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

#include "AchievementMenu.hpp"

using namespace geode::prelude;

class $modify(MyMenuLayer, MenuLayer) {
    void onAchievements(CCObject* sender) {
        auto popup = AchievementMenu::create();
        if (!popup) {
            return;
        }

        popup->setID("achievement-page");
        popup->setZOrder(100);
        popup->m_noElasticity = GameManager::get()->getGameVariable("0168");  // For fast menu setting
        popup->show();
    }
};