#include "AchievementMenu.hpp"

#include "DistinctAchievementPopup.hpp"
#include "PathAchievementPopup.hpp"
#include "ProgressAchievementPopup.hpp"
#include "ShardAchievementPopup.hpp"

using namespace geode::prelude;

bool AchievementMenu::setup() {
    m_achievementManager = AchievementManager::sharedState();

    m_achievementCategories = {
        // Levels
        {"Main Levels", "Main Levels", "Levels", "distinct", "GJ_playBtn_001.png", {"level##a", "level##b", "demoncoin##", "special##"}},
        {"Tower Levels", "Tower Levels", "Levels", "distinct", "theTower_01_001.png", {"tower##", "tower##Coin"}},
        {"User Levels", "User Levels", "Levels", "progress", "GJ_creatorBtn_001.png", {"custom##"}, "4"},
        {"Meltdown", "Meltdown", "Levels", "distinct", "GJ_md_001.png", {"mdlevel##b", "mdcoin##", "mdrate"}},
        {"World", "World", "Levels", "distinct", "gj_worldLogo_001.png", {"world"}},
        {"Subzero", "Subzero", "Levels", "distinct", "gj_subzeroLogo_001.png", {"subzero"}},
        {"Demons", "Demons", "Levels", "progress", "diffIcon_06_btn_001.png", {"demon##"}, "5"},
        {"Insanes", "Insanes", "Levels", "progress", "diffIcon_05_btn_001.png", {"insane##"}, "42"},
        {"Daily Levels", "Daily Levels", "Levels", "progress", "gj_dailyCrown_001.png", {"daily##"}, "15"},
        {"Map Packs", "Map Packs", "Levels", "progress", "GJ_mapPacksBtn_001.png", {"mappacks##"}, "7"},
        {"Gauntlets", "Gauntlets", "Levels", "progress", "GJ_gauntletsBtn_001.png", {"gauntlets##"}, "40"},
        {"Lists", "Lists", "Levels", "progress", "topListsLabel_001.png", {"lists##"}, "41"},

        // Stats
        {"Stars", "Stars", "Stats", "progress", "GJ_bigStar_noShadow_001.png", {"stars##"}, "6"},
        {"Moons", "Moons", "Stats", "progress", "GJ_bigMoon_noShadow_001.png", {"moons##"}, "28"},
        {"Diamonds", "Diamonds", "Stats", "progress", "GJ_bigDiamond_noShadow_001.png", {"diamonds##"}, "13"},
        {"Secret Coins", "Secret Coins", "Stats", "progress", "secretCoinUI_001.png", {"coins##"}, "8"},
        {"User Coins", "User Coins", "Stats", "progress", "secretCoinUI2_001.png", {"usercoins##"}, "12"},
        {"Jumps", "Jumps", "Stats", "progress", "PBtn_Jump_001.png", {"jump##"}, "1"},
        {"Attempts", "Attempts", "Stats", "progress", "GJ_playBtn2_001.png", {"attempt##"}, "2"},
        {"Shards", "Shards", "Stats", "shard", "GJ_shardsBtn_001.png", {}},
        {"Paths", "Paths", "Stats", "path", "GJ_pathsBtn_001.png", {}},

        // Social
        {"Liked/Disliked Levels", "Liked/Disliked\nLevels", "Social", "progress", "GJ_like2Btn_001.png", {"like", "like##", "like##b"}, "10"},
        {"Rated Levels", "Rated Levels", "Social", "progress", "GJ_starBtn_001.png", {"rateDiff", "rateDiff##", "rateDiff##b"}, "11"},
        {"Followed Creators", "Followed\nCreators", "Social", "progress", "gj_heartOn_001.png", {"followCreator", "followCreator##"}},
        {"Friends", "Friends", "Social", "distinct", "GJ_sFriendsIcon_001.png", {"friends##"}},

        // Other
        {"Creator", "Creator", "Other", "distinct", "GJ_hammerIcon_001.png", {"creator##", "submit"}},
        {"Vaults", "Vaults", "Other", "distinct", "secretDoorBtn2_open_001.png", {"v#"}},
        {"Players Destroyed", "Players\nDestroyed", "Other", "progress", "playerExplosion_01_001.png", {}, "9"},
        {"Secret", "Secret", "Other", "distinct", "GJ_lock_open_001.png", {"secret##", "secret##b"}},
        {"Misc", "Misc", "Other", "distinct", "GJ_plusBtn_001.png", {"rate", "moreGames", "facebook", "youtube", "twitter"}},
        {"Steam Exclusive", "Steam\nExclusive", "Other", "distinct", "secretCoin_2_b_01_001.png", {"steam##"}}};

    setTitle("Achievements", "goldFont.fnt", 1.0f);

    addCornerSprites();

    CCArray* array = m_achievementManager->m_allAchievements;
    for (int i = 0; i < array->count(); i++) {
        auto item = array->objectAtIndex(i);

        auto dict = typeinfo_cast<CCDictionary*>(item);
        if (!dict) continue;

        Achievement* ach = new Achievement();

        ach->title = std::string(dict->valueForKey("title")->getCString());
        ach->id = std::string(dict->valueForKey("identifier")->getCString());

        Category* category = getCategoryForAchievement(ach->id, betterDescriptions.contains(ach->id) ? std::get<1>(betterDescriptions[ach->id]) : std::string(dict->valueForKey("achievedDescription")->getCString()));
        if (category == nullptr) {
            log::debug("Category not found for ID: {}", ach->id);
            continue;
        }

        if (betterDescriptions.contains(ach->id) && (category->name == "Players Destroyed" || Mod::get()->getSettingValue<bool>("show-secrets"))) {
            log::debug("{} Using better descriptions for achievement: {}\t{}", betterDescriptions.contains(ach->id), ach->id, category->name);
            ach->unachievedDescription = std::get<0>(betterDescriptions[ach->id]);
            ach->achievedDescription = std::get<1>(betterDescriptions[ach->id]);

            dict->setObject(CCString::create(ach->unachievedDescription), "unachievedDescription");
            dict->setObject(CCString::create(ach->achievedDescription), "achievedDescription");
        } else {
            ach->achievedDescription = std::string(dict->valueForKey("achievedDescription")->getCString());
            ach->unachievedDescription = std::string(dict->valueForKey("unachievedDescription")->getCString());
        }

        ach->unlockValue = category->displayType == "progress" || category->displayType == "shard" ? extractValue(ach->achievedDescription) : -1;

        std::string icon = std::string(dict->valueForKey("icon")->getCString());
        size_t pos = icon.find('_');
        if (pos != std::string::npos) {
            ach->unlockID = stoi(icon.substr(pos + 1));
            ach->unlockType = unlockTypeFromString(icon.substr(0, pos));
        } else {  // coin
            ach->unlockID = -1;
            ach->unlockType = UnlockType::GJItem;
        }

        category->achievements.push_back(ach);
    }

    createCategoryMenu();

    addNavigation();

    return true;
}

