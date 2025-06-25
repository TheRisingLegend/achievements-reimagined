#include "ProgressAchievementPopup.hpp"

using namespace geode::prelude;

ProgressAchievementPopup* ProgressAchievementPopup::create(AchievementMenu* achievementMenu, Category* category) {
    auto popup = new ProgressAchievementPopup();
    if (popup && popup->initAnchored(450.f, 280.f, achievementMenu, category)) {
        popup->autorelease();
        return popup;
    }
    delete popup;
    return nullptr;
}

bool ProgressAchievementPopup::setup(AchievementMenu* achievementMenu, Category* category) {
    m_achievementMenu = achievementMenu;
    m_category = category;
    m_numAchievements = m_category->achievements.size();

    // Sort achievements by unlock value
    std::sort(m_category->achievements.begin(), m_category->achievements.end(), [](Achievement* a, Achievement* b) {
        return a->unlockValue < b->unlockValue;
    });

    createTitle();

    // Progress percent and fraction
    int percentComplete = achievementManager->percentForAchievement(m_category->achievements.back()->id.c_str());

    if (m_category->name == "Followed Creators")
        m_statValue = gameLevelManager->m_followedCreators->count();
    else
        m_statValue = m_category->statKey.empty() ? 0 : gameStatsManager->getStat(m_category->statKey.c_str());

    addProgressText(percentComplete, m_statValue, m_category->achievements.back()->unlockValue);

    addCornerSprites();

    // create pages
    m_maxIconsPerPage = 20;
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

CCNode* ProgressAchievementPopup::createPage(int pageNum) {
    auto page = CCNode::create();
    page->setPosition({0, 0});

    int numIconsOnPage = std::min(m_maxIconsPerPage, m_numAchievements - m_maxIconsPerPage * pageNum);

    /* Progress Bar */
    CCNode* progressBar = CCNode::create();
    progressBar->setID("progress-bar-container");
    progressBar->setPosition({m_mainLayer->getContentWidth() / 2, 120.f});
    page->addChild(progressBar);

    // Background (dark)
    CCNode* progressBarBg = CCNode::create();
    progressBarBg->setID("progress-bar-bg");
    progressBarBg->setPosition({0, 0});
    progressBar->addChild(progressBarBg);

    int numDotsOnPage = numIconsOnPage + 1;  // first dot is start and has no icon

    CCSprite* progressBarBgSpr = CCSprite::createWithSpriteFrameName("whiteSquare20_001.png");  // the long gray bar that acts as the background for the fill
    progressBarBgSpr->setID("progress-bar-bg-sprite");
    progressBarBgSpr->setScaleX(2.f * numDotsOnPage - 2.f);
    progressBarBgSpr->setScaleY(0.5f);

    progressBarBgSpr->setPosition({0, 0});
    progressBarBgSpr->setColor({37, 20, 12});
    progressBarBg->addChild(progressBarBgSpr);

    for (int i = 0; i < numDotsOnPage; ++i) {
        CCSprite* dotBgSpr = CCSprite::create("smallDot.png");  // the gray dots that mark each unlock point
        dotBgSpr->setID("dot-bg-sprite-" + std::to_string(i));
        dotBgSpr->setPosition({-20.f * numIconsOnPage / 2.f + 20.f * i, 0});
        dotBgSpr->setColor({37, 20, 12});
        progressBarBg->addChild(dotBgSpr);

        if (i > 0) {
            CCSprite* verticalBarBgSpr = CCSprite::createWithSpriteFrameName("whiteSquare20_001.png");  // the vertical gray bars that connect the icons with the dots
            verticalBarBgSpr->setID("vertical-bar-sprite-" + std::to_string(i));

            if (i % 2 == 0) {  // alternate the vertical bars above and below
                verticalBarBgSpr->setAnchorPoint({0.5f, 1});
                verticalBarBgSpr->setPosition({-20.f * numIconsOnPage / 2.f + 20.f * i, -10.f});
            } else {
                verticalBarBgSpr->setAnchorPoint({0.5f, 0});
                verticalBarBgSpr->setPosition({-20.f * numIconsOnPage / 2.f + 20.f * i, 10.f});
            }

            verticalBarBgSpr->setScaleX(0.1f);
            verticalBarBgSpr->setScaleY(1.5f);
            verticalBarBgSpr->setColor({37, 20, 12});
            verticalBarBgSpr->setOpacity(50);
            progressBarBg->addChild(verticalBarBgSpr);
        }
    }

    // Fill (player color or white)
    bool usePlayerColors = Mod::get()->getSettingValue<bool>("use-player-colors");

    CCNode* progressBarFill = CCNode::create();
    progressBarFill->setID("progress-bar-fill");
    progressBarFill->setPosition({0, 0});
    progressBar->addChild(progressBarFill);

    int numAchievementsUnlocked = 0;
    for (int i = 0; i < m_numAchievements; ++i) {
        if (achievementManager->isAchievementEarned(m_category->achievements[i]->id.c_str())) {
            numAchievementsUnlocked++;
        }
    }

    for (int i = 0; i < numDotsOnPage; ++i) {
        if (m_maxIconsPerPage * pageNum + i > numAchievementsUnlocked) break;

        CCSprite* dotFillSpr = CCSprite::create("smallDot.png");  // the colored dots that mark the reached unlock points
        dotFillSpr->setID("dot-fill-sprite-" + std::to_string(i));
        dotFillSpr->setPosition({-20.f * numIconsOnPage / 2.f + 20.f * i, 0});
        dotFillSpr->setColor(usePlayerColors ? gameManager->colorForIdx(gameManager->getPlayerColor()) : ccc3(255, 255, 255));
        dotFillSpr->setScale(0.7f);
        progressBarFill->addChild(dotFillSpr);
    }

    for (int i = 0; i < numDotsOnPage - 1; ++i) {
        Achievement* currAchievement = m_category->achievements[i + pageNum * m_maxIconsPerPage];

        float ratio;
        if (i + pageNum * m_maxIconsPerPage == 0)
            ratio = std::min(1.f, float(m_statValue) / currAchievement->unlockValue);
        else
            ratio = std::min(1.f, float(m_statValue - m_category->achievements[i - 1 + pageNum * m_maxIconsPerPage]->unlockValue) / (currAchievement->unlockValue - m_category->achievements[i - 1 + pageNum * m_maxIconsPerPage]->unlockValue));

        if (ratio < 0.f) break;

        CCSprite* fillSpr = CCSprite::createWithSpriteFrameName("whiteSquare20_001.png");  // the colored bar that fills up to the next unlock point and marks the progress
        fillSpr->setID("progress-bar-fill-sprite-" + std::to_string(i));
        fillSpr->setAnchorPoint({0, 0.5f});
        fillSpr->setPosition({-20.f * numIconsOnPage / 2.f + 20.f * i, 0});
        fillSpr->setColor(usePlayerColors ? gameManager->colorForIdx(gameManager->getPlayerColor()) : ccc3(255, 255, 255));
        fillSpr->setScaleX(2.f * ratio);
        fillSpr->setScaleY(0.2f);

        progressBarFill->addChild(fillSpr);
    }

    /* Player Sprites */
    CCMenu* playerMenu = CCMenu::create();
    playerMenu->setID("unlocks-container");
    playerMenu->setPosition(progressBar->getPosition());
    page->addChild(playerMenu);

    for (int i = 0; i < numIconsOnPage; ++i) {
        Achievement* currAchievement = m_category->achievements[i + pageNum * m_maxIconsPerPage];

        // Create the lock
        bool locked = !achievementManager->isAchievementEarned(currAchievement->id.c_str());
        CCSprite* lock = CCSprite::createWithSpriteFrameName("GJ_lockGray_001.png");
        lock->setID("lock-" + std::to_string(i));
        lock->setZOrder(1);

        // Create the text that shows how much is needed to unlock
        CCLabelBMFont* unlockValue = CCLabelBMFont::create(formatWithCommas(currAchievement->unlockValue).c_str(), "bigFont.fnt");
        unlockValue->setID("unlock-text-" + std::to_string(i));
        unlockValue->setScale(.25f);

        // Load the icon
        CCMenuItemSpriteExtra* unlockButton = nullptr;
        GJItemIcon* unlockItem = nullptr;

        std::vector<UnlockType> playerUnlockTypes = {UnlockType::Cube, UnlockType::Ship, UnlockType::Ball, UnlockType::Bird, UnlockType::Dart, UnlockType::Robot, UnlockType::Spider, UnlockType::Swing, UnlockType::Jetpack};
        if (std::find(playerUnlockTypes.begin(), playerUnlockTypes.end(), currAchievement->unlockType) != playerUnlockTypes.end()) {
            SimplePlayer* iconSprite = SimplePlayer::create(1);
            iconSprite->setID("icon" + std::to_string(i));
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
                menu_selector(ProgressAchievementPopup::onIcon));

            unlockButton->setContentSize({iconSprite->m_firstLayer->getContentWidth(), iconSprite->m_firstLayer->getContentHeight()});
            auto innerSpr = static_cast<cocos2d::CCSprite*>(unlockButton->getNormalImage());
            innerSpr->setPosition({iconSprite->m_firstLayer->getContentWidth() / 2.f, iconSprite->m_firstLayer->getContentHeight() / 2.f});

        } else if (currAchievement->unlockType == UnlockType::Col1 || currAchievement->unlockType == UnlockType::Col2) {
            ColorChannelSprite* colorSprite = ColorChannelSprite::create();
            colorSprite->setID("color" + std::to_string(i));
            colorSprite->setColor(gameManager->colorForIdx(currAchievement->unlockID));

            if (locked) {
                lock->setPosition({17.75f, 18.25f});
                colorSprite->addChild(lock);
            }

            unlockButton = CCMenuItemSpriteExtra::create(
                colorSprite,
                this,
                menu_selector(ProgressAchievementPopup::onIcon));
        } else {
            GJItemIcon* itemSprite = GJItemIcon::createBrowserItem(currAchievement->unlockType, currAchievement->unlockID);
            itemSprite->setID("item" + std::to_string(i));

            if (locked) {
                lock->setPosition({15, 15});
                itemSprite->addChild(lock);
            }

            unlockButton = CCMenuItemSpriteExtra::create(
                itemSprite,
                this,
                menu_selector(ProgressAchievementPopup::onIcon));
        }

        // This is for the callback function
        IconCallbackData* data = new IconCallbackData(currAchievement->unlockType, currAchievement->unlockID);
        data->autorelease();
        unlockButton->setUserObject(data);

        unlockButton->setID("unlock-sprite-" + std::to_string(i));
        unlockButton->setPosition({-20.f * numIconsOnPage / 2.f + 20.f * (i + 1), (i % 2 == 0) ? 40.f : -40.f});
        unlockButton->m_baseScale = 0.7f;
        unlockButton->setScale(0.7f);
        playerMenu->addChild(unlockButton);

        unlockValue->setPosition({unlockButton->getPositionX(), unlockButton->getPositionY() + (i % 2 == 0 ? 18.f : -16.f)});
        playerMenu->addChild(unlockValue);
    }

    return page;
}
