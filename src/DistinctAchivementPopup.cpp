#include <unordered_map>

#include "DistinctAchievementPopup.hpp"

using namespace geode::prelude;

DistinctAchievementPopup* DistinctAchievementPopup::create(AchievementMenu* achievementMenu, Category* category) {
    auto popup = new DistinctAchievementPopup();
    if (popup && popup->initAnchored(450.f, 280.f, achievementMenu, category)) {
        popup->autorelease();
        return popup;
    }
    delete popup;
    return nullptr;
}

bool DistinctAchievementPopup::setup(AchievementMenu* achievementMenu, Category* category) {
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

CCNode* DistinctAchievementPopup::createPage(int pageNum) {
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
        bool locked = !achievementManager->isAchievementEarned(currAchievement->id.c_str());
        CCSprite* lock = CCSprite::createWithSpriteFrameName("GJ_lockGray_001.png");
        lock->setID("lock");
        lock->setZOrder(1);

        /* Unlock description */
        SimpleTextArea* desc = SimpleTextArea::create(locked ? currAchievement->unachievedDescription : currAchievement->achievedDescription, "bigFont.fnt", 0.2f, 70);
        desc->setAlignment(kCCTextAlignmentCenter);
        desc->setWrappingMode(WrappingMode::SPACE_WRAP);
        desc->setAnchorPoint({0.5f, 1});
        desc->setID("desc");
        desc->setPosition({tile->getContentWidth() / 2, 40});
        tile->addChild(desc);

        /* Player Sprite */
        CCMenuItemSpriteExtra* unlockButton = nullptr;
        GJItemIcon* unlockItem = nullptr;

        std::vector<UnlockType> playerUnlockTypes = {UnlockType::Cube, UnlockType::Ship, UnlockType::Ball, UnlockType::Bird, UnlockType::Dart, UnlockType::Robot, UnlockType::Spider, UnlockType::Swing, UnlockType::Jetpack};
        if (std::find(playerUnlockTypes.begin(), playerUnlockTypes.end(), currAchievement->unlockType) != playerUnlockTypes.end()) {
            SimplePlayer* iconSprite = SimplePlayer::create(1);
            iconSprite->setID("icon");
            iconSprite->updatePlayerFrame(currAchievement->unlockID, gameManager->unlockTypeToIconType(static_cast<int>(currAchievement->unlockType)));

            if (locked) {
                iconSprite->setColors(ccc3(30, 30, 30), ccc3(80, 80, 80));
                iconSprite->disableGlowOutline();
                iconSprite->addChild(lock);
            } else if (usePlayerColors) {
                iconSprite->setColors(gameManager->colorForIdx(gameManager->getPlayerColor()), gameManager->colorForIdx(gameManager->getPlayerColor2()));
                if (gameManager->m_playerGlow) {
                    iconSprite->setGlowOutline(gameManager->colorForIdx(gameManager->getPlayerGlowColor()));
                }
            } else {
                iconSprite->setColors(ccc3(190, 190, 190), ccc3(245, 245, 245));
                iconSprite->disableGlowOutline();
            }

            unlockButton = CCMenuItemSpriteExtra::create(
                iconSprite,
                this,
                menu_selector(DistinctAchievementPopup::onIcon));

            unlockButton->setContentSize({iconSprite->m_firstLayer->getContentWidth(), iconSprite->m_firstLayer->getContentHeight()});
            auto innerSpr = static_cast<cocos2d::CCSprite*>(unlockButton->getNormalImage());
            innerSpr->setPosition({iconSprite->m_firstLayer->getContentWidth() / 2.f, iconSprite->m_firstLayer->getContentHeight() / 2.f});

        } else if (currAchievement->unlockType == UnlockType::Col1 || currAchievement->unlockType == UnlockType::Col2) {
            ColorChannelSprite* colorSprite = ColorChannelSprite::create();
            colorSprite->setID("color");
            colorSprite->setColor(gameManager->colorForIdx(currAchievement->unlockID));

            if (locked) {
                lock->setPosition({17.75f, 18.25f});
                colorSprite->addChild(lock);
            }

            unlockButton = CCMenuItemSpriteExtra::create(
                colorSprite,
                this,
                menu_selector(DistinctAchievementPopup::onIcon));
        } else if (currAchievement->unlockID == -1) {  // coin
            CCSprite* coinSprite = CCSprite::createWithSpriteFrameName("secretCoin_01_001.png");
            coinSprite->setID("coin");
            coinSprite->setScale(0.7f);
            coinSprite->setPosition({tile->getContentWidth() / 2, tile->getContentHeight() / 2 + 15});

            if (locked) {
                lock->setPosition({20, 20});
                coinSprite->addChild(lock);
            }

            tile->addChild(coinSprite);
            continue;
        } else {
            GJItemIcon* itemSprite = GJItemIcon::createBrowserItem(currAchievement->unlockType, currAchievement->unlockID);

            itemSprite->setID("item");

            if (locked) {
                lock->setPosition({15, 15});
                itemSprite->addChild(lock);
            }

            unlockButton = CCMenuItemSpriteExtra::create(
                itemSprite,
                this,
                menu_selector(DistinctAchievementPopup::onIcon));
        }

        // This is for the callback function
        IconCallbackData* data = new IconCallbackData(currAchievement->unlockType, currAchievement->unlockID);
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