AchievementMenu* AchievementMenu::create() {
    auto popup = new AchievementMenu();
    if (popup && popup->initAnchored(450.f, 280.f)) {
        popup->autorelease();
        return popup;
    }
    delete popup;
    return nullptr;
}

void AchievementMenu::createCategoryMenu() {
    std::vector<std::string> pageTitles = {"Levels", "Stats", "Social", "Other"};
    for (int i = 0; i < pageTitles.size(); i++) {
        auto menuPage = CCNode::create();
        menuPage->setID("page-" + std::to_string(i));
        menuPage->setTag(i);
        menuPage->setContentSize({m_mainLayer->getContentWidth(), m_mainLayer->getContentHeight() - 70.f});
        menuPage->setPosition({0, 0});
        menuPage->setVisible(i == m_categoryPage);
        m_mainLayer->addChild(menuPage);

        auto subTitle = CCLabelBMFont::create(pageTitles[i].c_str(), "bigFont.fnt");
        subTitle->setID("page-subtitle");
        subTitle->setScale(0.6f);
        subTitle->setPosition({menuPage->getContentWidth() / 2, 235});
        menuPage->addChild(subTitle);

        auto buttonMenu = CCMenu::create();
        buttonMenu->setID("categories-menu");
        buttonMenu->setContentSize({m_mainLayer->getContentWidth() - 100, m_mainLayer->getContentHeight() - 70.f});
        buttonMenu->setPosition({m_mainLayer->getContentWidth() / 2, m_mainLayer->getContentHeight() / 2 - 13.f});
        buttonMenu->setLayout(RowLayout::create()
                                  ->setGap(12.f)
                                  ->setAxisAlignment(AxisAlignment::Center)
                                  ->setCrossAxisAlignment(AxisAlignment::Even)
                                  ->setGrowCrossAxis(true));
        menuPage->addChild(buttonMenu);

        addCategoryButtons(buttonMenu, pageTitles[i]);
        buttonMenu->updateLayout();

        m_categoriesMenu.push_back(menuPage);
    }
}

