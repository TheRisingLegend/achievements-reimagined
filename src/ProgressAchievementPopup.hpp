#ifndef PROGRESSACHIEVEMENTPOPUP_HPP
#define PROGRESSACHIEVEMENTPOPUP_HPP

#include <Geode/Geode.hpp>

#include "AchievementCategoryPopup.hpp"

class ProgressAchievementPopup : public AchievementCategoryPopup {
   protected:
    bool setup(AchievementMenu* achievementMenu, Category* category) override;

   public:
    static ProgressAchievementPopup* create(AchievementMenu* achievementMenu, Category* category);

    cocos2d::CCNode* createPage(int pageNum);

    int m_statValue;
};

#endif