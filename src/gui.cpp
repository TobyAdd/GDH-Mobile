#include "gui.hpp"
#include <Geode/Geode.hpp>

using namespace geode::prelude;

std::array<std::string, 5> tabs = {"Core", "Player", "Creator", "Replay", "About"};

bool HacksLayer::setup() {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    this->setTitle("GDH Mobile");

    float tabPositionY = 170.f;

    for (int i = 0; i < tabs.size(); i++) {
        auto button = ButtonSprite::create(tabs[i].c_str(), 90, true, "bigFont.fnt", (i == 0) ? "GJ_button_02.png" : "GJ_button_01.png", 30.f, 0.7f);
        auto buttonClick = CCMenuItemSpriteExtra::create(button, this, menu_selector(HacksLayer::onChangeTab));
        buttonClick->setPosition({65.f, tabPositionY});
        buttonClick->setTag(i);
        m_buttonMenu->addChild(buttonClick);

        tabPositionY -= 35;
    }

    auto background = extension::CCScale9Sprite::create("square02_small.png");
    background->setOpacity(100);
    background->setPosition({238, 100});
    background->setContentSize({230, 180});
    m_mainLayer->addChild(background);

    auto& hacks = Hacks::get();

    auto arrow = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    auto left_arrowClick = CCMenuItemExt::createSpriteExtra(arrow, [this](CCMenuItemSpriteExtra* sender) {

    });
    left_arrowClick->setPosition({-25, 110});
    m_buttonMenu->addChild(left_arrowClick);

    auto arrowFlip = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    arrowFlip->setFlipX(true);
    auto right_arrowClick = CCMenuItemExt::createSpriteExtra(arrowFlip, [this](CCMenuItemSpriteExtra* sender) {

    });
    right_arrowClick->setPosition({390, 110});
    m_buttonMenu->addChild(right_arrowClick);

    //
    //Replay Engine
    //
    replayTab = CCMenu::create();
    replayTab->setPosition({0, 0});
    replayTab->setVisible(false);

    record_toggle = CCMenuItemExt::createTogglerWithStandardSprites(1.f, [this](CCMenuItemToggler* sender) {
        auto& hacks = Hacks::get();

        play_toggle->toggle(false);
        if (!record_toggle->isOn()) {
            engine.mode = state::record;

            if (!hacks.fps_enabled) {
                FLAlertLayer::create("Info", "Enable TPS Bypass to record the replay", "OK")->show();
                engine.mode = state::disable;
                record_toggle->toggle(true);
            }

            if (!engine.replay2.empty()) {
                FLAlertLayer::create("Info", "Please clear replay before recording another", "OK")->show();
                engine.mode = state::disable;
                record_toggle->toggle(true);
            }
        }
        else {
            engine.mode = state::disable;
        }
    });
    record_toggle->setPosition({145, 165});
    if (engine.mode == state::record) record_toggle->toggle(true);
    replayTab->addChild(record_toggle);

    auto record_label = CCLabelBMFont::create("Record", "bigFont.fnt");
    record_label->setAnchorPoint({0.f, 0.5f});
    record_label->setPosition({165, 165});
    record_label->setScale(0.5f);
    replayTab->addChild(record_label);

    play_toggle = CCMenuItemExt::createTogglerWithStandardSprites(1.f, [this](CCMenuItemToggler* sender) {
        auto& hacks = Hacks::get();

        record_toggle->toggle(false);
        if (!play_toggle->isOn()) {
            if (!hacks.fps_enabled) {
                FLAlertLayer::create("Info", "Enable TPS Bypass to playback the replay", "OK")->show();
                engine.mode = state::disable;
                play_toggle->toggle(true);
            }
            engine.mode = state::play;
        }
        else {
            engine.mode = state::disable;
        }
    });
    play_toggle->setPosition({250, 165});
    if (engine.mode == state::play) play_toggle->toggle(true);
    replayTab->addChild(play_toggle);

    auto play_label = CCLabelBMFont::create("Play", "bigFont.fnt");
    play_label->setAnchorPoint({0.f, 0.5f});
    play_label->setPosition({270, 165});
    play_label->setScale(0.5f);
    replayTab->addChild(play_label);

    auto replay_name_input = TextInput::create(220, "Replay Name", "chatFont.fnt");
    replay_name_input->setPosition({240.f, 130.f});
    replay_name_input->setString(engine.replay_name);
    replay_name_input->setCallback([&hacks](const std::string& text) {
        engine.replay_name = text;
    });
    replayTab->addChild(replay_name_input);

    auto info_label = CCLabelBMFont::create(fmt::format("Frame: {}\nReplay Size: {}", engine.get_frame(), engine.replay2.size()).c_str(), "chatFont.fnt");
    info_label->setAnchorPoint({0.f, 0.5f});
    info_label->setPosition({128, 25});
    info_label->setScale(0.5f);
    replayTab->addChild(info_label);

    auto saveButton = ButtonSprite::create("Save", 40, true, "bigFont.fnt", "GJ_button_01.png", 30.f, 0.7f);
    auto saveButtonClick = CCMenuItemExt::createSpriteExtra(saveButton, [this](CCMenuItemSpriteExtra* sender) {
        FLAlertLayer::create("Info", engine.save(engine.replay_name).c_str(), "OK")->show();
    });
    saveButtonClick->setPosition({155, 95});
    replayTab->addChild(saveButtonClick);

    auto loadButton = ButtonSprite::create("Load", 40, true, "bigFont.fnt", "GJ_button_01.png", 30.f, 0.7f);
    auto loadButtonClick = CCMenuItemExt::createSpriteExtra(loadButton, [this, info_label](CCMenuItemSpriteExtra* sender) {
        FLAlertLayer::create("Info", engine.load(engine.replay_name).c_str(), "OK")->show();
        info_label->setString(fmt::format("Frame: {}\nReplay Size: {}", engine.get_frame(), engine.replay2.size()).c_str());
    });
    loadButtonClick->setPosition({215, 95});
    replayTab->addChild(loadButtonClick);

    auto cleanButton = ButtonSprite::create("Clear", 40, true, "bigFont.fnt", "GJ_button_01.png", 30.f, 0.7f);
    auto cleanButtonClick = CCMenuItemExt::createSpriteExtra(cleanButton, [this, info_label](CCMenuItemSpriteExtra* sender) {
        engine.replay.clear();
        engine.replay2.clear();
        FLAlertLayer::create("Info", "Replay has been cleared", "OK")->show();
        info_label->setString(fmt::format("Frame: {}\nReplay Size: {}", engine.get_frame(), engine.replay2.size()).c_str());
    });
    cleanButtonClick->setPosition({275, 95});
    replayTab->addChild(cleanButtonClick);

    auto accuracy_fix_toggle = CCMenuItemExt::createTogglerWithStandardSprites(1.f, [this](CCMenuItemToggler* sender) { engine.accuracy_fix = !sender->isOn(); });
    accuracy_fix_toggle->toggle(engine.accuracy_fix);
    accuracy_fix_toggle->setScale(0.75f);
    accuracy_fix_toggle->setPosition({138.f, 65.f});
    replayTab->addChild(accuracy_fix_toggle);

    auto accuracy_fix_label = CCLabelBMFont::create("Accuracy Fix", "bigFont.fnt");
    accuracy_fix_label->setAnchorPoint({0.f, 0.5f});
    accuracy_fix_label->setScale(0.3f);
    accuracy_fix_label->setPosition({155.f, 65.f});
    replayTab->addChild(accuracy_fix_label);

    auto real_time_toggle = CCMenuItemExt::createTogglerWithStandardSprites(1.f, [this](CCMenuItemToggler* sender) { engine.real_time = !sender->isOn(); });
    real_time_toggle->toggle(engine.real_time);
    real_time_toggle->setScale(0.75f);
    real_time_toggle->setPosition({245.f, 65.f});
    replayTab->addChild(real_time_toggle);

    auto real_time_label = CCLabelBMFont::create("Real Time", "bigFont.fnt");
    real_time_label->setAnchorPoint({0.f, 0.5f});
    real_time_label->setScale(0.3f);
    real_time_label->setPosition({262.f, 65.f});
    replayTab->addChild(real_time_label);
    //
    //Replay Engine
    //

    aboutTab = CCMenu::create();
    aboutTab->setPosition({0, 0});
    aboutTab->setVisible(false);
    
    auto aboutLabel = CCLabelBMFont::create("GDH Mobile by TobyAdd\nVersion: 1.0\n\nSpecial Thanks:\nPrevter", "bigFont.fnt");
    aboutLabel->setAnchorPoint({0.f, 0.5f});
    aboutLabel->setScale(0.5f);
    aboutLabel->setPosition({130, 140});
    aboutTab->addChild(aboutLabel);

    m_mainLayer->addChild(replayTab);
    m_mainLayer->addChild(aboutTab);

    return true;
}

void HacksLayer::onChangeTab(CCObject* sender) {
    int tag = sender->getTag();

    for (int i = 0; i < tabs.size(); i++) {
        auto* btn = m_buttonMenu->getChildByTag(i);
        auto* child = btn->getChildren()->objectAtIndex(0);
        if (auto* btnSprite = typeinfo_cast<ButtonSprite*>(child)) {
            btnSprite->updateBGImage(i == tag ? "GJ_button_02.png" : "GJ_button_01.png");
        }
    }

    // coreTab->setVisible(tag == 0);
    // playerTab->setVisible(tag == 1);
    // creatorTab->setVisible(tag == 2);
    replayTab->setVisible(tag == 3);
    aboutTab->setVisible(tag == 4);
}

HacksLayer* HacksLayer::create() {
    auto ret = new HacksLayer();
    if (ret->initAnchored(365.f, 225.f)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}
