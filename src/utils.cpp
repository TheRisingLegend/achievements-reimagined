#include "utils.hpp"

#include <regex>

using namespace geode::prelude;

std::vector<Category> achievementCategories = {
    {"Main Levels", "Main Levels", "distinct", {"level##a", "level##b", "demoncoin##", "special##"}},
    {"Tower Levels", "Tower Levels", "distinct", {"tower##", "tower##Coin"}},
    {"User Levels", "User Levels", "progress", {"custom##"}, "4"},
    {"Meltdown", "Meltdown", "distinct", {"mdlevel##b", "mdcoin##", "mdrate"}},
    {"World", "World", "distinct", {"world"}},
    {"Subzero", "Subzero", "distinct", {"subzero"}},
    {"Stars", "Stars", "progress", {"stars##"}, "6"},
    {"Moons", "Moons", "progress", {"moons##"}, "28"},
    {"Diamonds", "Diamonds", "progress", {"diamonds##"}, "13"},
    {"Secret Coins", "Secret Coins", "progress", {"coins##"}, "8"},
    {"User Coins", "User Coins", "progress", {"usercoins##"}, "12"},
    {"Creator", "Creator", "distinct", {"creator##", "submit"}},
    {"Demons", "Demons", "progress", {"demon##"}, "5"},
    {"Insanes", "Insanes", "progress", {"insane##"}, "42"},
    {"Jumps", "Jumps", "progress", {"jump##"}, "1"},
    {"Attempts", "Attempts", "progress", {"attempt##"}, "2"},
    {"Daily Levels", "Daily Levels", "progress", {"daily##"}, "15"},
    {"Map Packs", "Map Packs", "progress", {"mappacks##"}, "7"},
    {"Gauntlets", "Gauntlets", "progress", {"gauntlets##"}, "40"},
    {"Lists", "Lists", "progress", {"lists##"}, "41"},
    {"Liked/Disliked Levels", "Liked/Disliked\nLevels", "progress", {"like", "like##", "like##b"}, "10"},
    {"Rated Levels", "Rated Levels", "progress", {"rateDiff", "rateDiff##", "rateDiff##b"}, "11"},
    {"Followed Creators", "Followed\nCreators", "progress", {"followCreator", "followCreator##"}},
    {"Friends", "Friends", "distinct", {"friends##"}},
    {"Players Destroyed", "Players\nDestroyed", "progress", {}, "9"},
    {"Secret", "Secret", "distinct", {"secret##", "secret##b"}},
    {"Vaults", "Vaults", "distinct", {"v#"}},
    {"Misc", "Misc", "distinct", {"rate", "moreGames", "facebook", "youtube", "twitter"}},
    {"Steam Exclusive", "Steam\nExclusive", "distinct", {"steam##"}}};

