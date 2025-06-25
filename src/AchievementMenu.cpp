#include "AchievementMenu.hpp"

#include "DistinctAchievementPopup.hpp"
#include "ProgressAchievementPopup.hpp"

using namespace geode::prelude;

bool AchievementMenu::setup() {
    m_achievementManager = AchievementManager::sharedState();

    m_achievementCategories = {
        // Levels
        {"Main Levels", "Main Levels", "Levels", "distinct", {"level##a", "level##b", "demoncoin##", "special##"}},
        {"Tower Levels", "Tower Levels", "Levels", "distinct", {"tower##", "tower##Coin"}},
        {"User Levels", "User Levels", "Levels", "progress", {"custom##"}, "4"},
        {"Meltdown", "Meltdown", "Levels", "distinct", {"mdlevel##b", "mdcoin##", "mdrate"}},
        {"World", "World", "Levels", "distinct", {"world"}},
        {"Subzero", "Subzero", "Levels", "distinct", {"subzero"}},
        {"Demons", "Demons", "Levels", "progress", {"demon##"}, "5"},
        {"Insanes", "Insanes", "Levels", "progress", {"insane##"}, "42"},
        {"Daily Levels", "Daily Levels", "Levels", "progress", {"daily##"}, "15"},
        {"Map Packs", "Map Packs", "Levels", "progress", {"mappacks##"}, "7"},
        {"Gauntlets", "Gauntlets", "Levels", "progress", {"gauntlets##"}, "40"},
        {"Lists", "Lists", "Levels", "progress", {"lists##"}, "41"},

        // Stats
        {"Stars", "Stars", "Stats", "progress", {"stars##"}, "6"},
        {"Moons", "Moons", "Stats", "progress", {"moons##"}, "28"},
        {"Diamonds", "Diamonds", "Stats", "progress", {"diamonds##"}, "13"},
        {"Secret Coins", "Secret Coins", "Stats", "progress", {"coins##"}, "8"},
        {"User Coins", "User Coins", "Stats", "progress", {"usercoins##"}, "12"},
        {"Jumps", "Jumps", "Stats", "progress", {"jump##"}, "1"},
        {"Attempts", "Attempts", "Stats", "progress", {"attempt##"}, "2"},

        // Social
        {"Liked/Disliked Levels", "Liked/Disliked\nLevels", "Social", "progress", {"like", "like##", "like##b"}, "10"},
        {"Rated Levels", "Rated Levels", "Social", "progress", {"rateDiff", "rateDiff##", "rateDiff##b"}, "11"},
        {"Followed Creators", "Followed\nCreators", "Social", "progress", {"followCreator", "followCreator##"}},
        {"Friends", "Friends", "Social", "distinct", {"friends##"}},

        // Other
        {"Creator", "Creator", "Other", "distinct", {"creator##", "submit"}},
        {"Vaults", "Vaults", "Other", "distinct", {"v#"}},
        {"Players Destroyed", "Players\nDestroyed", "Other", "progress", {}, "9"},
        {"Secret", "Secret", "Other", "distinct", {"secret##", "secret##b"}},
        {"Misc", "Misc", "Other", "distinct", {"rate", "moreGames", "facebook", "youtube", "twitter"}},
        {"Steam Exclusive", "Steam\nExclusive", "Other", "distinct", {"steam##"}}};

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

        if (Mod::get()->getSettingValue<bool>("use-better-descriptions") && betterDescriptions.contains(ach->id)) {
            ach->unachievedDescription = std::get<0>(betterDescriptions[ach->id]);
            ach->achievedDescription = std::get<1>(betterDescriptions[ach->id]);

            dict->setObject(CCString::create(ach->unachievedDescription), "unachievedDescription");
            dict->setObject(CCString::create(ach->achievedDescription), "achievedDescription");
        } else {
            ach->achievedDescription = std::string(dict->valueForKey("achievedDescription")->getCString());
            ach->unachievedDescription = std::string(dict->valueForKey("unachievedDescription")->getCString());
        }

        Category* category = getCategoryForAchievement(ach->id, ach->achievedDescription);
        if (category == nullptr) {
            log::debug("Category not found for ID: {}", ach->id);
            continue;
        }

        ach->unlockValue = category->displayType == "progress" ? extractValue(ach->achievedDescription) : -1;

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

        if (!Mod::get()->getSettingValue<bool>("hide-category-checkmarks")) {
            CCSprite* checkmark = CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png");
            checkmark->setID("checkmark");
            checkmark->setPosition({buttonSprite->getContentWidth() - 5.f, buttonSprite->getContentHeight() - 5.f});
            checkmark->setZOrder(1);
            buttonSprite->addChild(checkmark);

            bool isCategoryCompleted = true;
            for (const Achievement* ach : m_achievementCategories[i].achievements) {
                log::debug("{}: {}", ach->id, achievementManager->isAchievementEarned(ach->id.c_str()));
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
    leftArrow->setPosition({-30.f, m_navMenu->getContentHeight() / 2});
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
    rightArrow->setPosition({m_navMenu->getContentWidth() + 30.f, m_navMenu->getContentHeight() / 2});
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

    // Currently don't support path or shards achievements
    // Could just open their vanilla menus when the category is clicked or
    // build own menus for each to match the style of the mod
    if (generic == "path##" || generic.find("shard") != std::string::npos) {
        return nullptr;
    }

    // 'Players destroyed' achievements are grouped in with the secret achievements, so take those out based on their unlock description
    // Additionally, some 'Vault' achievements are grouped in with the secret achievements
    for (Category& cat : m_achievementCategories) {
        for (std::string id : cat.identifiers) {
            if (generic == id) {
                if (cat.name == "Secret" && achievedDescription.find("Destroyed") != std::string::npos)
                    return &m_achievementCategories[25];  // Players Destroyed

                if (cat.name == "Secret" && achievedDescription.find("Vault") != std::string::npos)
                    return &m_achievementCategories[24];  // Vaults

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