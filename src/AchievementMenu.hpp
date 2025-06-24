#ifndef ACHIEVEMENTMENU_HPP
#define ACHIEVEMENTMENU_HPP

#include <Geode/Geode.hpp>

#include "utils.hpp"

class AchievementMenu : public geode::Popup<> {
   protected:
    bool setup() override;

   public:
    static AchievementMenu* create();

    AchievementManager* m_achievementManager;

    std::vector<cocos2d::CCMenu*> m_categoriesMenu;
    cocos2d::CCMenu* m_navMenu;
    cocos2d::CCMenu* m_navButtons;
    int m_categoryPage = 0;  // the current page of the category menu
    int m_maxCategoriesPerPage = 12;

    // std::vector<Achievement*> m_achievements;

    void createCategoryMenu();

    void addNavigation();
    void onCategoryArrow(CCObject* sender);
    void onNavButton(CCObject* sender);
    void hideArrows();
    void showArrows();

    void addCategoryButtons(cocos2d::CCMenu* menuPage, int pageNum);
    void onCategoryButton(CCObject* sender);

    UnlockType unlockTypeFromString(const std::string& str);
};
#endif