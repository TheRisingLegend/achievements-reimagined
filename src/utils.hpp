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
    std::string formattedName;
    std::string type;
    std::vector<std::string> identifiers;
    std::string statKey;
    std::vector<Achievement*> achievements;
};

struct IconCallbackData : public cocos2d::CCObject {
    UnlockType unlockType;
    int unlockID;

    IconCallbackData(UnlockType type, int id) : unlockType(type), unlockID(id) {}
};

extern std::vector<Category> achievementCategories;
extern std::map<std::string, std::tuple<std::string, std::string>> betterDescriptions;
extern bool achievementsLoaded;

extern AchievementManager* achievementManager;
extern GameManager* gameManager;
extern GameStatsManager* gameStatsManager;
extern GameLevelManager* gameLevelManager;

Category* getCategory(const std::string& id, const std::string& achievedDescription);
std::string formatWithCommas(int number);
int extractValue(const std::string& desc);

/* to figure out stat ids */
// auto stats = gameStatsManager->m_playerStats;
// auto keys = stats->allKeys();
// for (int i = 0; i < keys->count(); i++) {
//     auto key = typeinfo_cast<CCString*>(keys->objectAtIndex(i));
//     if (key) {
//         auto value = stats->valueForKey(key->getCString());
//         log::debug("Key: {}, Value: {}", key->getCString(), value ? value->getCString() : "null");
//     }
// }

/* game stats:
1: jumps
2: attempts
3: completed levels
4: completed online levels
5: completed demon levels
6: stars
7: completed map packs
8: secret coins
9: players destroyed
10: liked/disliked levels
11: rated levels
12: user coins
13: total diamonds
14: current orbs
15: completed daily levels
16: fire shards
17: ice shards
18: poison shards
19: shadow shards
20: lava shards
21:
22: total orbs
23: earth shards
24: blood shards
25: metal shards
26: light shards
27: soul shards
28: moons
29: current diamonds
30: stars/moons towards path - fire
31: stars/moons towards path - ice
32: stars/moons towards path - poison
33: stars/moons towards path - shadow
34: stars/moons towards path - lava
35: stars/moons towards path - earth
36: stars/moons towards path - blood
37: stars/moons towards path - metal
38: stars/moons towards path - light
39: stars/moons towards path - souls
40: completed gauntlets
41: collected list rewards
42: completed insane levels
43:
uniques: ...
*/

#endif