void AchievementMenu::addCategoryButtons(CCMenu* menuPage, std::string pageTitle) {
    for (int i = 0; i < m_achievementCategories.size(); i++) {
        if (m_achievementCategories[i].page != pageTitle) continue;

        ButtonSprite* buttonSprite = ButtonSprite::create(m_achievementCategories[i].formattedName.c_str(), 90.f, true, "bigFont.fnt", "GJ_button_01.png", 40.f, 0.5f);

        // CCSprite* logo = CCSprite::createWithSpriteFrameName(m_achievementCategories[i].logo.c_str());
        // if (!logo) {
        //     log::error("Failed to load logo for category: {}", m_achievementCategories[i].name);

        // } else {
        //     logo->setID("logo");
        //     logo->setScale(25.f / logo->getContentWidth());
        //     logo->setPosition({buttonSprite->getContentWidth() / 2, 0});
        //     logo->setZOrder(1);
        //     buttonSprite->addChild(logo);
        // }

        if (!Mod::get()->getSettingValue<bool>("hide-category-checkmarks")) {
            CCSprite* checkmark = CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png");
            checkmark->setID("checkmark");
            checkmark->setPosition({buttonSprite->getContentWidth() - 5.f, buttonSprite->getContentHeight() - 5.f});
            checkmark->setZOrder(1);
            buttonSprite->addChild(checkmark);

            bool isCategoryCompleted = true;
            for (const Achievement* ach : m_achievementCategories[i].achievements) {
                if (!achievementManager->isAchievementEarned(ach->id.c_str())) {
                    isCategoryCompleted = false;
                    break;
                }
            }
            checkmark->setVisible(isCategoryCompleted);
        }

        CCMenuItemSpriteExtra* button = CCMenuItemSpriteExtra::create(
            buttonSprite,
            this,
            menu_selector(AchievementMenu::onCategoryButton));

        button->setID(m_achievementCategories[i].name);
        button->setTag(i);

        menuPage->addChild(button);
    }
}

void AchievementMenu::onCategoryButton(CCObject* sender) {
    CCMenuItemSpriteExtra* categoryButton = static_cast<CCMenuItemSpriteExtra*>(sender);

    int index = categoryButton->getTag();
    Category* category = &m_achievementCategories[index];

    if (category->displayType == "progress") {
        ProgressAchievementPopup* popup = ProgressAchievementPopup::create(this, category);
        popup->m_noElasticity = GameManager::get()->getGameVariable("0168");  // For fast menu setting
        popup->show();
    } else if (category->displayType == "distinct") {
        DistinctAchievementPopup* popup = DistinctAchievementPopup::create(this, category);
        popup->m_noElasticity = GameManager::get()->getGameVariable("0168");  // For fast menu setting
        popup->show();
    } else if (category->displayType == "shard") {
        ShardAchievementPopup* popup = ShardAchievementPopup::create(this, category);
        popup->m_noElasticity = GameManager::get()->getGameVariable("0168");  // For fast menu setting
        popup->show();
    } else if (category->displayType == "path") {
        PathAchievementPopup* popup = PathAchievementPopup::create(this, category);
        popup->m_noElasticity = GameManager::get()->getGameVariable("0168");  // For fast menu setting
        popup->show();
    }

    hideArrows();
}

