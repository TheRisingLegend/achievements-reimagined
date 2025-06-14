#include "AchievementMenuPopup.hpp"

using namespace geode::prelude;

bool AchievementMenuPopup::setup() {
    // add corner sprites and other decorations

    m_achievementManager = AchievementManager::sharedState();

    setTitle("Achievements", "goldFont.fnt", 1.0f);

    createCategoryMenu();

    auto array = m_achievementManager->m_allAchievements;
    for (int i = 0; i < array->count(); i++) {
        auto item = array->objectAtIndex(i);

        auto dict = typeinfo_cast<CCDictionary*>(item);
        if (!dict) continue;

        std::string id = std::string(dict->valueForKey("identifier")->getCString());
        std::string category;
        std::stringstream ss(id);
        std::getline(ss, category, '.');
        std::getline(ss, category, '.');
        std::getline(ss, category, '.');
        log::debug("Category: {}", category);
        // log::debug("ID: {}", id);

        std::string title = std::string(dict->valueForKey("title")->getCString());
        std::string achievedDescription = std::string(dict->valueForKey("achievedDescription")->getCString());
        std::string unachievedDescription = std::string(dict->valueForKey("unachievedDescription")->getCString());

        UnlockType unlockType;
        int unlockID;
        std::string icon = std::string(dict->valueForKey("icon")->getCString());
        size_t pos = icon.find('_');
        if (pos != std::string::npos) {
            unlockID = stoi(icon.substr(pos + 1));
            unlockType = unlockTypeFromString(icon.substr(0, pos));
        } else {
            log::debug("Unknown icon format: {}. {}; {}: {}", dict->valueForKey("icon")->getCString(), category, title, achievedDescription);
        }

        m_achievements.push_back(make_tuple(category, title, achievedDescription, unachievedDescription, unlockType, unlockID));
    }
    return true;
}

AchievementMenuPopup* AchievementMenuPopup::create() {
    auto popup = new AchievementMenuPopup();
    if (popup && popup->initAnchored(450.f, 280.f)) {
        popup->autorelease();
        return popup;
    }
    delete popup;
    return nullptr;
}

void AchievementMenuPopup::createCategoryMenu() {
    for (int i = 0; i * m_maxCategoriesPerPage < m_categories.size(); i++) {
        auto menuPage = CCMenu::create();
        menuPage->setID("categories-menu");
        menuPage->setContentSize({m_size.width, m_size.height - 50.f});
        menuPage->ignoreAnchorPointForPosition(false);
        menuPage->setPosition({this->getContentWidth() / 2, this->getContentHeight() / 2 - 15.f});
        menuPage->setLayout(ColumnLayout::create()
                                ->setGap(10.f)
                                ->setAxisAlignment(AxisAlignment::End)
                                ->setCrossAxisAlignment(AxisAlignment::Start)
                                ->setGrowCrossAxis(true)
                                ->setAxisReverse(true)
                                ->setCrossAxisReverse(true));
        menuPage->setTag(i);
        menuPage->setVisible(i == m_categoryPage);

        addCategoryButtons(menuPage, i);

        menuPage->updateLayout();

        this->addChild(menuPage);
        m_categoriesMenu.push_back(menuPage);
    }

    if (m_categoriesMenu.size() > 1) {
        CCMenu* arrowMenu = CCMenu::create();
        arrowMenu->setID("arrow-menu");
        arrowMenu->setPosition({0, 0});
        this->addChild(arrowMenu);

        CCSprite* leftArrowSprite = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
        m_leftArrow = CCMenuItemSpriteExtra::create(
            leftArrowSprite,
            this,
            menu_selector(AchievementMenuPopup::onCategoryArrow));
        m_leftArrow->setPosition({78.f, m_categoriesMenu[0]->getPositionY()});
        m_leftArrow->setTag(0);
        m_leftArrow->setID("left-arrow");
        arrowMenu->addChild(m_leftArrow);
        m_leftArrow->setVisible(false);

        CCSprite* rightArrowSprite = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
        rightArrowSprite->setFlipX(true);

        m_rightArrow = CCMenuItemSpriteExtra::create(
            rightArrowSprite,
            this,
            menu_selector(AchievementMenuPopup::onCategoryArrow));
        m_rightArrow->setPosition({this->getContentWidth() - 78.f, m_categoriesMenu[0]->getPositionY()});
        m_rightArrow->setTag(1);
        m_rightArrow->setID("right-arrow");
        arrowMenu->addChild(m_rightArrow);
    }
}

void AchievementMenuPopup::onCategoryArrow(CCObject* sender) {
    auto button = typeinfo_cast<CCMenuItemSpriteExtra*>(sender);
    if (!button) return;

    int arrowType = button->getTag();
    if (arrowType == 0)
        m_categoryPage--;
    else if (arrowType == 1)
        m_categoryPage++;

    // Page visibility
    for (auto menu : m_categoriesMenu) {
        menu->setVisible(menu->getTag() == m_categoryPage);
    }

    // Arrow visibility
    m_leftArrow->setVisible(m_categoryPage > 0);
    m_rightArrow->setVisible(m_categoryPage < m_categoriesMenu.size() - 1);
}

void AchievementMenuPopup::addCategoryButtons(CCMenu* menuPage, int pageNum) {
    for (int i = m_maxCategoriesPerPage * pageNum; i < m_categories.size() && i < m_maxCategoriesPerPage * (pageNum + 1); i++) {
        auto button = CCMenuItemSpriteExtra::create(
            ButtonSprite::create(std::get<0>(m_categories[i]).c_str(), 100.f, true, "bigFont.fnt", "GJ_button_01.png", 50.f, 1.f),
            this,
            menu_selector(AchievementMenuPopup::onCategoryButton));

        button->setID(std::get<1>(m_categories[i]));
        button->setTag(i);

        menuPage->addChild(button);
    }
}

void AchievementMenuPopup::onCategoryButton(CCObject* sender) {
    return;
}

UnlockType AchievementMenuPopup::unlockTypeFromString(const std::string& str) {
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
    return UnlockType::GJItem;  // Default case
}
