#ifndef UTILS_HPP
#define UTILS_HPP

#include <Geode/Geode.hpp>

struct Achievement {
    std::string id;
    std::string title;
    std::string achievedDescription;
    std::string unachievedDescription;
    UnlockType unlockType;
    int unlockID;
    int unlockValue;
};

struct Category {
    std::string name;
    std::string formattedName;  // with new lines
    std::string page;           // which page: "Levels", "Stats", "Social", or "Other"
    std::string displayType;    // "distinct" or "progress"
    std::string logo;
    std::vector<std::string> identifiers;
    std::string statKey;
    std::vector<Achievement*> achievements;
};

struct IconCallbackData : public cocos2d::CCObject {
    UnlockType unlockType;
    int unlockID;

    IconCallbackData(UnlockType type, int id) : unlockType(type), unlockID(id) {}
};

extern std::map<std::string, std::tuple<std::string, std::string>> betterDescriptions;

extern AchievementManager* achievementManager;
extern GameManager* gameManager;
extern GameStatsManager* gameStatsManager;
extern GameLevelManager* gameLevelManager;

Category* getCategoryForAchievement(const std::string& id, const std::string& achievedDescription);
std::string formatWithCommas(int number);
int extractValue(const std::string& desc);

#endif