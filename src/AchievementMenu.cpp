#include "AchievementMenu.hpp"

#include "popups/DistinctPopup.hpp"
#include "popups/PathPopup.hpp"
#include "popups/ProgressPopup.hpp"
#include "popups/ShardPopup.hpp"

using namespace geode::prelude;

bool AchievementMenu::init() {
    if (!Popup::init(450.f, 280.f))
        return false;

    m_achievementManager = AchievementManager::sharedState();

    m_achievementCategories = {
        // Levels
        {"Main Levels", "Main\nLevels", "Levels", "distinct", "main_levels.png"_spr, {"level##a", "level##b", "demoncoin##", "special##"}},
        {"Tower Levels", "Tower\nLevels", "Levels", "distinct", "tower_levels.png"_spr, {"tower##", "tower##Coin"}},
        {"User Levels", "User\nLevels", "Levels", "progress", "user_levels.png"_spr, {"custom##"}, "4"},
        {"Geometry Dash Meltdown", "Meltdown", "Levels", "distinct", "meltdown.png"_spr, {"mdlevel##b", "mdcoin##", "mdrate"}},
        {"Geometry Dash World", "World", "Levels", "distinct", "world.png"_spr, {"world"}},
        {"Geometry Dash Subzero", "Subzero", "Levels", "distinct", "subzero.png"_spr, {"subzero"}},
        {"Demons", "Demons", "Levels", "progress", "demon.png"_spr, {"demon##"}, "5"},
        {"Insanes", "Insanes", "Levels", "progress", "insane.png"_spr, {"insane##"}, "42"},
        {"Daily Levels", "Daily\nLevels", "Levels", "progress", "daily.png"_spr, {"daily##"}, "15"},
        {"Map Packs", "Map\nPacks", "Levels", "progress", "map_packs.png"_spr, {"mappacks##"}, "7"},
        {"Gauntlets", "Gauntlets", "Levels", "progress", "gauntlets.png"_spr, {"gauntlets##"}, "40"},
        {"Lists", "Lists", "Levels", "progress", "lists.png"_spr, {"lists##"}, "41"},

        // Stats
        {"Stars", "Stars", "Stats", "progress", "GJ_bigStar_noShadow_001.png", {"stars##"}, "6"},
        {"Moons", "Moons", "Stats", "progress", "GJ_bigMoon_noShadow_001.png", {"moons##"}, "28"},
        {"Diamonds", "Diamonds", "Stats", "progress", "GJ_bigDiamond_noShadow_001.png", {"diamonds##"}, "13"},
        {"Secret Coins", "Secret\nCoins", "Stats", "progress", "secretCoin_01_001.png", {"coins##"}, "8"},
        {"User Coins", "User\nCoins", "Stats", "progress", "secretCoin_2_01_001.png", {"usercoins##"}, "12"},
        {"Jumps", "Jumps", "Stats", "progress", "jumps.png"_spr, {"jump##"}, "1"},
        {"Attempts", "Attempts", "Stats", "progress", "attempts.png"_spr, {"attempt##"}, "2"},
        {"Shards", "Shards", "Stats", "shard", "shards.png"_spr, {}},
        {"Paths", "Paths", "Stats", "path", "paths.png"_spr, {}},

        // Other
        {"Liked/Disliked Levels", "Liked /\nDisliked\nLevels", "Other", "progress", "like_dislike.png"_spr, {"like", "like##", "like##b"}, "10"},
        {"Rated Levels", "Rated\nLevels", "Other", "progress", "rate.png"_spr, {"rateDiff", "rateDiff##", "rateDiff##b"}, "11"},
        {"Followed Creators", "Followed\nCreators", "Other", "progress", "followed_creators.png"_spr, {"followCreator", "followCreator##"}},
        {"Friends", "Friends", "Other", "distinct", "friends.png"_spr, {"friends##"}},
        {"Creator", "Creator", "Other", "distinct", "GJ_hammerIcon_001.png", {"creator##", "submit"}},
        {"Vaults", "Vaults", "Other", "distinct", "vaults.png"_spr, {"v#"}},
        {"Players Destroyed", "Players\nDestroyed", "Other", "progress", "players_destroyed.png"_spr, {}, "9"},
        {"Secret", "Secret", "Other", "distinct", "secret.png"_spr, {"secret##", "secret##b"}},
        {"Misc", "Misc", "Other", "distinct", "misc.png"_spr, {"rate", "moreGames", "facebook", "youtube", "twitter"}},
        {"Steam Exclusive", "Steam\nExclusive", "Other", "distinct", "steam.png"_spr, {"steam##"}}};

    setTitle("Achievements", "goldFont.fnt", 1.0f, 15.f);

    addCornerSprites();

    // Parse through achievements and assign them to a category
    CCArray* array = m_achievementManager->m_allAchievements;
    for (int i = 0; i < array->count(); i++) {
        auto item = array->objectAtIndex(i);

        auto dict = typeinfo_cast<CCDictionary*>(item);
        if (!dict) {
            log::debug("Unexpected achievement item type at index {}: {}", i, typeid(*item).name());
            continue;
        }

        Achievement* ach = new Achievement();

        ach->title = std::string(dict->valueForKey("title")->getCString());
        ach->id = std::string(dict->valueForKey("identifier")->getCString());

        Category* category = getCategoryForAchievement(ach->id, betterDescriptions.contains(ach->id) ? std::get<1>(betterDescriptions[ach->id]) : std::string(dict->valueForKey("achievedDescription")->getCString()));
        if (category == nullptr) {
            log::debug("Category not found for ID: {}", ach->id);
            continue;
        }

        if (betterDescriptions.contains(ach->id) && (category->name == "Players Destroyed" || Mod::get()->getSettingValue<bool>("show-secrets"))) {
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

    auto SFC = CCSpriteFrameCache::get();
    SFC->addSpriteFramesWithFile("TowerSheet.plist");

    if (Mod::get()->getSettingValue<bool>("summary-page-first")) {
        createSummaryPage();
        createCategoryMenu();
    } else {
        createCategoryMenu();
        createSummaryPage();
    }

    addNavigation();

    return true;
}

AchievementMenu* AchievementMenu::create() {
    auto popup = new AchievementMenu();
    if (popup && popup->init()) {
        popup->autorelease();
        return popup;
    }
    delete popup;
    return nullptr;
}

void AchievementMenu::createCategoryMenu() {
    std::vector<std::string> pageTitles = {"Levels", "Stats", "Other"};
    for (int i = 0; i < pageTitles.size(); i++) {
        auto menuPage = CCNode::create();
        menuPage->setID("page-" + std::to_string(m_categoriesMenu.size()));
        menuPage->setTag(m_categoriesMenu.size());
        menuPage->setContentSize({m_mainLayer->getContentWidth(), m_mainLayer->getContentHeight() - 70.f});
        menuPage->setPosition({0, 0});
        menuPage->setVisible(m_categoriesMenu.size() == m_categoryPage);
        m_mainLayer->addChild(menuPage);

        auto subTitle = CCLabelBMFont::create(pageTitles[i].c_str(), "bigFont.fnt");
        subTitle->setID("page-subtitle");
        subTitle->setScale(0.5f);
        subTitle->setPosition({menuPage->getContentWidth() / 2, 247});
        menuPage->addChild(subTitle);

        auto buttonMenu = CCMenu::create();
        buttonMenu->setID("categories-menu");
        buttonMenu->setContentSize({m_mainLayer->getContentWidth() - 70, m_mainLayer->getContentHeight() - 70.f});
        buttonMenu->setPosition({m_mainLayer->getContentWidth() / 2, m_mainLayer->getContentHeight() / 2 - 8.f});
        buttonMenu->setLayout(RowLayout::create()
                                  ->setGap(12.f)
                                  ->setAxisAlignment(AxisAlignment::Center)
                                  ->setCrossAxisAlignment(AxisAlignment::Even)
                                  ->setGrowCrossAxis(true));
        menuPage->addChild(buttonMenu);

        int totalAchievementsInPage = 0;
        int completedAchievementsInPage = 0;
        addCategoryButtons(buttonMenu, pageTitles[i], totalAchievementsInPage, completedAchievementsInPage);
        buttonMenu->updateLayout();

        CCNode* progressText = createFractionLabel(completedAchievementsInPage, totalAchievementsInPage);
        progressText->setID("page-progress-fraction");
        progressText->setPosition({menuPage->getContentWidth() / 2, 235});
        menuPage->addChild(progressText);

        m_categoriesMenu.push_back(menuPage);
    }
}

void AchievementMenu::createSummaryPage() {
    auto summaryPage = CCNode::create();
    summaryPage->setID("page-summary");
    summaryPage->setTag(m_categoriesMenu.size());
    summaryPage->setContentSize({m_mainLayer->getContentWidth(), m_mainLayer->getContentHeight() - 70.f});
    summaryPage->setPosition({0, 0});
    summaryPage->setVisible(m_categoriesMenu.size() == m_categoryPage);
    m_mainLayer->addChild(summaryPage);

    auto subTitle = CCLabelBMFont::create("Summary", "bigFont.fnt");
    subTitle->setID("page-subtitle");
    subTitle->setScale(0.5f);
    subTitle->setPosition({summaryPage->getContentWidth() / 2, 247});
    summaryPage->addChild(subTitle);

    std::vector<std::tuple<std::string, std::string, std::string>> summaryTiles = {
        {"geometry-dash", "Geometry Dash", "geometry_dash.png"_spr},
        {"geometry-dash-meltdown", "Geometry Dash Meltdown", "meltdown.png"_spr},
        {"geometry-dash-world", "Geometry Dash World", "world.png"_spr},
        {"geometry-dash-subzero", "Geometry Dash Subzero", "subzero.png"_spr}};

    CCMenu* tiles = CCMenu::create();
    tiles->setPosition({m_mainLayer->getContentWidth() / 2, m_mainLayer->getContentHeight() / 2});
    tiles->setContentSize({80 * float(summaryTiles.size()) / 2 + 50, m_mainLayer->getContentHeight() - 100});
    tiles->setID("tiles");
    tiles->setLayout(RowLayout::create()
                         ->setAutoScale(false)
                         ->setGrowCrossAxis(true)
                         ->setAxisAlignment(AxisAlignment::Center)
                         ->setCrossAxisAlignment(AxisAlignment::Start)
                         ->setGap(12.f));
    summaryPage->addChild(tiles);

    for (int i = 0; i < summaryTiles.size(); i++) {
        CCMenu* tile = CCMenu::create();
        tile->setContentSize({80, 80});
        tile->setID("tile-" + std::get<0>(summaryTiles[i]));
        tiles->addChild(tile);

        /* Background */
        CCScale9Sprite* bg = CCScale9Sprite::create("square02b_001.png");
        bg->setID("bg");
        bg->setContentSize(tile->getContentSize());
        bg->setPosition({tile->getContentWidth() / 2, tile->getContentHeight() / 2});
        bg->setColor({116, 56, 29});
        tile->addChild(bg);

        /* Description */
        SimpleTextArea* desc = SimpleTextArea::create(std::get<1>(summaryTiles[i]), "bigFont.fnt", 0.2f, 70);
        desc->setAlignment(kCCTextAlignmentCenter);
        desc->setWrappingMode(WrappingMode::SPACE_WRAP);
        desc->setAnchorPoint({0.5f, 0.5f});
        desc->setID("desc");
        desc->setPosition({tile->getContentWidth() / 2, tile->getContentHeight() - 10});
        tile->addChild(desc);

        /* Logo sprite */
        CCSprite* logoSprite = CCSprite::create(std::get<2>(summaryTiles[i]).c_str());
        logoSprite->setID("logo-" + std::get<0>(summaryTiles[i]));
        logoSprite->setScale(0.5f);
        logoSprite->setPosition({tile->getContentWidth() / 2, tile->getContentHeight() / 2});

        tile->addChild(logoSprite);

        /* Progress fraction */
        int total = 0;
        int completed = 0;
        if (std::get<1>(summaryTiles[i]) == "Geometry Dash") {
            for (auto category : m_achievementCategories) {
                if (category.name == "Geometry Dash Meltdown" || category.name == "Geometry Dash World" || category.name == "Geometry Dash Subzero") {
                    continue;
                }
                for (auto achievement : category.achievements) {
                    total++;
                    if (achievementManager->isAchievementEarned(achievement->id.c_str())) {
                        completed++;
                    }
                }
            }
        } else {
            for (auto category : m_achievementCategories) {
                if (category.name == std::get<1>(summaryTiles[i])) {
                    for (auto achievement : category.achievements) {
                        total++;
                        if (achievementManager->isAchievementEarned(achievement->id.c_str())) {
                            completed++;
                        }
                    }
                }
            }
        }
        CCNode* progressText = createFractionLabel(completed, total);
        progressText->setID("progress-" + std::get<0>(summaryTiles[i]));
        progressText->setPosition({tile->getContentWidth() / 2, 10});
        tile->addChild(progressText);
    }

    tiles->updateLayout();

    m_categoriesMenu.push_back(summaryPage);
}

void AchievementMenu::addCategoryButtons(CCMenu* menuPage, std::string pageTitle, int& totalAchievementsInPage, int& completedAchievementsInPage) {
    for (int i = 0; i < m_achievementCategories.size(); i++) {
        if (m_achievementCategories[i].page != pageTitle) continue;

        int totalAchievementsInCategory = m_achievementCategories[i].achievements.size();
        totalAchievementsInPage += totalAchievementsInCategory;

        int completedAchievementsInCategory = 0;

        ButtonSprite* buttonSprite = ButtonSprite::create(m_achievementCategories[i].formattedName.c_str(), 90.f, true, "bigFont.fnt", "GJ_button_01.png", 40.f, 0.35f);

        // slide over the text to make room for the logo
        buttonSprite->m_label->setPositionX(buttonSprite->m_label->getContentWidth() / 2 * buttonSprite->m_label->getScale() + 37.f);
        buttonSprite->m_label->setPositionY(buttonSprite->m_label->getPositionY() - 1.f);

        // The little logo on the left side of the button
        const std::vector<std::string> fromSpritesheet = {"Stars", "Moons", "Diamonds", "Secret Coins", "User Coins", "Creator"};

        CCSprite* logo;
        if (std::find(fromSpritesheet.begin(), fromSpritesheet.end(), m_achievementCategories[i].name) != fromSpritesheet.end())
            logo = CCSprite::createWithSpriteFrameName(m_achievementCategories[i].logo.c_str());  // try from spritesheet
        else
            logo = CCSprite::create(m_achievementCategories[i].logo.c_str());  // otherwise try mod resources from logos/

        if (!logo) {
            log::error("Failed to load logo for category: {}", m_achievementCategories[i].name);
        } else {
            logo->setID("logo");
            logo->setAnchorPoint({0.5f, 0.5f});
            logo->setScale(std::min(20.f / logo->getContentWidth(), 20.f / logo->getContentHeight()));
            logo->setPosition({20.f, buttonSprite->getContentHeight() / 2});
            logo->setZOrder(1);
            buttonSprite->addChild(logo);

            if (m_achievementCategories[i].name == "Jumps") {
                GJItemIcon* jumpingIcon = GJItemIcon::create(UnlockType::Cube, gameManager->getPlayerFrame(), gameManager->colorForIdx(gameManager->getPlayerColor()), gameManager->colorForIdx(gameManager->getPlayerColor2()), true, false, false, gameManager->colorForIdx(gameManager->getPlayerGlowColor()));

                // Jank way to set glow cause not sure how to do that in the above create
                CCObject* child;
                if (gameManager->m_playerGlow) {
                    CCObject* child;
                    for (auto child : CCArrayExt(jumpingIcon->getChildren())) {
                        if (auto spr = typeinfo_cast<SimplePlayer*>(child)) {
                            spr->setGlowOutline(gameManager->colorForIdx(gameManager->getPlayerGlowColor()));
                        }
                    }
                }

                jumpingIcon->setRotation(50.f);
                jumpingIcon->setPosition({40, 60});
                logo->addChild(jumpingIcon);
            }
        }

        // Calculate some values for the checkmark and progress fraction
        bool isCategoryCompleted = true;
        for (const Achievement* ach : m_achievementCategories[i].achievements) {
            if (!achievementManager->isAchievementEarned(ach->id.c_str())) {
                isCategoryCompleted = false;
            } else {
                completedAchievementsInCategory++;
                completedAchievementsInPage++;
            }
        }
        // Checkmark for completed categories
        CCSprite* checkmark = CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png");
        checkmark->setID("checkmark");
        checkmark->setPosition({buttonSprite->getContentWidth() - 5.f, buttonSprite->getContentHeight() - 5.f});
        checkmark->setZOrder(1);
        buttonSprite->addChild(checkmark);

        checkmark->setVisible(isCategoryCompleted);

        // Progress fraction for progress categories, if enabled
        if (!Mod::get()->getSettingValue<bool>("hide-category-count") && totalAchievementsInCategory > 0) {
            CCNode* completedFraction = createFractionLabel(completedAchievementsInCategory, totalAchievementsInCategory);
            completedFraction->setID("category-progress-fraction");
            completedFraction->setAnchorPoint({0, 0.5f});
            completedFraction->setPosition({4.f, buttonSprite->getContentHeight() - 6.f});
            completedFraction->setScale(0.75f);
            buttonSprite->addChild(completedFraction);
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

    AchievementCategoryPopup* popup;

    if (category->displayType == "distinct" || Mod::get()->getSettingValue<bool>("all-discrete")) {
        popup = DistinctPopup::create(this, category);
    } else if (category->displayType == "progress") {
        popup = ProgressPopup::create(this, category);
    } else if (category->displayType == "shard") {
        popup = ShardPopup::create(this, category);
    } else if (category->displayType == "path") {
        popup = PathPopup::create(this, category);
    }
    popup->m_noElasticity = GameManager::get()->getGameVariable("0168");  // For fast menu setting
    popup->show();

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
    m_navButtons->setContentSize({400.f, 20.f});
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