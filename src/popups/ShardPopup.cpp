#include "ShardPopup.hpp"

using namespace geode::prelude;

ShardPopup* ShardPopup::create(AchievementMenu* achievementMenu, Category* category) {
    auto popup = new ShardPopup();
    if (popup && popup->init(achievementMenu, category)) {
        popup->autorelease();
        return popup;
    }
    delete popup;
    return nullptr;
}

bool ShardPopup::init(AchievementMenu* achievementMenu, Category* category) {
    if (!Popup::init(450.f, 280.f))
        return false;

    m_achievementMenu = achievementMenu;
    m_category = category;
    m_numAchievements = m_category->achievements.size();

    addCornerSprites();

    // create pages
    m_maxIconsPerPage = 5;
    m_numPages = (m_numAchievements + m_maxIconsPerPage - 1) / m_maxIconsPerPage;
    for (int i = 0; i < m_numPages; i++) {
        CCNode* page = createPage(i);
        page->setID("page-" + std::to_string(i));
        page->setTag(i);
        page->retain();

        m_mainLayer->addChild(page);
        page->setVisible(i == 0);
    }

    addNavigation(0);

    return true;
}

void ShardPopup::addNavigation(int activePage) {
    bool refresh = false;
    if (m_navButtons) {
        m_navButtons->removeAllChildren();
        m_navButtons->removeFromParent();
        refresh = true;
    }

    if (!refresh) {
        m_navMenu = CCMenu::create();
        m_navMenu->setID("nav-menu");
        m_navMenu->setContentSize(m_mainLayer->getContentSize());
        m_navMenu->setPosition({0.f, 0.f});
        m_mainLayer->addChild(m_navMenu);
    }

    m_navButtons = CCMenu::create();
    m_navButtons->setPosition({m_navMenu->getContentWidth() / 2, 20.f});
    m_navButtons->setContentSize({400.f, 20.f});
    m_navButtons->setID("nav-buttons");
    m_navButtons->setLayout(RowLayout::create()
                                ->setGap(0.f)
                                ->setAutoScale(false)
                                ->setAxisAlignment(AxisAlignment::Center));
    m_navMenu->addChild(m_navButtons);

    // navigation buttons
    for (int i = 0; i < m_numPages; i++) {
        CCSprite* onSprite = CCSprite::createWithSpriteFrameName(shardSprites[i]);
        onSprite->setScale(i == 5 || i == 11 ? 1.5f : 0.75f);
        CCSprite* offSprite = CCSprite::createWithSpriteFrameName(shardSprites[i]);
        offSprite->setScale(i == 5 || i == 11 ? 1.f : 0.5f);

        int gameStatID = gameStatIDs[i];
        int progress = 100;
        if (i == 5 || i == 11) {  // bonus pages, these values aren't stored so need to calculate them
            for (int j = i - 5; j < i; ++j) {
                progress = std::min(progress, gameStatsManager->getStat(std::to_string(gameStatIDs[j]).c_str()));
            }
        } else
            progress = gameStatsManager->getStat(std::to_string(gameStatID).c_str());

        bool showCheckmark = !Mod::get()->getSettingValue<bool>("hide-achievement-checkmarks") && progress >= 100;
        if (showCheckmark) {
            CCSprite* checkmark = CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png");
            checkmark->setID("checkmark");
            checkmark->setZOrder(1);
            checkmark->setScale(i == 5 || i == 11 ? 0.5f : 1.f);

            if (i == activePage) {
                checkmark->setPosition({onSprite->getContentWidth() / 2, onSprite->getContentHeight() / 2});
                onSprite->addChild(checkmark);
            } else {
                checkmark->setPosition({offSprite->getContentWidth() / 2, offSprite->getContentHeight() / 2});
                offSprite->addChild(checkmark);
            }
        }

        CCMenuItemSpriteExtra* button = CCMenuItemSpriteExtra::create(
            i == activePage ? onSprite : offSprite,
            this,
            menu_selector(ShardPopup::onNavButton));
        button->setID("page-button-" + std::to_string(i));
        button->setTag(i);
        button->setScale(0.8f);
        button->m_baseScale = 0.8f;
        button->setContentWidth(30.f);
        if (auto sprite = typeinfo_cast<CCSprite*>(button->getChildren()->objectAtIndex(0)))
            sprite->setPositionX(15.f);

        m_navButtons->addChild(button);
    }
    m_navButtons->updateLayout();

    if (refresh) return;

    // navigation arrows
    CCSprite* leftArrowSprite = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    CCMenuItemSpriteExtra* leftArrow = CCMenuItemSpriteExtra::create(
        leftArrowSprite,
        this,
        menu_selector(ShardPopup::onArrow));
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
        menu_selector(ShardPopup::onArrow));
    if (m_mainLayer->getPositionX() - m_mainLayer->getContentWidth() / 2 > 40)
        rightArrow->setPosition({m_navMenu->getContentWidth() + 30.f, m_navMenu->getContentHeight() / 2});
    else
        rightArrow->setPosition({m_navMenu->getContentWidth() + 6.f, m_navMenu->getContentHeight() / 2});
    rightArrow->setTag(1);
    rightArrow->setID("right-arrow");
    m_navMenu->addChild(rightArrow);
}

