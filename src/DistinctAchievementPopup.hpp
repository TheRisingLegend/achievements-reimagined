#ifndef DISTINCTACHIEVEMENTPOPUP_HPP
#define DISTINCTACHIEVEMENTPOPUP_HPP

#include <Geode/Geode.hpp>

#include "AchievementCategoryPopup.hpp"

class DistinctAchievementPopup : public AchievementCategoryPopup {
   protected:
    bool setup(AchievementMenu* achievementMenu, Category* category) override;

   public:
    static DistinctAchievementPopup* create(AchievementMenu* achievementMenu, Category* category);

    cocos2d::CCNode* createPage(int pageNum);
};
#endif