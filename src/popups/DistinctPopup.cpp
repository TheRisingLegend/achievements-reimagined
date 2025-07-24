#include "DistinctPopup.hpp"

#include <unordered_map>

using namespace geode::prelude;

DistinctPopup* DistinctPopup::create(AchievementMenu* achievementMenu, Category* category) {
    auto popup = new DistinctPopup();
    if (popup && popup->initAnchored(450.f, 280.f, achievementMenu, category)) {
        popup->autorelease();
        return popup;
    }
    delete popup;
    return nullptr;
}

bool DistinctPopup::setup(AchievementMenu* achievementMenu, Category* category) {
    m_achievementMenu = achievementMenu;
    m_category = category;
    m_numAchievements = m_category->achievements.size();

    if (m_category->name == "Creator") {
        std::unordered_map<std::string, int> creatorAchievementsOrder = {
            {"geometry.ach.submit", 0},
            {"geometry.ach.creator03", 1},
            {"geometry.ach.creator01", 2},
            {"geometry.ach.creator04", 3},
            {"geometry.ach.creator05", 4},
            {"geometry.ach.creator06", 5},
            {"geometry.ach.creator02", 6}};

        std::sort(m_category->achievements.begin(), m_category->achievements.end(), [&](Achievement* a, Achievement* b) {
            return creatorAchievementsOrder[a->id] < creatorAchievementsOrder[b->id];
        });
    }

    createTitle();

    // Progress percent and fraction
    int numAchievementsComplete = 0;
    for (Achievement* ach : m_category->achievements) {
        if (achievementManager->isAchievementEarned(ach->id.c_str())) {
            numAchievementsComplete++;
        }
    }
    int percentComplete = 100 * float(numAchievementsComplete) / m_numAchievements;

    addProgressText(percentComplete, numAchievementsComplete, m_numAchievements);

    addCornerSprites();

    // create pages
    m_maxIconsPerPage = 10;
    m_numPages = (m_numAchievements + m_maxIconsPerPage - 1) / m_maxIconsPerPage;
    for (int i = 0; i < m_numPages; i++) {
        CCNode* page = createPage(i);
        page->setID("page-" + std::to_string(i));
        page->setTag(i);
        page->retain();

        m_mainLayer->addChild(page);
        page->setVisible(i == 0);
    }

    addNavigation();

    return true;
}

