#include "gui.hpp"
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include "hacks.hpp"

using namespace geode::prelude;

void CheckDir(const std::filesystem::path &path)
{
    if (!std::filesystem::is_directory(path) || !std::filesystem::exists(path))
    {
        std::filesystem::create_directory(path);
    }
}

$execute {
    CheckDir(folderMacroPath);

	auto& config = Config::get();
	config.load(fileDataPath);
}

class $modify(MyMenuLayer, MenuLayer) {
	bool init() {
		if (!MenuLayer::init()) {
			return false;
		}

		auto myButton = CCMenuItemSpriteExtra::create(
			CCSprite::createWithSpriteFrameName("GJ_likeBtn_001.png"),
			this,
			menu_selector(MyMenuLayer::showPopup)
		);

		auto menu = this->getChildByID("bottom-menu");
		menu->addChild(myButton);

		myButton->setID("gdh-menu"_spr);
		menu->updateLayout();

		return true;
	}

	void showPopup(CCObject* sender) {
		HacksLayer::create()->show();
	}
};

class $modify(MyPauseLayer, PauseLayer) {
	void customSetup() {
		PauseLayer::customSetup();

		auto myButton = CCMenuItemSpriteExtra::create(
			CCSprite::createWithSpriteFrameName("GJ_likeBtn_001.png"),
			this,
			menu_selector(MyPauseLayer::showPopup)
		);
		myButton->setPosition({35.f, 35.f});
		myButton->setID("gdh-menu"_spr);

		CCMenu* menu = CCMenu::create();
		menu->setPosition({0, 0});
		
		menu->addChild(myButton);
		this->addChild(menu);
	}

	void showPopup(CCObject* sender) {
		HacksLayer::create()->show();
	}
};

class $modify(MyEditorPauseLayer, EditorPauseLayer) {
	void customSetup() {
		EditorPauseLayer::customSetup();

		auto myButton = CCMenuItemSpriteExtra::create(
			CCSprite::createWithSpriteFrameName("GJ_likeBtn_001.png"),
			this,
			menu_selector(MyEditorPauseLayer::showPopup)
		);
		myButton->setPosition({410.f, 285.f});
		myButton->setID("gdh-menu"_spr);

		CCMenu* menu = CCMenu::create();
		menu->setPosition({0, 0});
		
		menu->addChild(myButton);
		this->addChild(menu);
	}

	void showPopup(CCObject* sender) {
		HacksLayer::create()->show();
	}
};