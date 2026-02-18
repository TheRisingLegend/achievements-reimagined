#ifndef DISTINCTPOPUP_HPP
#define DISTINCTPOPUP_HPP

#include <Geode/Geode.hpp>

#include "AchievementCategoryPopup.hpp"

class DistinctPopup : public AchievementCategoryPopup {
   protected:
    bool init(AchievementMenu* achievementMenu, Category* category);

   public:
    static DistinctPopup* create(AchievementMenu* achievementMenu, Category* category);

    cocos2d::CCNode* createPage(int pageNum);
};
#endif