CCNode* DistinctPopup::createPage(int pageNum) {
    auto page = CCNode::create();
    page->setPosition({0, 0});

    int numIconsOnPage = std::min(m_maxIconsPerPage, m_numAchievements - m_maxIconsPerPage * pageNum);
    bool usePlayerColors = Mod::get()->getSettingValue<bool>("use-player-colors");

    CCMenu* tiles = CCMenu::create();
    tiles->setPosition({m_mainLayer->getContentWidth() / 2, m_mainLayer->getContentHeight() / 2 - 22});
    tiles->setContentSize({80 * float(m_maxIconsPerPage) / 2 + 50, m_mainLayer->getContentHeight() - 100});
    tiles->setID("tiles");
    tiles->setLayout(RowLayout::create()
                         ->setAutoScale(false)
                         ->setGrowCrossAxis(true)
                         ->setAxisAlignment(AxisAlignment::Center)
                         ->setCrossAxisAlignment(AxisAlignment::Start));
    page->addChild(tiles);

    for (int i = 0; i < numIconsOnPage; i++) {
        CCMenu* tile = CCMenu::create();
        tile->setContentSize({80, 80});
        tile->setID("tile-" + std::to_string(i));
        tiles->addChild(tile);

        Achievement* currAchievement = m_category->achievements[i + pageNum * m_maxIconsPerPage];

        /* Background */
        CCScale9Sprite* bg = CCScale9Sprite::create("square02b_001.png");
        bg->setID("bg");
        bg->setContentSize(tile->getContentSize());
        bg->setPosition({tile->getContentWidth() / 2, tile->getContentHeight() / 2});
        bg->setColor({116, 56, 29});
        tile->addChild(bg);

        /* Lock */
        bool earned = achievementManager->isAchievementEarned(currAchievement->id.c_str());
        CCSprite* lock = CCSprite::createWithSpriteFrameName("GJ_lock_001.png");
        lock->setID("lock");
        lock->setZOrder(1);

        /* Unlock description */
        SimpleTextArea* desc = SimpleTextArea::create(earned ? currAchievement->achievedDescription : currAchievement->unachievedDescription, "bigFont.fnt", 0.2f, 70);
        desc->setAlignment(kCCTextAlignmentCenter);
        desc->setWrappingMode(WrappingMode::SPACE_WRAP);
        desc->setAnchorPoint({0.5f, 1});
        desc->setID("desc");
        desc->setPosition({tile->getContentWidth() / 2, 40});
        tile->addChild(desc);

        /* Completion checkmark */
        if (!Mod::get()->getSettingValue<bool>("hide-achievement-checkmarks")) {
            CCSprite* checkmark = CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png");
            checkmark->setID("checkmark");
            checkmark->setPosition({bg->getContentWidth() - 10.f, bg->getContentHeight() - 10.f});
            checkmark->setZOrder(1);
            checkmark->setVisible(achievementManager->isAchievementEarned(currAchievement->id.c_str()));
            bg->addChild(checkmark);
        }

        /* Player sprite */
        GJItemIcon* unlockItem;

        if (currAchievement->unlockID == -1) {  // coin
            CCSprite* coinSprite = CCSprite::createWithSpriteFrameName("secretCoinUI_001.png");
            coinSprite->setID("coin");
            coinSprite->setScale(0.7f);
            coinSprite->setPosition({tile->getContentWidth() / 2, tile->getContentHeight() / 2 + 15});

            if (!earned) {
                lock->setPosition({coinSprite->getContentWidth() / 2.f, coinSprite->getContentHeight() / 2.f});
                coinSprite->addChild(lock);
            }

            tile->addChild(coinSprite);
            continue;
        } else if (earned) {
            std::vector<UnlockType> playerUnlockTypes = {UnlockType::Cube, UnlockType::Ship, UnlockType::Ball, UnlockType::Bird, UnlockType::Dart, UnlockType::Robot, UnlockType::Spider, UnlockType::Swing, UnlockType::Jetpack};
            bool isIcon = std::find(playerUnlockTypes.begin(), playerUnlockTypes.end(), currAchievement->unlockType) != playerUnlockTypes.end();

            if (usePlayerColors) {
                unlockItem = GJItemIcon::create(currAchievement->unlockType, currAchievement->unlockID, gameManager->colorForIdx(gameManager->getPlayerColor()), gameManager->colorForIdx(gameManager->getPlayerColor2()), isIcon, false, false, gameManager->colorForIdx(gameManager->getPlayerGlowColor()));  // p4: is icon?, p5: idk, p6: hide color number? ----- Not sure how to turn on glow, so doing that separately below

                if (gameManager->m_playerGlow) {
                    CCObject* child;
                    CCARRAY_FOREACH(unlockItem->getChildren(), child) {
                        if (auto spr = typeinfo_cast<SimplePlayer*>(child)) {
                            spr->setGlowOutline(gameManager->colorForIdx(gameManager->getPlayerGlowColor()));
                        }
                    }
                }
            } else {
                unlockItem = GJItemIcon::create(currAchievement->unlockType, currAchievement->unlockID, {175, 175, 175}, {255, 255, 255}, isIcon, false, false, {255, 255, 255});
            }
        } else {
            unlockItem = GJItemIcon::createBrowserItem(currAchievement->unlockType, currAchievement->unlockID);

            lock->setPosition({unlockItem->getContentWidth() / 2.f, unlockItem->getContentHeight() / 2.f});
            unlockItem->addChild(lock);
        }
        unlockItem->setID("item-" + std::to_string(i));

        CCMenuItemSpriteExtra* unlockButton = CCMenuItemSpriteExtra::create(
            unlockItem,
            this,
            menu_selector(DistinctPopup::onIcon));

        // This is for the callback function
        IconCallbackData* data = new IconCallbackData(currAchievement->unlockType, currAchievement->unlockID, currAchievement->achievedDescription);
        data->autorelease();
        unlockButton->setUserObject(data);

        unlockButton->setID("unlock-sprite");
        unlockButton->setPosition({tile->getContentWidth() / 2, tile->getContentHeight() / 2 + 15});
        unlockButton->m_baseScale = 0.7f;
        unlockButton->setScale(0.7f);
        tile->addChild(unlockButton);
    }

    tiles->updateLayout();

    return page;
}