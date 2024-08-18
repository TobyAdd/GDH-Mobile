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
    static HacksTab* create();
    void addToggle(const std::string& text, bool enabled, const std::function<void(bool)>& callback);
    float x_lastToggle;
    float y_lastToggle;
private:
    HacksTab() = default;

    bool init();
    void onToggle(CCObject* sender);
protected:
    std::map<CCMenuItemToggler*, std::function<void(bool)>> m_togglerCallbacks;
};

class HacksLayer : public geode::Popup<> {
private:
    CCMenuItemToggler* record_toggle;
    CCMenuItemToggler* play_toggle;

    HacksTab* coreTab;
    CCMenu* replayTab;
    CCMenu* aboutTab;
protected:
    bool setup() override;
    void onChangeTab(CCObject* sender);

public:
    static HacksLayer* create();
};