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
protected:
    bool setup() override;
    void onChangeTab(CCObject* sender);

public:
    static HacksLayer* create();
};