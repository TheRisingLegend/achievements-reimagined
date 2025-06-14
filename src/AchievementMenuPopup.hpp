#ifndef ACHIEVEMENTMENUPOPUP_HPP
#define ACHIEVEMENTMENUPOPUP_HPP

#include <Geode/Geode.hpp>

class AchievementMenuPopup : public geode::Popup<> {
   protected:
    bool setup() override;

   public:
    static AchievementMenuPopup* create();

    AchievementManager* m_achievementManager;

    std::vector<cocos2d::CCMenu*> m_categoriesMenu;
    CCMenuItemSpriteExtra* m_leftArrow;
    CCMenuItemSpriteExtra* m_rightArrow;
    int m_categoryPage = 0;  // the current page of the category menu
    int m_maxCategoriesPerPage = 12;

    std::vector<std::tuple<std::string, std::string, std::string, std::string, UnlockType, int>> m_achievements;  // <category, title, achievedDescription, unachievedDescription, UnlockType, unlockID>
    UnlockType unlockTypeFromString(const std::string& str);

    void createCategoryMenu();
    void onCategoryArrow(CCObject* sender);
    void addCategoryButtons(cocos2d::CCMenu* menuPage, int pageNum);
    void onCategoryButton(CCObject* sender);

    std::vector<std::tuple<std::string, std::string>> m_categories = {
        {"Stars", "stars"},
        {"Moons", "moons"},
        {"Secret Coins", "secretCoins"},
        {"User Coins", "userCoins"},
        {"Diamonds", "diamonds"},
        {"Jumps", "jumps"},
        {"Attempts", "attempts"},
        {"Players Destroyed", "playersDestroyed"},
        {"Friends", "friends"},
        {"Followed Creators", "followedCreators"},
        {"Liked/Disliked Levels", "likedDislikedLevels"},
        {"Rated Levels", "ratedLevels"},
        {"User Levels", "userLevels"},
        {"Insanes", "insanes"},
        {"Demons", "demons"},
        {"Daily Levels", "dailyLevels"},
        {"Map Packs", "mapPacks"},
        {"Gauntlets", "gauntlets"},
        {"Lists", "lists"},
        {"Created Level Likes", "createdLevelLikes"},
        {"Creator Points", "creatorPoints"}};
};

#endif