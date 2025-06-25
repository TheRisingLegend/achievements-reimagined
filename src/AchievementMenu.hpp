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

    std::vector<cocos2d::CCNode*> m_categoriesMenu;
    cocos2d::CCMenu* m_navMenu;
    cocos2d::CCMenu* m_navButtons;
    int m_categoryPage = 0;  // the current page of the category menu
    int m_maxCategoriesPerPage = 12;

    void createCategoryMenu();
    void addCategoryButtons(cocos2d::CCMenu* menuPage, std::string pageTitle);
    void onCategoryButton(CCObject* sender);

    void addNavigation();
    void onCategoryArrow(CCObject* sender);
    void onNavButton(CCObject* sender);
    void hideArrows();
    void showArrows();

    void addCornerSprites();

    UnlockType unlockTypeFromString(const std::string& str);
    Category* getCategoryForAchievement(const std::string& id, const std::string& achievedDescription);

    std::vector<Category> m_achievementCategories;
};

#endif