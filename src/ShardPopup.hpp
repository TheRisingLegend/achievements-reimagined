#ifndef SHARDPOPUP_HPP
#define SHARDPOPUP_HPP

#include <Geode/Geode.hpp>

#include "AchievementCategoryPopup.hpp"

class ShardPopup : public AchievementCategoryPopup {
   protected:
    bool setup(AchievementMenu* achievementMenu, Category* category) override;
    const std::vector<const char*> shardSprites = {"fireShardBig_001.png", "iceShardBig_001.png", "poisonShardBig_001.png", "shadowShardBig_001.png", "lavaShardBig_001.png", "bonusShardSmall_001.png", "shard0201ShardBig_001.png", "shard0202ShardBig_001.png", "shard0203ShardBig_001.png", "shard0204ShardBig_001.png", "shard0205ShardBig_001.png", "bonusShard2Small_001.png"};
    const std::vector<const char*> shardLogos = {"fireShardLabel_001.png", "iceShardLabel_001.png", "poisonShardLabel_001.png", "shadowShardLabel_001.png", "lavaShardLabel_001.png", "bonusShardLabel_001.png", "shard0201ShardLabel_001.png", "shard0202ShardLabel_001.png", "shard0203ShardLabel_001.png", "shard0204ShardLabel_001.png", "shard0205ShardLabel_001.png", "bonusShardLabel_001.png"};
    std::map<int, int> gameStatIDs = {{0, 18}, {1, 19}, {2, 17}, {3, 16}, {4, 20}, {5, -1}, {6, 23}, {7, 24}, {8, 25}, {9, 26}, {10, 27}, {11, -1}};

   public:
    static ShardPopup* create(AchievementMenu* achievementMenu, Category* category);
    void addNavigation(int activePage);
    void onNavButton(CCObject* sender) override;
    void onArrow(CCObject* sender) override;

    cocos2d::CCNode* createPage(int pageNum);
};

#endif