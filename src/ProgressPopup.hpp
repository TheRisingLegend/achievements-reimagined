#ifndef PROGRESSPOPUP_HPP
#define PROGRESSPOPUP_HPP

#include <Geode/Geode.hpp>

#include "AchievementCategoryPopup.hpp"

class ProgressPopup : public AchievementCategoryPopup {
   protected:
    bool setup(AchievementMenu* achievementMenu, Category* category) override;

   public:
    static ProgressPopup* create(AchievementMenu* achievementMenu, Category* category);

    cocos2d::CCNode* createPage(int pageNum);
    cocos2d::CCNode* createProgressBar(int pageNum, int numIconsOnPage);

    int m_statValue;
};

#endif