void AchievementMenu::addNavigation() {
    if (m_categoriesMenu.size() <= 1) return;

    m_navMenu = CCMenu::create();
    m_navMenu->setID("nav-menu");
    m_navMenu->setContentSize(m_mainLayer->getContentSize());
    m_navMenu->setPosition({0.f, 0.f});

    m_navButtons = CCMenu::create();
    m_navButtons->setPosition({m_navMenu->getContentWidth() / 2, 20.f});
    m_navButtons->setContentSize({100.f, 20.f});
    m_navButtons->setID("nav-buttons");
    m_navButtons->setLayout(RowLayout::create()
                                ->setGap(10.0f)
                                ->setAutoScale(false)
                                ->setAxisAlignment(AxisAlignment::Center));
    m_navMenu->addChild(m_navButtons);
    m_mainLayer->addChild(m_navMenu);

    // navigation buttons
    for (int i = 0; i < m_categoriesMenu.size(); i++) {
        CCMenuItemSpriteExtra* button = CCMenuItemSpriteExtra::create(
            i == 0 ? CCSprite::createWithSpriteFrameName("gj_navDotBtn_on_001.png") : CCSprite::createWithSpriteFrameName("gj_navDotBtn_off_001.png"),
            this,
            menu_selector(AchievementMenu::onNavButton));
        button->setID("page-button-" + std::to_string(i));
        button->setTag(i);
        button->setScale(0.8f);
        button->m_baseScale = 0.8f;
        m_navButtons->addChild(button);
    }
    m_navButtons->updateLayout();

    // navigation arrows
    CCSprite* leftArrowSprite = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    CCMenuItemSpriteExtra* leftArrow = CCMenuItemSpriteExtra::create(
        leftArrowSprite,
        this,
        menu_selector(AchievementMenu::onCategoryArrow));
    if (m_mainLayer->getPositionX() - m_mainLayer->getContentWidth() / 2 > 40)
        leftArrow->setPosition({-30.f, m_navMenu->getContentHeight() / 2});
    else
        leftArrow->setPosition({-16.f, m_navMenu->getContentHeight() / 2});
    leftArrow->setTag(0);
    leftArrow->setID("left-arrow");
    m_navMenu->addChild(leftArrow);
    leftArrow->setVisible(false);

    CCSprite* rightArrowSprite = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    rightArrowSprite->setFlipX(true);
    CCMenuItemSpriteExtra* rightArrow = CCMenuItemSpriteExtra::create(
        rightArrowSprite,
        this,
        menu_selector(AchievementMenu::onCategoryArrow));
    if (m_mainLayer->getPositionX() - m_mainLayer->getContentWidth() / 2 > 40)
        rightArrow->setPosition({m_navMenu->getContentWidth() + 30.f, m_navMenu->getContentHeight() / 2});
    else
        rightArrow->setPosition({m_navMenu->getContentWidth() + 6.f, m_navMenu->getContentHeight() / 2});
    rightArrow->setTag(1);
    rightArrow->setID("right-arrow");
    m_navMenu->addChild(rightArrow);
}

void AchievementMenu::onCategoryArrow(CCObject* sender) {
    CCMenuItemSpriteExtra* button = static_cast<CCMenuItemSpriteExtra*>(sender);

    int arrowType = button->getTag();
    if (arrowType == 0)
        m_categoryPage--;
    else if (arrowType == 1)
        m_categoryPage++;

    // page visibility
    for (auto menu : m_categoriesMenu) {
        menu->setVisible(menu->getTag() == m_categoryPage);
    }

    // navigation buttons
    for (int i = 0; i < m_categoriesMenu.size(); i++) {
        CCMenuItemSpriteExtra* navButton = static_cast<CCMenuItemSpriteExtra*>(m_navButtons->getChildByID("page-button-" + std::to_string(i)));
        if (navButton) {
            navButton->setNormalImage(i == m_categoryPage ? CCSprite::createWithSpriteFrameName("gj_navDotBtn_on_001.png") : CCSprite::createWithSpriteFrameName("gj_navDotBtn_off_001.png"));
        }
    }

    // arrow visibility
    m_navMenu->getChildByID("left-arrow")->setVisible(m_categoryPage > 0);
    m_navMenu->getChildByID("right-arrow")->setVisible(m_categoryPage < m_categoriesMenu.size() - 1);
}

void AchievementMenu::onNavButton(CCObject* sender) {
    CCMenuItemSpriteExtra* button = static_cast<CCMenuItemSpriteExtra*>(sender);
    m_categoryPage = button->getTag();

    // page visibility
    for (auto menu : m_categoriesMenu) {
        menu->setVisible(menu->getTag() == m_categoryPage);
    }

    // navigation buttons
    for (int i = 0; i < m_categoriesMenu.size(); i++) {
        CCMenuItemSpriteExtra* navButton = static_cast<CCMenuItemSpriteExtra*>(m_navButtons->getChildByID("page-button-" + std::to_string(i)));
        if (navButton) {
            navButton->setNormalImage(i == m_categoryPage ? CCSprite::createWithSpriteFrameName("gj_navDotBtn_on_001.png") : CCSprite::createWithSpriteFrameName("gj_navDotBtn_off_001.png"));
        }
    }

    // arrow visibility
    m_navMenu->getChildByID("left-arrow")->setVisible(m_categoryPage > 0);
    m_navMenu->getChildByID("right-arrow")->setVisible(m_categoryPage < m_categoriesMenu.size() - 1);
}

