#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

#include "AchievementMenuPopup.hpp"

using namespace geode::prelude;

class $modify(MyMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) {
            return false;
        }

        auto sprite = ButtonSprite::create("ACH");
        auto tempButton = CCMenuItemSpriteExtra::create(
            sprite,
            this,
            menu_selector(MyMenuLayer::onMyAchievements));
        tempButton->setID("my-achievements");

        auto bottomMenu = getChildByID("bottom-menu");
        bottomMenu->addChild(tempButton);
        bottomMenu->updateLayout();

        return true;
    }

    void onMyAchievements(CCObject* sender) {
        // auto popup = UnlockProgressionPopup::create("Icon progression");
        // popup->setID("Better-Unlock-Progression");
        // popup->setZOrder(95);
        // popup->m_noElasticity = GameManager::get()->getGameVariable("0168");  // For fast menu setting
        // if (popup) {
        //     popup->show();
        // }

        auto popup = AchievementMenuPopup::create();
        if (!popup) {
            return;
        }

        popup->setID("achievement-page");
        popup->setZOrder(100);
        popup->m_noElasticity = GameManager::get()->getGameVariable("0168");  // For fast menu setting
        popup->show();
    }
};