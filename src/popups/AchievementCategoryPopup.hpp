#ifndef ACHIEVEMENTCATEGORYPOPUP_HPP
#define ACHIEVEMENTCATEGORYPOPUP_HPP

#include <Geode/Geode.hpp>

#include "../utils.hpp"

class AchievementMenu;
class AchievementCategoryPopup : public geode::Popup<AchievementMenu*, Category*> {
   public:
    void createTitle();
    void addProgressText(int percent, int statValue, int goalValue);
    void addCornerSprites();
    void addNavigation();

    virtual cocos2d::CCNode* createPage(int pageNum) = 0;

    void onIcon(CCObject* sender);
    virtual void onNavButton(CCObject* sender);
    virtual void onArrow(CCObject* sender);
    void onClose(CCObject* sender) override;

    int m_maxIconsPerPage;
    int m_numPages;
    Category* m_category;
    int m_numAchievements;

    cocos2d::CCMenu* m_navMenu;
    cocos2d::CCMenu* m_navButtons;

    AchievementMenu* m_achievementMenu;
};

#endif