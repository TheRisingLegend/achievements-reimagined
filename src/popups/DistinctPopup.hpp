#ifndef DISTINCTPOPUP_HPP
#define DISTINCTPOPUP_HPP

#include <Geode/Geode.hpp>

#include "AchievementCategoryPopup.hpp"

class DistinctPopup : public AchievementCategoryPopup {
   protected:
    bool setup(AchievementMenu* achievementMenu, Category* category) override;

   public:
    static DistinctPopup* create(AchievementMenu* achievementMenu, Category* category);

    cocos2d::CCNode* createPage(int pageNum);
};
#endif