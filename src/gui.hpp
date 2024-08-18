#pragma once
#include <Geode/Geode.hpp>
#include <functional>
#include <map>
#include <array>
#include <string>
#include "hacks.hpp"
#include "replayEngine.hpp"

using namespace geode::prelude;

class HacksLayer : public geode::Popup<> {
private:
    CCMenuItemToggler* record_toggle;
    CCMenuItemToggler* play_toggle;

    CCMenu* replayTab;
    CCMenu* aboutTab;
protected:
    bool setup() override;
    void onChangeTab(CCObject* sender);

public:
    static HacksLayer* create();
};