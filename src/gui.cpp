#include "gui.hpp"
#include <Geode/Geode.hpp>

using namespace geode::prelude;

std::array<std::string, 5> tabs = {"Core", "Player", "Creator", "Replay", "About"};

HacksTab* HacksTab::create() {
    auto ret = new HacksTab();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

void HacksTab::addToggle(const std::string& text, bool enabled, const std::function<void(bool)>& callback) {
    auto toggle = CCMenuItemToggler::createWithStandardSprites(this, menu_selector(HacksTab::onToggle), 0.75f);
    y_lastToggle = 175 - (m_togglerCallbacks.size() * 25);

    toggle->setPosition({ 140, y_lastToggle });
    toggle->toggle(enabled);

    auto label = CCLabelBMFont::create(text.c_str(), "bigFont.fnt");
    label->setAnchorPoint({0.f, 0.5f});
    label->setScale(0.5f);
    label->setPosition({ 155, y_lastToggle });

    addChild(toggle);
    addChild(label);

    m_togglerCallbacks[toggle] = callback;
}

bool HacksTab::init() {
    if (!CCMenu::init())
        return false;

    setPosition({0, 0});

    return true;
}

void HacksTab::onToggle(CCObject* sender) {
    auto toggler = static_cast<CCMenuItemToggler*>(sender);
    if (!toggler) return;

    auto it = m_togglerCallbacks.find(toggler);
    if (it != m_togglerCallbacks.end())
        it->second(!toggler->isToggled());
}

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
        if (index == 0) {
            if (coreTab->isVisible()) {
                coreTab->setVisible(false);
                coreTab2->setVisible(true);
            }
            else if (coreTab2->isVisible()) {
                coreTab->setVisible(true);
                coreTab2->setVisible(false);
            }
        } 
        else if (index == 1) {
            if (playerTab->isVisible()) {
                playerTab->setVisible(false);
                playerTab2->setVisible(false);
                playerTab3->setVisible(true);
            } 
            else if (playerTab2->isVisible()) {
                playerTab->setVisible(true);
                playerTab2->setVisible(false);
                playerTab3->setVisible(false);
            } 
            else if (playerTab3->isVisible()) {
                playerTab->setVisible(false);
                playerTab2->setVisible(true);
                playerTab3->setVisible(false);
            }
        }
    });
    left_arrowClick->setPosition({-25, 110});
    m_buttonMenu->addChild(left_arrowClick);

    auto arrowFlip = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    arrowFlip->setFlipX(true);
    auto right_arrowClick = CCMenuItemExt::createSpriteExtra(arrowFlip, [this](CCMenuItemSpriteExtra* sender) {
        if (index == 0) {
            if (coreTab->isVisible()) {
                coreTab->setVisible(false);
                coreTab2->setVisible(true);
            }
            else if (coreTab2->isVisible()) {
                coreTab->setVisible(true);
                coreTab2->setVisible(false);
            }
        } 
        else if (index == 1) {
            if (playerTab->isVisible()) {
                playerTab->setVisible(false);
                playerTab2->setVisible(true);
                playerTab3->setVisible(false);
            } 
            else if (playerTab2->isVisible()) {
                playerTab->setVisible(false);
                playerTab2->setVisible(false);
                playerTab3->setVisible(true);
            } 
            else {
                playerTab->setVisible(true);
                playerTab2->setVisible(false);
                playerTab3->setVisible(false);
            }
        }
    });
    
    right_arrowClick->setPosition({390, 110});
    m_buttonMenu->addChild(right_arrowClick);

    //
    // Core
    //

    coreTab = HacksTab::create();
    coreTab->addToggle("Noclip", hacks.noclip, [&hacks](bool enabled) { hacks.noclip = enabled; });
    coreTab->addToggle("Unlock Items", hacks.unlock_items, [&hacks](bool enabled) { hacks.unlock_items = enabled; });
    coreTab->addToggle("No Respawn Blink", hacks.no_respawn_blink, [&hacks](bool enabled) { hacks.no_respawn_blink = enabled; });
    coreTab->addToggle("No Death Effect", hacks.no_death_effect, [&hacks](bool enabled) { hacks.no_death_effect = enabled; });
    coreTab->addToggle("Safe Mode", hacks.safe_mode, [&hacks](bool enabled) { hacks.safe_mode = enabled; });
    
    coreTab->addToggle("TPS", hacks.fps_enabled, [&hacks](bool enabled) { hacks.fps_enabled = enabled; });

    auto fps_value_input = TextInput::create(55, "TPS Value", "chatFont.fnt");
    fps_value_input->setPosition({225.f, coreTab->y_lastToggle});
    fps_value_input->setFilter("1234567890.");
    fps_value_input->setMaxCharCount(6);
    fps_value_input->setString(fmt::format("{:.0f}", hacks.fps_value));
    fps_value_input->setCallback([&hacks](const std::string& text) {
        float fps = 240.f;
        
        bool valid = !text.empty() && std::all_of(text.begin(), text.end(), [](char c) { return std::isdigit(c) || c == '.'; });
        
        if (valid) {
            std::istringstream iss(text);
            iss >> fps;
        }
        
        if (fps >= 1.f) {
            hacks.fps_value = fps;
        }
    });
    coreTab->addChild(fps_value_input);     
    //
    //TPS Bypass
    //   

    
    //
    //Speedhack
    //
    
    coreTab->addToggle("Speedhack", hacks.speedhack_enabled, [&hacks](bool enabled) { hacks.speedhack_enabled = enabled; });

    auto speedhack_value_input = TextInput::create(55, "Speed Value", "chatFont.fnt");
    speedhack_value_input->setPosition({285.f, coreTab->y_lastToggle + 5});
    speedhack_value_input->setFilter("1234567890.");
    speedhack_value_input->setMaxCharCount(6);
    speedhack_value_input->setString(fmt::format("{:.2f}", hacks.speedhack_value));
    speedhack_value_input->setCallback([&hacks](const std::string& text) {
        float speed = 1.f;
        
        bool valid = !text.empty() && std::all_of(text.begin(), text.end(), [](char c) { return std::isdigit(c) || c == '.'; });
        
        if (valid) {
            std::istringstream iss(text);
            iss >> speed;
        }
        
        if (speed >= 0.01f) {
            hacks.speedhack_value = speed;
        }
    });
    coreTab->addChild(speedhack_value_input);

    coreTab2 = HacksTab::create();
    coreTab2->setVisible(false);
    coreTab2->addToggle("Speedhack Audio", hacks.speedhack_audio, [&hacks](bool enabled) { hacks.speedhack_audio = enabled; });    
    


    //
    // Core
    //

    //
    // Player
    //
    playerTab = HacksTab::create();
    playerTab->setVisible(false);
    playerTab->addToggle("Allow Low Volume", hacks.allow_low_volume, [&hacks](bool enabled) { hacks.allow_low_volume = enabled; });
    playerTab->addToggle("Auto Pickup Coins", hacks.auto_pickup_coins, [&hacks](bool enabled) { hacks.auto_pickup_coins = enabled; });
    playerTab->addToggle("Jump Hack", hacks.jump_hack, [&hacks](bool enabled) { hacks.jump_hack = enabled; });
    playerTab->addToggle("Force Platformer", hacks.force_platformer, [&hacks](bool enabled) { hacks.force_platformer = enabled; });
    playerTab->addToggle("Hide Pause Button", hacks.hide_pause_button, [&hacks](bool enabled) { hacks.hide_pause_button = enabled; });
    playerTab->addToggle("No Camera Move", hacks.no_camera_move, [&hacks](bool enabled) { hacks.no_camera_move = enabled; });
    playerTab->addToggle("No Camera Zoom", hacks.no_camera_zoom, [&hacks](bool enabled) { hacks.no_camera_zoom = enabled; });

    playerTab2 = HacksTab::create();
    playerTab2->setVisible(false);
    playerTab2->addToggle("No Shaders", hacks.no_shader_layer, [&hacks](bool enabled) { hacks.no_shader_layer = enabled; });
    playerTab2->addToggle("No Particles", hacks.no_particles, [&hacks](bool enabled) { hacks.no_particles = enabled; });
    playerTab2->addToggle("No Short Number", hacks.no_short_number, [&hacks](bool enabled) { hacks.no_short_number = enabled; });
    playerTab2->addToggle("No Glow", hacks.no_glow, [&hacks](bool enabled) { hacks.no_glow = enabled; });
    playerTab2->addToggle("No Mirror Portal", hacks.no_mirror_portal, [&hacks](bool enabled) { hacks.no_mirror_portal = enabled; });
    playerTab2->addToggle("Main Levels Bypass", hacks.main_levels, [&hacks](bool enabled) { hacks.main_levels = enabled; });
    playerTab2->addToggle("No Lighting", hacks.no_lighting, [&hacks](bool enabled) { hacks.no_lighting = enabled; });

    playerTab3 = HacksTab::create();
    playerTab3->setVisible(false);
    playerTab3->addToggle("Show Hitbox", hacks.show_hitboxes, [&hacks](bool enabled) {
        hacks.show_hitboxes = enabled;
        auto pl = PlayLayer::get();
        if (pl && !hacks.show_hitboxes && !(pl->m_isPracticeMode && GameManager::get()->getGameVariable("0166"))) {
            pl->m_debugDrawNode->setVisible(false);
        }    
    });

    playerTab3->addToggle("Hitbox Trail", hacks.draw_trail, [&hacks](bool enabled) { hacks.draw_trail = enabled; });

    auto hitbox_hitbox_trail_size_input = TextInput::create(55, "Trail Size", "chatFont.fnt");
    hitbox_hitbox_trail_size_input->setPosition({310.f, playerTab3->y_lastToggle + 15});
    hitbox_hitbox_trail_size_input->setFilter("1234567890");
    hitbox_hitbox_trail_size_input->setMaxCharCount(3);
    hitbox_hitbox_trail_size_input->setString(fmt::format("{}", hacks.trail_size));
    hitbox_hitbox_trail_size_input->setCallback([&hacks](const std::string& text) {
        int trail_size = 240;
        
        bool valid = !text.empty();
        
        if (valid) {
            std::istringstream iss(text);
            iss >> trail_size;
        }
        
        hacks.trail_size = trail_size;
    });
    playerTab3->addChild(hitbox_hitbox_trail_size_input); 
    //
    // Player
    //

    //
    // Creator
    //

    creatorTab = HacksTab::create();
    creatorTab->setVisible(false);
    creatorTab->addToggle("Copy Hack", hacks.copy_hack, [&hacks](bool enabled) { hacks.copy_hack = enabled; });
    creatorTab->addToggle("Level Edit", hacks.level_edit, [&hacks](bool enabled) { hacks.level_edit = enabled; });
    creatorTab->addToggle("No (C) Mark", hacks.no_c_mark, [&hacks](bool enabled) { hacks.no_c_mark = enabled; });
    creatorTab->addToggle("Verify Hack", hacks.verify_hack, [&hacks](bool enabled) { hacks.verify_hack = enabled; });

    //
    // Creator
    //

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

    //
    //About
    //
    aboutTab = CCMenu::create();
    aboutTab->setPosition({0, 0});
    aboutTab->setVisible(false);
    
    auto aboutLabel = CCLabelBMFont::create("GDH Mobile by TobyAdd\nVersion: 1.0\n\nSpecial Thanks:\nPrevter", "bigFont.fnt");
    aboutLabel->setAnchorPoint({0.f, 0.5f});
    aboutLabel->setScale(0.5f);
    aboutLabel->setPosition({130, 140});
    aboutTab->addChild(aboutLabel);
    //
    //About
    //

    m_mainLayer->addChild(coreTab);
    m_mainLayer->addChild(coreTab2);

    m_mainLayer->addChild(playerTab);
    m_mainLayer->addChild(playerTab2);
    m_mainLayer->addChild(playerTab3);

    m_mainLayer->addChild(creatorTab);

    m_mainLayer->addChild(replayTab);
    m_mainLayer->addChild(aboutTab);

    return true;
}

void HacksLayer::onChangeTab(CCObject* sender) {
    index = sender->getTag();

    for (int i = 0; i < tabs.size(); i++) {
        auto* btn = m_buttonMenu->getChildByTag(i);
        auto* child = btn->getChildren()->objectAtIndex(0);
        if (auto* btnSprite = typeinfo_cast<ButtonSprite*>(child)) {
            btnSprite->updateBGImage(i == index ? "GJ_button_02.png" : "GJ_button_01.png");
        }
    }

    coreTab->setVisible(index == 0);
    coreTab2->setVisible(false);

    playerTab->setVisible(index == 1);
    playerTab2->setVisible(false);
    playerTab3->setVisible(false);

    creatorTab->setVisible(index == 2);

    replayTab->setVisible(index == 3);
    aboutTab->setVisible(index == 4);
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