void ShardPopup::onNavButton(CCObject* sender) {
    CCMenuItemSpriteExtra* button = static_cast<CCMenuItemSpriteExtra*>(sender);
    int pageNum = button->getTag();

    // update page visibility
    for (int i = 0; i < m_numPages; i++) {
        CCNode* page = m_mainLayer->getChildByID("page-" + std::to_string(i));
        if (page) {
            page->setVisible(i == pageNum);
        }
    }

    addNavigation(pageNum);

    // update arrow visibility
    m_navMenu->getChildByID("left-arrow")->setVisible(pageNum > 0);
    m_navMenu->getChildByID("right-arrow")->setVisible(pageNum < m_numPages - 1);
}

void ShardPopup::onArrow(CCObject* sender) {
    CCMenuItemSpriteExtra* button = static_cast<CCMenuItemSpriteExtra*>(sender);
    int direction = button->getTag();

    int currentPage = -1;
    for (int i = 0; i < m_numPages; i++) {
        if (m_mainLayer->getChildByID("page-" + std::to_string(i))->isVisible()) {
            currentPage = i;
            break;
        }
    }

    int newPage = currentPage + (direction == 0 ? -1 : 1);
    if (newPage < 0 || newPage >= m_numPages) return;

    // update page visibility
    for (int i = 0; i < m_numPages; i++) {
        CCNode* page = m_mainLayer->getChildByID("page-" + std::to_string(i));
        if (page) {
            page->setVisible(i == newPage);
        }
    }

    addNavigation(newPage);

    // update arrow visibility
    m_navMenu->getChildByID("left-arrow")->setVisible(newPage > 0);
    m_navMenu->getChildByID("right-arrow")->setVisible(newPage < m_numPages - 1);
}