void AchievementMenu::hideArrows() {
    m_navMenu->getChildByID("left-arrow")->setVisible(false);
    m_navMenu->getChildByID("right-arrow")->setVisible(false);
}

void AchievementMenu::showArrows() {
    m_navMenu->getChildByID("left-arrow")->setVisible(m_categoryPage > 0);
    m_navMenu->getChildByID("right-arrow")->setVisible(m_categoryPage < m_categoriesMenu.size() - 1);
}

UnlockType AchievementMenu::unlockTypeFromString(const std::string& str) {
    if (str == "icon") {
        return UnlockType::Cube;
    } else if (str == "ship") {
        return UnlockType::Ship;
    } else if (str == "ball") {
        return UnlockType::Ball;
    } else if (str == "bird") {
        return UnlockType::Bird;
    } else if (str == "dart") {
        return UnlockType::Dart;
    } else if (str == "robot") {
        return UnlockType::Robot;
    } else if (str == "spider") {
        return UnlockType::Spider;
    } else if (str == "special") {
        return UnlockType::Streak;
    } else if (str == "death") {
        return UnlockType::Death;
    } else if (str == "swing") {
        return UnlockType::Swing;
    } else if (str == "jetpack") {
        return UnlockType::Jetpack;
    } else if (str == "shipfire") {
        return UnlockType::ShipFire;
    } else if (str == "color") {
        return UnlockType::Col1;
    } else if (str == "color2") {
        return UnlockType::Col2;
    }

    log::debug("Unknown unlock type: {}", str);
    return UnlockType::GJItem;
}

Category* AchievementMenu::getCategoryForAchievement(const std::string& id, const std::string& achievedDescription) {
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

    if (generic.find("shard") != std::string::npos)
        return &m_achievementCategories[19];  // Shards
    if (generic == "path##")
        return &m_achievementCategories[20];  // Paths

    // 'Players destroyed' achievements are grouped in with the secret achievements, so take those out based on their unlock description
    // Additionally, some 'Vault' achievements are grouped in with the secret achievements
    for (Category& cat : m_achievementCategories) {
        for (std::string id : cat.identifiers) {
            if (generic == id) {
                if (cat.name == "Secret" && achievedDescription.find("Destroyed") != std::string::npos)
                    return &m_achievementCategories[27];  // Players Destroyed

                if (cat.name == "Secret" && achievedDescription.find("Vault") != std::string::npos)
                    return &m_achievementCategories[26];  // Vaults

                return &cat;
            }
        }
    }

    return nullptr;
}

void AchievementMenu::addCornerSprites() {
    CCNode* cornerContainer = CCNode::create();
    cornerContainer->setID("corners");
    cornerContainer->setContentSize(m_mainLayer->getContentSize());
    cornerContainer->setPosition({0.f, 0.f});
    m_mainLayer->addChild(cornerContainer);

    CCSprite* blCornerSprite = CCSprite::createWithSpriteFrameName("dailyLevelCorner_001.png");
    blCornerSprite->setID("bottom-left");
    blCornerSprite->setAnchorPoint({0.f, 0.f});
    blCornerSprite->setPosition({0.f, 0.f});
    cornerContainer->addChild(blCornerSprite);

    CCSprite* brCornerSprite = CCSprite::createWithSpriteFrameName("dailyLevelCorner_001.png");
    brCornerSprite->setID("bottom-right");
    brCornerSprite->setAnchorPoint({1.f, 0.f});
    brCornerSprite->setPosition({cornerContainer->getContentWidth(), 0.f});
    brCornerSprite->setFlipX(true);
    cornerContainer->addChild(brCornerSprite);

    CCSprite* tlCornerSprite = CCSprite::createWithSpriteFrameName("dailyLevelCorner_001.png");
    tlCornerSprite->setID("top-left");
    tlCornerSprite->setAnchorPoint({0.f, 1.f});
    tlCornerSprite->setPosition({0.f, cornerContainer->getContentHeight()});
    tlCornerSprite->setFlipY(true);
    cornerContainer->addChild(tlCornerSprite);

    CCSprite* trCornerSprite = CCSprite::createWithSpriteFrameName("dailyLevelCorner_001.png");
    trCornerSprite->setID("top-right");
    trCornerSprite->setAnchorPoint({1.f, 1.f});
    trCornerSprite->setPosition({cornerContainer->getContentWidth(), cornerContainer->getContentHeight()});
    trCornerSprite->setFlipX(true);
    trCornerSprite->setFlipY(true);
    cornerContainer->addChild(trCornerSprite);
}