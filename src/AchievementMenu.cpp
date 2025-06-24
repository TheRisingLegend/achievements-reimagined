#include "AchievementMenu.hpp"

#include "DistinctAchievementPopup.hpp"
#include "ProgressAchievementPopup.hpp"

using namespace geode::prelude;

bool AchievementMenu::setup() {
    m_achievementManager = AchievementManager::sharedState();

    setTitle("Achievements", "goldFont.fnt", 1.0f);

    createCategoryMenu();

    // Assign all achievements to a category
    if (achievementsLoaded)
        return true;

    CCArray* array = m_achievementManager->m_allAchievements;
    for (int i = 0; i < array->count(); i++) {
        auto item = array->objectAtIndex(i);

        auto dict = typeinfo_cast<CCDictionary*>(item);
        if (!dict) continue;

        Achievement* ach = new Achievement();

        ach->title = std::string(dict->valueForKey("title")->getCString());
        ach->id = std::string(dict->valueForKey("identifier")->getCString());

        if (Mod::get()->getSetting("use-better-descriptions") && betterDescriptions.contains(ach->id)) {
            ach->unachievedDescription = std::get<0>(betterDescriptions[ach->id]);
            ach->achievedDescription = std::get<1>(betterDescriptions[ach->id]);

            dict->setObject(CCString::create(ach->unachievedDescription), "unachievedDescription");
            dict->setObject(CCString::create(ach->achievedDescription), "achievedDescription");
        } else {
            ach->achievedDescription = std::string(dict->valueForKey("achievedDescription")->getCString());
            ach->unachievedDescription = std::string(dict->valueForKey("unachievedDescription")->getCString());
        }

        Category* category = getCategory(ach->id, ach->achievedDescription);
        if (category == nullptr) {
            log::debug("Category not found for ID: {}", ach->id);
            continue;
        }

        ach->unlockValue = category->type == "progress" ? extractValue(ach->achievedDescription) : -1;

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

    achievementsLoaded = true;

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
    for (int i = 0; i * m_maxCategoriesPerPage < achievementCategories.size(); i++) {
        auto menuPage = CCMenu::create();
        menuPage->setID("categories-menu");
        menuPage->setContentSize({m_mainLayer->getContentWidth(), m_mainLayer->getContentHeight() - 70.f});
        menuPage->setPosition({m_mainLayer->getContentWidth() / 2, m_mainLayer->getContentHeight() / 2 - 3.f});
        menuPage->setLayout(RowLayout::create()
                                ->setGap(14.f)
                                ->setAxisAlignment(AxisAlignment::Center)
                                ->setCrossAxisAlignment(AxisAlignment::Even)
                                ->setGrowCrossAxis(true));
        menuPage->setTag(i);
        menuPage->setVisible(i == m_categoryPage);

        addCategoryButtons(menuPage, i);

        menuPage->updateLayout();

        m_mainLayer->addChild(menuPage);
        m_categoriesMenu.push_back(menuPage);
    }

    addNavigation();
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

void AchievementMenu::addCategoryButtons(CCMenu* menuPage, int pageNum) {
    for (int i = m_maxCategoriesPerPage * pageNum; i < achievementCategories.size() && i < m_maxCategoriesPerPage * (pageNum + 1); i++) {
        auto button = CCMenuItemSpriteExtra::create(
            ButtonSprite::create(achievementCategories[i].formattedName.c_str(), 90.f, true, "bigFont.fnt", "GJ_button_01.png", 45.f, 1.f),
            this,
            menu_selector(AchievementMenu::onCategoryButton));

        button->setID(achievementCategories[i].name);
        button->setTag(i);

        menuPage->addChild(button);
    }
}

void AchievementMenu::onCategoryButton(CCObject* sender) {
    CCMenuItemSpriteExtra* categoryButton = static_cast<CCMenuItemSpriteExtra*>(sender);

    int index = categoryButton->getTag();
    Category* category = &achievementCategories[index];

    if (category->type == "progress") {
        ProgressAchievementPopup* popup = ProgressAchievementPopup::create(this, category);
        popup->m_noElasticity = GameManager::get()->getGameVariable("0168");  // For fast menu setting
        popup->show();
    } else if (category->type == "distinct") {
        DistinctAchievementPopup* popup = DistinctAchievementPopup::create(this, category);
        popup->m_noElasticity = GameManager::get()->getGameVariable("0168");  // For fast menu setting
        popup->show();
    }

    hideArrows();
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
