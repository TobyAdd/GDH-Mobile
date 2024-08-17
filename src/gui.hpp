#pragma once
#include <Geode/Geode.hpp>
#include <functional>
#include <map>
#include <array>
#include <string>
#include "hacks.hpp"
#include "replayEngine.hpp"

using namespace geode::prelude;

class HacksTab : public CCMenu {
public:
    static HacksTab* create(bool no_scroll);
    void addToggle(const std::string& text, bool enabled, const std::function<void(bool)>& callback, float scale);
    void setAmountOfHacks(int amount);
    ScrollLayer* m_scrollLayer;
    float y_lastToggle;
private:
    HacksTab() = default;

    bool init(bool no_scroll);
    void onToggle(CCObject* sender);
protected:
    std::map<CCMenuItemToggler*, std::function<void(bool)>> m_togglerCallbacks;
    CCMenu* m_togglesMenu;
};

class HacksLayer : public geode::Popup<> {
private:
    HacksTab* coreTab;
    HacksTab* playerTab;
    HacksTab* creatorTab;
    HacksTab* replayTab;
    HacksTab* aboutTab;

    CCMenuItemToggler* record_toggle;
    CCMenuItemToggler* play_toggle;

    void on_record(CCObject*) {
        play_toggle->toggle(false);
        if (!record_toggle->isOn()) {
            engine.mode = state::record;
            if (!engine.replay2.empty()) {
                FLAlertLayer::create("Info", "Clear replay to record new replay", "OK")->show();
                engine.mode = state::disable;
                record_toggle->toggle(false);
            }
        }
        else {
            engine.mode = state::disable;
        }
    }

    void on_play(CCObject*) {
        record_toggle->toggle(false);
        if (!play_toggle->isOn()) {
            engine.mode = state::play;
        }
        else {
            engine.mode = state::disable;
        }
    }

    void on_save(CCObject*) {
        FLAlertLayer::create("Info", engine.save(engine.replay_name).c_str(), "OK")->show();
    }

    void on_load(CCObject*) {
        FLAlertLayer::create("Info", engine.load(engine.replay_name).c_str(), "OK")->show();
    }

    void on_clear_replay(CCObject*) {
        engine.replay.clear();
        engine.replay2.clear();
        FLAlertLayer::create("Info", "Replay has been cleared", "OK")->show();
    }


protected:
    bool setup() override;
    void onChangeTab(CCObject* sender);

public:
    static HacksLayer* create();
};