cocos2d::CCNode* ShardPopup::createPage(int pageNum) {
    auto page = CCNode::create();
    page->setPosition({0, 0});

    /* Path Logo*/
    CCSprite* shardLogo = CCSprite::createWithSpriteFrameName(shardLogos[pageNum]);
    if (!shardLogo) {
        log::error("Failed to load shard logo for page {}", pageNum);
    } else {
        shardLogo->setID("shard-logo");
        shardLogo->setScale(1.2f);
        shardLogo->setPosition({225, 260});
        page->addChild(shardLogo);
    }

    /* Progress Fraction */
    CCNode* container = CCNode::create();
    container->setID("fraction-complete");
    container->setAnchorPoint({0.5f, 0.5f});
    container->setContentSize({100, 20});

    int gameStatID = gameStatIDs[pageNum];
    log::debug("stat ID: {}", gameStatID);
    int progress = 100;
    if (pageNum == 5 || pageNum == 11) {  // bonus pages, these values aren't stored so need to calculate them
        for (int i = pageNum - 5; i < pageNum; ++i) {
            progress = std::min(progress, gameStatsManager->getStat(std::to_string(gameStatIDs[i]).c_str()));
        }
    } else
        progress = gameStatsManager->getStat(std::to_string(gameStatID).c_str());

    CCLabelBMFont* progressLabel = nullptr;
    if (progress < 100) {
        progressLabel = CCLabelBMFont::create(formatWithCommas(progress).c_str(), "bigFont.fnt");
        progressLabel->setScale(0.39f);
        progressLabel->setPosition({container->getContentWidth() / 2 - 2, container->getContentHeight() / 2 - 0.25f});
    } else {
        progressLabel = CCLabelBMFont::create(formatWithCommas(progress).c_str(), "goldFont.fnt");
        progressLabel->setScale(0.5f);
        progressLabel->setPosition({container->getContentWidth() / 2 - 2, container->getContentHeight() / 2});
    }

    CCLabelBMFont* goalLabel = CCLabelBMFont::create("/100", "goldFont.fnt");
    goalLabel->setScale(0.5f);
    goalLabel->setAnchorPoint({0, 0.5f});
    goalLabel->setPosition({container->getContentWidth() / 2 - 3, container->getContentHeight() / 2});
    progressLabel->setAnchorPoint({1, 0.5f});

    CCSprite* shard = CCSprite::createWithSpriteFrameName(shardSprites[pageNum]);
    shard->setID("shard");
    shard->setScale(pageNum == 5 || pageNum == 11 ? 0.75f : 0.5f);
    shard->setPosition({container->getContentWidth() / 2 + 34, container->getContentHeight() / 2 - 1});

    container->addChild(progressLabel);
    container->addChild(goalLabel);
    container->addChild(shard);
    container->setPosition({225, 240});
    page->addChild(container);

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

    int numIconsOnPage = 5;
    int numDotsOnPage = numIconsOnPage + 1;

    float dotSpacing = std::min(50.f, 400.f / numDotsOnPage);

    CCSprite* progressBarBgSpr = CCSprite::createWithSpriteFrameName("whiteSquare20_001.png");  // the long gray bar that acts as the background for the fill
    progressBarBgSpr->setID("progress-bar-bg-sprite");
    progressBarBgSpr->setScaleX(dotSpacing / 10 * numDotsOnPage - dotSpacing / 10);
    progressBarBgSpr->setScaleY(0.5f);

    progressBarBgSpr->setPosition({0, 0});
    progressBarBgSpr->setColor({37, 20, 12});
    progressBarBg->addChild(progressBarBgSpr);

    for (int i = 0; i < numDotsOnPage; ++i) {
        CCSprite* dotBgSpr = CCSprite::create("smallDot.png");  // the gray dots that mark each unlock point
        dotBgSpr->setID("dot-bg-sprite-" + std::to_string(i));
        dotBgSpr->setPosition({-dotSpacing * numIconsOnPage / 2.f + dotSpacing * i, 0});
        dotBgSpr->setColor({37, 20, 12});
        progressBarBg->addChild(dotBgSpr);

        if (i != 0) {
            CCSprite* verticalBarBgSpr = CCSprite::createWithSpriteFrameName("whiteSquare20_001.png");  // the vertical gray bars that connect the icons with the dots
            verticalBarBgSpr->setID("vertical-bar-sprite-" + std::to_string(i));

            if (i % 2 == 0) {  // alternate the vertical bars above and below
                verticalBarBgSpr->setAnchorPoint({0.5f, 1});
                verticalBarBgSpr->setPosition({-dotSpacing * numIconsOnPage / 2.f + dotSpacing * i, -10.f});
            } else {
                verticalBarBgSpr->setAnchorPoint({0.5f, 0});
                verticalBarBgSpr->setPosition({-dotSpacing * numIconsOnPage / 2.f + dotSpacing * i, 10.f});
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
    for (int i = m_maxIconsPerPage * pageNum; i < m_maxIconsPerPage * (pageNum + 1); ++i) {
        if (achievementManager->isAchievementEarned(m_category->achievements[i]->id.c_str())) {
            numAchievementsUnlocked++;
        }
    }

    for (int i = 0; i <= numAchievementsUnlocked; ++i) {
        CCSprite* dotFillSpr = CCSprite::create("smallDot.png");  // the colored dots that mark the reached unlock points
        dotFillSpr->setID("dot-fill-sprite-" + std::to_string(i));
        dotFillSpr->setPosition({-dotSpacing * numIconsOnPage / 2.f + dotSpacing * i, 0});
        dotFillSpr->setColor(usePlayerColors ? gameManager->colorForIdx(gameManager->getPlayerColor()) : ccc3(255, 255, 255));
        dotFillSpr->setScale(0.7f);
        progressBarFill->addChild(dotFillSpr);
    }

    for (int i = 0; i < numDotsOnPage - 1; ++i) {
        Achievement* currAchievement = m_category->achievements[i + pageNum * m_maxIconsPerPage];

        float ratio;
        if (i == 0)
            ratio = std::min(1.f, float(progress) / currAchievement->unlockValue);
        else
            ratio = std::min(1.f, float(progress - m_category->achievements[i - 1 + pageNum * m_maxIconsPerPage]->unlockValue) / (currAchievement->unlockValue - m_category->achievements[i - 1 + pageNum * m_maxIconsPerPage]->unlockValue));

        if (ratio < 0.f) break;

        CCSprite* fillSpr = CCSprite::createWithSpriteFrameName("whiteSquare20_001.png");  // the colored bar that fills up to the next unlock point and marks the progress
        fillSpr->setID("progress-bar-fill-sprite-" + std::to_string(i));
        fillSpr->setAnchorPoint({0, 0.5f});
        fillSpr->setPosition({-dotSpacing * numIconsOnPage / 2.f + dotSpacing * i, 0});
        fillSpr->setColor(usePlayerColors ? gameManager->colorForIdx(gameManager->getPlayerColor()) : ccc3(255, 255, 255));
        fillSpr->setScaleX(dotSpacing / 10 * ratio);
        fillSpr->setScaleY(0.2f);

        progressBarFill->addChild(fillSpr);
    }

    /* Player Sprites */
    CCMenu* playerMenu = CCMenu::create();
    playerMenu->setID("unlocks-container");
    playerMenu->setPosition(progressBar->getPosition());
    page->addChild(playerMenu);

    const int unlockValues[] = {5, 15, 35, 65, 100};
    for (int i = 0; i < numIconsOnPage; ++i) {
        Achievement* currAchievement = m_category->achievements[i + pageNum * m_maxIconsPerPage];

        // Create the lock
        bool earned = achievementManager->isAchievementEarned(currAchievement->id.c_str());
        CCSprite* lock = CCSprite::createWithSpriteFrameName("GJ_lock_001.png");
        lock->setID("lock-" + std::to_string(i));
        lock->setZOrder(1);

        // Create the text that shows how much is needed to unlock
        CCLabelBMFont* unlockValue = CCLabelBMFont::create(formatWithCommas(unlockValues[i]).c_str(), "bigFont.fnt");
        unlockValue->setID("unlock-text-" + std::to_string(i));
        unlockValue->setScale(.25f);

        // Load the icon
        GJItemIcon* unlockItem;

        if (earned) {
            std::vector<UnlockType> playerUnlockTypes = {UnlockType::Cube, UnlockType::Ship, UnlockType::Ball, UnlockType::Bird, UnlockType::Dart, UnlockType::Robot, UnlockType::Spider, UnlockType::Swing, UnlockType::Jetpack};
            bool isIcon = std::find(playerUnlockTypes.begin(), playerUnlockTypes.end(), currAchievement->unlockType) != playerUnlockTypes.end();

            if (usePlayerColors) {
                unlockItem = GJItemIcon::create(currAchievement->unlockType, currAchievement->unlockID, gameManager->colorForIdx(gameManager->getPlayerColor()), gameManager->colorForIdx(gameManager->getPlayerColor2()), isIcon, false, false, gameManager->colorForIdx(gameManager->getPlayerGlowColor()));  // p4: is icon?, p5: idk, p6: hide color number? ----- Not sure how to turn on glow, so doing that separately below

                if (gameManager->m_playerGlow) {
                    CCObject* child;
                    for (auto child : CCArrayExt(unlockItem->getChildren())) {
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
            menu_selector(ShardPopup::onIcon));

        // This is for the callback function
        IconCallbackData* data = new IconCallbackData(currAchievement->unlockType, currAchievement->unlockID, currAchievement->achievedDescription);
        data->autorelease();
        unlockButton->setUserObject(data);

        unlockButton->setID("unlock-sprite-" + std::to_string(i));
        unlockButton->setPosition({-dotSpacing * numIconsOnPage / 2.f + dotSpacing * (i + 1), (i % 2 == 0) ? 40.f : -40.f});
        unlockButton->m_baseScale = 0.7f;
        unlockButton->setScale(0.7f);
        playerMenu->addChild(unlockButton);

        unlockValue->setPosition({unlockButton->getPositionX(), unlockButton->getPositionY() + (i % 2 == 0 ? 18.f : -16.f)});
        playerMenu->addChild(unlockValue);
    }

    return page;
}