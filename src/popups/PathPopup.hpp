#ifndef PATHPOPUP_HPP
#define PATHPOPUP_HPP

#include <Geode/Geode.hpp>

#include "AchievementCategoryPopup.hpp"

class PathPopup : public AchievementCategoryPopup {
   protected:
    bool init(AchievementMenu* achievementMenu, Category* category);

   public:
    static PathPopup* create(AchievementMenu* achievementMenu, Category* category);
    void addNavigation(int activePage);
    void onNavButton(CCObject* sender) override;
    void onArrow(CCObject* sender) override;

    cocos2d::CCNode* createPage(int pageNum);
};

#endif