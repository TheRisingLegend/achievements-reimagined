#include "AchievementCategoryPopup.hpp"

#include "AchievementMenu.hpp"

using namespace geode::prelude;

void AchievementCategoryPopup::createTitle() {
    CCLabelBMFont* unlockTitle = CCLabelBMFont::create(m_category->name.c_str(), "goldFont.fnt");
    unlockTitle->setID("page-title");
    unlockTitle->setPosition({m_mainLayer->getContentWidth() / 2, 260});
    unlockTitle->setScale(1.0f);
    m_mainLayer->addChild(unlockTitle);
}

void AchievementCategoryPopup::addProgressText(int percent, int statValue, int goalValue) {
    CCLabelBMFont* percentLabel = nullptr;
    CCLabelBMFont* numCompleteLabel = nullptr;

    if (percent < 100) {
        percentLabel = CCLabelBMFont::create(("(" + std::to_string(percent) + "%)").c_str(), "bigFont.fnt");
        percentLabel->setScale(0.58f);

        numCompleteLabel = CCLabelBMFont::create(formatWithCommas(statValue).c_str(), "bigFont.fnt");
        numCompleteLabel->setScale(0.39f);
        numCompleteLabel->setPosition({-2, -0.25f});
    } else {
        percentLabel = CCLabelBMFont::create(("(" + std::to_string(percent) + "%)").c_str(), "goldFont.fnt");
        percentLabel->setScale(0.75f);

        numCompleteLabel = CCLabelBMFont::create(formatWithCommas(statValue).c_str(), "goldFont.fnt");
        numCompleteLabel->setScale(0.5f);
        numCompleteLabel->setPosition({-2, 0});
    }

    percentLabel->setID("percent-complete");
    percentLabel->setPosition({m_mainLayer->getContentWidth() / 2, 230});
    percentLabel->setAnchorPoint({0.5f, 0.5f});
    m_mainLayer->addChild(percentLabel);

    std::string goalText = formatWithCommas(goalValue);
    auto goalLabel = CCLabelBMFont::create(("/" + goalText).c_str(), "goldFont.fnt");
    goalLabel->setScale(0.5f);
    goalLabel->setAnchorPoint({0, 0.5f});
    goalLabel->setPosition({-3, 0});
    numCompleteLabel->setAnchorPoint({1, 0.5f});

    auto container = CCNode::create();
    container->setID("fraction-complete");
    container->addChild(numCompleteLabel);
    container->addChild(goalLabel);

    container->setPosition({225, 210});
    m_mainLayer->addChild(container);
}

void AchievementCategoryPopup::addCornerSprites() {
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

void AchievementCategoryPopup::addNavigation() {
    if (m_numPages <= 1) return;

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
    for (int i = 0; i < m_numPages; i++) {
        CCMenuItemSpriteExtra* button = CCMenuItemSpriteExtra::create(
            i == 0 ? CCSprite::createWithSpriteFrameName("gj_navDotBtn_on_001.png") : CCSprite::createWithSpriteFrameName("gj_navDotBtn_off_001.png"),
            this,
            menu_selector(AchievementCategoryPopup::onNavButton));
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
        menu_selector(AchievementCategoryPopup::onArrow));
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
        menu_selector(AchievementCategoryPopup::onArrow));
    rightArrow->setPosition({m_navMenu->getContentWidth() + 30.f, m_navMenu->getContentHeight() / 2});
    rightArrow->setTag(1);
    rightArrow->setID("right-arrow");
    m_navMenu->addChild(rightArrow);
}

void AchievementCategoryPopup::onIcon(CCObject* sender) {
    CCMenuItemSpriteExtra* button = static_cast<CCMenuItemSpriteExtra*>(sender);
    IconCallbackData* data = dynamic_cast<IconCallbackData*>(button->getUserObject());

    ItemInfoPopup* popup = ItemInfoPopup::create(data->unlockID, data->unlockType);
    popup->show();

    std::vector<UnlockType> playerUnlockTypes = {UnlockType::Cube, UnlockType::Ship, UnlockType::Ball, UnlockType::Bird, UnlockType::Dart, UnlockType::Robot, UnlockType::Spider, UnlockType::Swing};

    if (std::find(playerUnlockTypes.begin(), playerUnlockTypes.end(), data->unlockType) == playerUnlockTypes.end()) return;  // is not a player icon

    if (!Mod::get()->getSettingValue<bool>("use-player-colors")) return;  // use player colors setting is off

    if (!popup) return;  // popup creation failed

    auto mainLayer = dynamic_cast<CCLayer*>(popup->getChildByID("main-layer"));
    if (!mainLayer) return;

    auto icon = dynamic_cast<GJItemIcon*>(mainLayer->getChildByID("item-icon"));
    if (!icon) return;

    auto children = icon->getChildren();
    for (int i = 0; i < children->count(); i++) {
        auto simplePlayer = dynamic_cast<SimplePlayer*>(children->objectAtIndex(i));

        simplePlayer->setColors(
            gameManager->colorForIdx(gameManager->getPlayerColor()),
            gameManager->colorForIdx(gameManager->getPlayerColor2()));

        if (gameManager->m_playerGlow) {
            simplePlayer->setGlowOutline(
                gameManager->colorForIdx(gameManager->getPlayerGlowColor()));
        }
    }
}

void AchievementCategoryPopup::onNavButton(CCObject* sender) {
    CCMenuItemSpriteExtra* button = static_cast<CCMenuItemSpriteExtra*>(sender);
    int pageNum = button->getTag();

    // update page visibility
    for (int i = 0; i < m_numPages; i++) {
        CCNode* page = m_mainLayer->getChildByID("page-" + std::to_string(i));
        if (page) {
            page->setVisible(i == pageNum);
        }
    }

    // update navigation buttons
    for (int i = 0; i < m_numPages; i++) {
        CCMenuItemSpriteExtra* navButton = static_cast<CCMenuItemSpriteExtra*>(m_navButtons->getChildByID("page-button-" + std::to_string(i)));
        if (navButton) {
            navButton->setNormalImage(i == pageNum ? CCSprite::createWithSpriteFrameName("gj_navDotBtn_on_001.png") : CCSprite::createWithSpriteFrameName("gj_navDotBtn_off_001.png"));
        }
    }

    // update arrow visibility
    m_navMenu->getChildByID("left-arrow")->setVisible(pageNum > 0);
    m_navMenu->getChildByID("right-arrow")->setVisible(pageNum < m_numPages - 1);
}

void AchievementCategoryPopup::onArrow(CCObject* sender) {
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

    // update navigation buttons
    for (int i = 0; i < m_numPages; i++) {
        CCMenuItemSpriteExtra* navButton = static_cast<CCMenuItemSpriteExtra*>(m_navButtons->getChildByID("page-button-" + std::to_string(i)));
        if (navButton) {
            navButton->setNormalImage(i == newPage ? CCSprite::createWithSpriteFrameName("gj_navDotBtn_on_001.png") : CCSprite::createWithSpriteFrameName("gj_navDotBtn_off_001.png"));
        }
    }

    // update arrow visibility
    m_navMenu->getChildByID("left-arrow")->setVisible(newPage > 0);
    m_navMenu->getChildByID("right-arrow")->setVisible(newPage < m_numPages - 1);
}

void AchievementCategoryPopup::onClose(CCObject* sender) {
    Popup::onClose(sender);

    m_achievementMenu->showArrows();
}