std::map<std::string, std::tuple<std::string, std::string>> betterDescriptions = {
    // Secret
    {"geometry.ach.secret04", {"Find the hidden coin on the Coming Soon screen", "Found the hidden coin on the Coming Soon screen"}},
    {"geometry.ach.secret11", {"Find the secret by destroying this cube on the main menu", "Found the secret by destroying this cube on the main menu"}},
    {"geometry.ach.secret12", {"Find the secret by destroying this cube on the main menu", "Found the secret by destroying this cube on the main menu"}},

    // Vaults
    {"geometry.ach.secret05", {"Find the secret by entering 'lenny' into the Vault", "Found the secret by entering 'lenny' into the Vault"}},
    {"geometry.ach.secret06", {"Steal the Vault Keeper's coin by entering 'sparky' into the Vault", "Stole the Vault Keeper's coin by entering 'sparky' into the Vault"}},
    {"geometry.ach.secret07", {"Steal the Vault Keeper's icon by entering 'spooky' into the Vault", "Stole the Vault Keeper's icon by entering 'spooky' into the Vault"}},
    {"geometry.ach.secret08", {"Find the secret by entering 'blockbite' into the Vault", "Found the secret by entering 'blockbite' into the Vault"}},
    {"geometry.ach.secret09", {"Find the secret by entering 'robotop' into the Vault", "Found the secret by entering 'robotop' into the Vault"}},
    {"geometry.ach.secret10", {"Find the secret by entering 'ahead' into the Vault", "Found the secret by entering 'ahead' into the Vault"}},
    {"geometry.ach.secret13", {"Find the secret by entering 'mule' into the Vault", "Found the secret by entering 'mule' into the Vault"}},
    {"geometry.ach.secret14", {"Find the secret by entering 'neverending' into the Vault", "Found the secret by entering 'neverending' into the Vault"}},
    {"geometry.ach.secret15", {"Find the secret by entering 'gandalfpotter' into the Vault", "Found the secret by entering 'gandalfpotter' into the Vault"}},
    {"geometry.ach.secret16", {"Find the secret by consecutively entering '8', '16', '30', '32', '46' and '84' into the Vault", "Found the secret by consecutively entering '8', '16', '30', '32', '46' and '84' into the Vault"}},
    {"geometry.ach.secret17", {"Find the secret by entering your username into the Vault", "Found the secret by entering your username into the Vault"}},
    {"geometry.ach.secret19", {"Find the secret by entering 'finalboss' into the Vault", "Found the secret by entering 'finalboss' into the Vault"}},
    {"geometry.ach.v2.secret01", {"Find the secret by entering 'brainpower' into the Vault of Secrets", "Found the secret by entering 'brainpower' into the Vault of Secrets"}},
    {"geometry.ach.v2.secret02", {"Find the secret by solving the 'cod3breaker' puzzle in the Vault of Secrets", "Found the secret by solving the 'cod3breaker' puzzle in the Vault of Secrets"}},
    {"geometry.ach.v2.secret03", {"Steal the Keymaster's coin by solving the 'glubfub' puzzle in the Vault of Secrets", "Stole the Keymaster's coin by solving the 'glubfub' puzzle in the Vault of Secrets"}},
    {"geometry.ach.v2.secret04", {"Find the secret by entering 'octocube' into the Vault of Secrets", "Found the secret by entering 'octocube' into the Vault of Secrets"}},
    {"geometry.ach.v2.secret05", {"Find the secret by entering your star count into the Vault of Secrets", "Found the secret by entering your star count into the Vault of Secrets"}},
    {"geometry.ach.v2.secret06", {"Find the secret by entering 'seven' into the Vault of Secrets", "Found the secret by entering 'seven' into the Vault of Secrets"}},
    {"geometry.ach.v2.secret07", {"Find the secret by entering 'gimmiethecolor' into the Vault of Secrets", "Found the secret by entering 'gimmiethecolor' into the Vault of Secrets"}},
    {"geometry.ach.v2.secret08", {"Find the secret by entering 'thechickenisonfire' into the Vault of Secrets", "Found the secret by entering 'thechickenisonfire' into the Vault of Secrets"}},
    {"geometry.ach.v2.secret09", {"Find the secret by entering 'd4shg30me7ry' into the Vault of Secrets", "Found the secret by entering 'd4shg30me7ry' into the Vault of Secrets"}},
    {"geometry.ach.v2.secret10", {"Find the secret by entering 'thechickenisready' into the Vault of Secrets", "Found the secret by entering 'thechickenisready' into the Vault of Secrets"}},
    {"geometry.ach.v3.secret01", {"Find the secret by entering 'darkness' into the Chamber of Time", "Found the secret by entering 'darkness' into the Chamber of Time"}},
    {"geometry.ach.v3.secret02", {"Find the secret by entering 'silence' into the Chamber of Time", "Found the secret by entering 'silence' into the Chamber of Time"}},
    {"geometry.ach.v3.secret03", {"Find the secret by entering 'river' into the Chamber of Time", "Found the secret by entering 'river' into the Chamber of Time"}},
    {"geometry.ach.v3.secret04", {"Find the secret by entering 'hunger' into the Chamber of Time", "Found the secret by entering 'hunger' into the Chamber of Time"}},
    {"geometry.ach.v3.secret05", {"Find the secret by entering 'volcano' into the Chamber of Time", "Found the secret by entering 'volcano' into the Chamber of Time"}},
    {"geometry.ach.v3.secret06", {"Find the secret by entering 'backontrack' into the Chamber of Time", "Found the secret by entering 'backontrack' into the Chamber of Time"}},
    {"geometry.ach.v3.secret07", {"Find the secret by entering 'givemehelper' into the Chamber of Time", "Found the secret by entering 'givemehelper' into the Chamber of Time"}},

    // Players Destroyed
    {"geometry.ach.secret01", {"Destroy 1 player on the main menu", "Destroyed 1 player on the main menu"}},
    {"geometry.ach.secret02", {"Destroy 50 players on the main menu", "Destroyed 50 players on the main menu"}},
    {"geometry.ach.secret02b", {"Destroy 100 players on the main menu", "Destroyed 100 players on the main menu"}},
    {"geometry.ach.secret03", {"Destroy 200 players on the main menu", "Destroyed 200 players on the main menu"}},
    {"geometry.ach.secret03b", {"Destroy 500 players on the main menu", "Destroyed 500 players on the main menu"}},
    {"geometry.ach.secret18", {"Destroy 750 players on the main menu", "Destroyed 750 players on the main menu"}},

};

bool achievementsLoaded = false;

AchievementManager* achievementManager = AchievementManager::sharedState();
GameManager* gameManager = GameManager::sharedState();
GameStatsManager* gameStatsManager = GameStatsManager::sharedState();
GameLevelManager* gameLevelManager = GameLevelManager::sharedState();

Category* getCategory(const std::string& id, const std::string& achievedDescription) {
    std::string sub;
    std::stringstream ss(id);
    std::getline(ss, sub, '.');
    std::getline(ss, sub, '.');
    std::getline(ss, sub, '.');

    std::string generic;
    for (auto it = sub.begin(); it < sub.end(); it++) {
        auto c = *it;
        if (c >= '0' && c <= '9') {
            generic += "#";
        } else {
            generic += c;
        }
    }

    // Currently don't support path or shards achievements
    // Could just open their vanilla menus when the category is clicked or
    // build own menus for each to match the style of the mod
    if (generic == "path##" || generic.find("shard") != std::string::npos) {
        return nullptr;
    }

    // 'Players destroyed' achievements are grouped in with the secret achievements, so take those out based on their unlock description
    // Additionally, some 'Vault' achievements are grouped in with the secret achievements
    for (Category& cat : achievementCategories) {
        for (std::string id : cat.identifiers) {
            if (generic == id) {
                if (cat.name == "Secret" && achievedDescription.find("Destroyed") != std::string::npos)
                    return &achievementCategories[24];  // Players Destroyed

                if (cat.name == "Secret" && achievedDescription.find("Vault") != std::string::npos)
                    return &achievementCategories[26];  // Vaults

                return &cat;
            }
        }
    }

    return nullptr;
}

std::string formatWithCommas(int number) {
    std::string s = std::to_string(number);
    int n = s.length();
    for (int i = n - 3; i > 0; i -= 3) {
        s.insert(i, ",");
    }
    return s;
}

// entry condition: this achievement is part of a progress category
int extractValue(const std::string& desc) {
    std::regex numberRegex(R"((\d+))");
    std::smatch match;
    if (std::regex_search(desc, match, numberRegex)) {
        return std::stoi(match[1].str());
    }
    return 1;
}