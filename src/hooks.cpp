#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GameManager.hpp>
#include <Geode/modify/GameStatsManager.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/GJGameLevel.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/UILayer.hpp>
#include <Geode/modify/CameraTriggerGameObject.hpp>
#include <Geode/modify/ShaderLayer.hpp>
#include <Geode/modify/CCParticleSystemQuad.hpp>
#include <Geode/modify/GameToolbox.hpp>
#include <Geode/modify/EndLevelLayer.hpp>
#include <Geode/modify/LevelPage.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/LevelTools.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/EditLevelLayer.hpp>
#include <Geode/modify/ShareLevelLayer.hpp>
#include <Geode/modify/SongSelectNode.hpp>
#include <Geode/modify/CCScheduler.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/CCDrawNode.hpp>
#include <Geode/modify/OptionsLayer.hpp>
#include "hacks.hpp"
#include "speedhackAudio.hpp"
#include "replayEngine.hpp"

std::deque<cocos2d::CCRect> playerTrail1, playerTrail2;

void drawRect(cocos2d::CCDrawNode *node, const cocos2d::CCRect &rect, const cocos2d::_ccColor4F &color, float borderWidth, const cocos2d::_ccColor4F &borderColor) {
    std::vector<cocos2d::CCPoint> vertices = {
            cocos2d::CCPoint(rect.getMinX(), rect.getMinY()),
            cocos2d::CCPoint(rect.getMinX(), rect.getMaxY()),
            cocos2d::CCPoint(rect.getMaxX(), rect.getMaxY()),
            cocos2d::CCPoint(rect.getMaxX(), rect.getMinY())
    };
    node->drawPolygon(vertices.data(), vertices.size(), color, borderWidth, borderColor);
}

void drawTrail(cocos2d::CCDrawNode *node) {
    for (auto &hitbox: playerTrail1) {
        drawRect(node, hitbox, {0, 0, 0, 0}, 0.25f, {1.f, 1.f, 0.f, 1.0f});
    }
        
    for (auto &hitbox: playerTrail2) {
        drawRect(node, hitbox, {0, 0, 0, 0}, 0.25f, {1.f, 1.f, 0.f, 1.0f});
    }
}

class $modify(cocos2d::CCDrawNode) {
    bool drawPolygon(cocos2d::CCPoint* vertex, unsigned int count, const cocos2d::ccColor4F& fillColor,
                     float borderWidth, const cocos2d::ccColor4F& borderColor) {
        auto& config = Config::get();

        if (config.get<bool>("show_hitboxes", false))
            borderWidth = abs(borderWidth);

        return cocos2d::CCDrawNode::drawPolygon(vertex, count, fillColor, borderWidth, borderColor);
    }

    bool drawCircle(const cocos2d::CCPoint& position, float radius, const cocos2d::ccColor4F& color,
                    float borderWidth, const cocos2d::ccColor4F& borderColor, unsigned int segments) {
        auto& config = Config::get();

        if (config.get<bool>("show_hitboxes", false))
            borderWidth = abs(borderWidth);            

        return cocos2d::CCDrawNode::drawCircle(position, radius, color, borderWidth, borderColor, segments);
    }
};

float left_over = 0.f;
class $modify(cocos2d::CCScheduler) {
    void update(float dt) {
        auto& config = Config::get();        

        if (config.get<bool>("speedhack_enabled", false))
            dt *= config.get<float>("speedhack_value", 1.f);

        speedhackAudio::update();

        auto pl = GameManager::sharedState()->getPlayLayer();

        if (config.get<bool>("fps_enabled", false) && pl && !pl->m_isPaused) {
            float newdt = 1.f / config.get<float>("fps_value", 240.f); 

            if (!engine.real_time) {
                return CCScheduler::update(newdt);
            }

            unsigned times = static_cast<int>((dt + left_over) / newdt);  
            auto start = std::chrono::high_resolution_clock::now();
            using namespace std::literals;

            for (unsigned i = 0; i < times; ++i) {
                CCScheduler::update(newdt);
                if (std::chrono::high_resolution_clock::now() - start > 33.333ms) {            
                    times = i + 1;
                    break;
                }
            }
            left_over += dt - newdt * times;
        }
        else {
            CCScheduler::update(dt);
        }
    }
};

class $modify(PlayLayer) {
    struct Fields {
        GameObject* anticheat_obj = nullptr;
        std::vector<GameObject *> coinsObjects;

        ~Fields() {
            playerTrail1.clear();
            playerTrail2.clear();
        }
    };

    void destroyPlayer(PlayerObject* player, GameObject* obj) {
        auto& config = Config::get();

        if (!m_fields->anticheat_obj)
            m_fields->anticheat_obj = obj;
        
        if (obj == m_fields->anticheat_obj)
            PlayLayer::destroyPlayer(player, obj);

        if (!config.get<bool>("noclip", false)) {
            PlayLayer::destroyPlayer(player, obj);
        }
    }

    void updateProgressbar() {
        PlayLayer::updateProgressbar();

        auto& config = Config::get();

        if (config.get<bool>("show_hitboxes", false)) {
            if (!(m_isPracticeMode && GameManager::get()->getGameVariable("0166")))
                PlayLayer::updateDebugDraw();
            
            m_debugDrawNode->setVisible(true);

            if (config.get<bool>("draw_trail", false))
                drawTrail(m_debugDrawNode);
        }
    }

    void showNewBest(bool p0, int p1, int p2, bool p3, bool p4, bool p5) {
        if (Config::get().get<bool>("safe_mode", false)) return;        
        PlayLayer::showNewBest(p0, p1, p2, p3, p4, p5);
    }

    void resetLevel() {
        auto& config = Config::get();

        PlayLayer::resetLevel();

        playerTrail1.clear();
        playerTrail2.clear();

        engine.handle_reseting(this);

        left_over = 0.f;

        if (config.get<bool>("safe_mode", false))
            m_level->m_attempts = m_level->m_attempts - 1;

        if (config.get<bool>("auto_pickup_coins", false)) {
            for (auto* coin : m_fields->coinsObjects) {
                if (!coin) continue;
                destroyObject(coin);
                pickupItem(static_cast<EffectGameObject*>(coin));
            }
        }

    }

    void addObject(GameObject* obj) {
        auto& config = Config::get();

        PlayLayer::addObject(obj);

        if (obj->m_objectID == 1329 || obj->m_objectID == 142) {
            m_fields->coinsObjects.push_back(obj);
        }

        if (config.get<bool>("no_glow", false)) {
            obj->m_hasNoGlow = true;
        }
    }

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        auto& config = Config::get();
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        if (config.get<bool>("force_platformer", false))  {
            m_player1->togglePlatformerMode(true);
            m_player2->togglePlatformerMode(true);
        }

        return true;
    }

    void playEndAnimationToPos(cocos2d::CCPoint pos) {
        PlayLayer::playEndAnimationToPos(pos);
        if (engine.mode == state::record)
            engine.mode = state::disable;
    }

    void playPlatformerEndAnimationToPos(cocos2d::CCPoint pos, bool idk) {
        PlayLayer::playPlatformerEndAnimationToPos(pos, idk);
        if (engine.mode == state::record)
            engine.mode = state::disable;
    }
};

class $modify(GameManager) {
    bool isColorUnlocked(int key, UnlockType type) {
        if (GameManager::isColorUnlocked(key, type))
            return true;

        auto& config = Config::get();
        return config.get<bool>("unlock_items", false);
    }

    bool isIconUnlocked(int key, IconType type) {
        if (GameManager::isIconUnlocked(key, type))
            return true;

        auto& config = Config::get();
        return config.get<bool>("unlock_items", false);
    }
};

class $modify(GameStatsManager) {
    bool isItemUnlocked(UnlockType p0, int p1) {
        if (GameStatsManager::isItemUnlocked(p0, p1))
            return true;
        
        auto& config = Config::get();
        if (!config.get<bool>("unlock_items", false))
            return false;

        if (p0 == UnlockType::GJItem & p1 == 16)
            return true;

        if (p0 == UnlockType::GJItem & p1 == 17)
            return true;

        return false;
    }
};

class $modify(PlayerObject) {
    void playSpawnEffect() {
        if (Config::get().get<bool>("no_respawn_blink", false)) return;        
        PlayerObject::playSpawnEffect();
    }

    void playDeathEffect() {
        if (Config::get().get<bool>("no_death_effect", false)) return;            
        PlayerObject::playDeathEffect();
    }

    void incrementJumps() {
        if (Config::get().get<bool>("safe_mode", false)) return;

        PlayerObject::incrementJumps();
    }
};

class $modify(GJGameLevel) {
    void savePercentage(int percent, bool isPracticeMode, int clicks, int attempts, bool isChkValid) {
        if (Config::get().get<bool>("safe_mode", false)) return;
        GJGameLevel::savePercentage(percent, isPracticeMode, clicks, attempts, isChkValid);
    }
};

class $modify(GJBaseGameLayer) {
    struct Fields {
        bool jump;
    };

    void handleButton(bool down, int button, bool isPlayer1) {
        auto& config = Config::get();

        GJBaseGameLayer::handleButton(down, button, isPlayer1);   

        if (config.get<bool>("jump_hack", false) && down) {
            m_fields->jump = true;
        }

        if (engine.mode == state::record) {
            engine.handle_recording2(down, button, isPlayer1);
        }
    }

    void update(float dt) {
        auto& config = Config::get();

        GJBaseGameLayer::update(dt);

        if (config.get<bool>("jump_hack", false) && m_fields->jump) {
            m_player1->m_isOnGround = true;
            m_player2->m_isOnGround = true;
            m_fields->jump = false;
        }

        if (engine.mode == state::record) {
            engine.handle_recording(this, true);
            engine.handle_recording(this, false);
        }
        else if (engine.mode == state::play) {
            engine.handle_playing(this);
        }
    }

    void updateZoom(float p0, float p1, int p2, float p3, int p4, int p5) {
        auto& config = Config::get();
        if (config.get<bool>("no_camera_zoom", false)) return;

        GJBaseGameLayer::updateZoom(p0, p1, p2, p3, p4, p5);
    }

    void teleportPlayer(TeleportPortalObject *p0, PlayerObject *p1) {
        auto& config = Config::get();
        bool orig = p0->m_hasNoEffects;
        p0->m_hasNoEffects = config.get<bool>("no_lighting", false) ? true : orig;
        GJBaseGameLayer::teleportPlayer(p0, p1);
        p0->m_hasNoEffects = orig;
    }

    bool canBeActivatedByPlayer(PlayerObject *p0, EffectGameObject *p1) {
        auto& config = Config::get();
        if (config.get<bool>("no_mirror_portal", false) && p1->m_objectType == GameObjectType::InverseMirrorPortal) return false;

        return GJBaseGameLayer::canBeActivatedByPlayer(p0, p1);
    }

    void processCommands(float dt) {
        GJBaseGameLayer::processCommands(dt);

        auto& config = Config::get();
        if (config.get<bool>("show_hitboxes", false) && config.get<bool>("draw_trail", false)) {
            if (!m_player1->m_isDead) {
                playerTrail1.push_back(m_player1->getObjectRect());
                playerTrail2.push_back(m_player2->getObjectRect());
            }

            auto pl = GameManager::sharedState()->getPlayLayer();
            if (!pl) return;

            auto maxLength = static_cast<size_t>(config.get<int>("trail_size", 240));
            while (playerTrail1.size() > maxLength)
                playerTrail1.pop_front();
            while (playerTrail2.size() > maxLength)
                playerTrail2.pop_front();
        }
    }

    float getModifiedDelta(float dt) {
        auto& config = Config::get();

        if (!config.get<bool>("fps_enabled", false)) {
            return GJBaseGameLayer::getModifiedDelta(dt);
        }

        if (m_resumeTimer-- > 0) {
            dt = 0.f;
        }

        float modifier = fminf(1.0, m_gameState.m_timeWarp) / config.get<float>("fps_value", 240.f);
        float total = dt + m_unk3248; //probably m_physDeltaBuffer, idk
        double result = (double)llroundf(total / modifier) * modifier;
        m_unk3248 = total - result;
        return result;
    }
};

class $modify(UILayer) {
    bool init(GJBaseGameLayer* p0) {
        auto& config = Config::get();

        if (!UILayer::init(p0)) return false;
        if (!config.get<bool>("hide_pause_button", false)) return true;
        
        auto menu = geode::cocos::getChildOfType<cocos2d::CCMenu>(this, 0);
        auto btn = geode::cocos::getChildOfType<CCMenuItemSpriteExtra>(menu, 0);

        if (menu && btn)
            btn->getNormalImage()->setVisible(false);

        return true;
    }
};

class $modify(CameraTriggerGameObject) {
    void triggerObject(GJBaseGameLayer *p0, int p1, const gd::vector<int> *p2) {
        auto& config = Config::get();
        if (config.get<bool>("no_camera_move", false)) return;

        CameraTriggerGameObject::triggerObject(p0, p1, p2);
    }
};

class $modify(ShaderLayer) {
    void visit() {
        auto& config = Config::get();
        if (config.get<bool>("no_shader_layer", false))
            return CCNode::visit();

        ShaderLayer::visit();
    }
};

class $modify(cocos2d::CCParticleSystemQuad) {
    void draw() {
        auto& config = Config::get();
        if (config.get<bool>("no_particles", false)) return;

        cocos2d::CCParticleSystemQuad::draw();
    }
};

class $modify(GameToolbox) {
    static gd::string intToShortString(int value) {
        auto& config = Config::get();
        if (!config.get<bool>("no_short_number", false))
            return GameToolbox::intToShortString(value);

        gd::string str = fmt::format("{}", value);
        return str;
    }
};

class $modify(LevelPage) {
    void onPlay(cocos2d::CCObject* sender) {
        auto coins = m_level->m_requiredCoins;

        if (Config::get().get<bool>("main_levels", false)) m_level->m_requiredCoins = 0;

        LevelPage::onPlay(sender);

        m_level->m_requiredCoins = coins;
    }
};

class $modify(LevelInfoLayer) {
    bool init(GJGameLevel *p0, bool p1) {
        if (!LevelInfoLayer::init(p0, p1))
            return false;

        if (!Config::get().get<bool>("copy_hack", false)) return true;

        auto gm = GameManager::sharedState();
        if (gm->m_playerUserID == p0->m_userID) return true;
        if (m_cloneBtn == nullptr) return true;

        auto cloneBtn = CCMenuItemSpriteExtra::create(cocos2d::CCSprite::createWithSpriteFrameName("GJ_duplicateBtn_001.png"), this, menu_selector(LevelInfoLayer::confirmClone));
        cloneBtn->setPosition(m_cloneBtn->getPosition());
        m_cloneBtn->getParent()->addChild(cloneBtn);
        m_cloneBtn->setVisible(false);
        
        return true;
    }

    void levelDownloadFinished(GJGameLevel* p0) {
        LevelInfoLayer::levelDownloadFinished(p0);

        if (!Config::get().get<bool>("copy_hack", false)) return;

        if (m_cloneBtn != nullptr) {
            m_cloneBtn->setVisible(false);
        }
    }
};

class $modify(PauseLayer) {
    void customSetup() {
        auto levelType = PlayLayer::get()->m_level->m_levelType;

        if (Config::get().get<bool>("level_edit", false))
            PlayLayer::get()->m_level->m_levelType = GJLevelType::Editor;

        PauseLayer::customSetup();

        PlayLayer::get()->m_level->m_levelType = levelType;
    }

    void onEdit(cocos2d::CCObject* sender) {
        auto a = PlayLayer::get()->m_level->m_levelType;

        if (Config::get().get<bool>("level_edit", false))
            PlayLayer::get()->m_level->m_levelType = GJLevelType::Editor;

        PauseLayer::onEdit(sender);

        PlayLayer::get()->m_level->m_levelType = a;
    }

    void musicSliderChanged(cocos2d::CCObject* sender) {
        auto& config = Config::get();

        PauseLayer::musicSliderChanged(sender);
        
        if (!Config::get().get<bool>("allow_low_volume", false))
            return;
        
        auto value = geode::cast::typeinfo_cast<SliderThumb*>(sender)->getValue();

        auto* audio_engine = FMODAudioEngine::sharedEngine();
        if (value < 0.03f)
            audio_engine->setBackgroundMusicVolume(value);
    }

    #ifdef GEODE_IS_ANDROID
    
    void sfxSliderChanged(cocos2d::CCObject* sender) {
        auto& config = Config::get();

        PauseLayer::sfxSliderChanged(sender);
        
        if (!hacks.allow_low_volume)
            return;

        auto value = geode::cast::typeinfo_cast<SliderThumb*>(sender)->getValue();

        auto* audio_engine = FMODAudioEngine::sharedEngine();
        if (value < 0.03f)
            audio_engine->setEffectsVolume(value);
    }

    #endif
};

class $modify(LevelTools) {
    static bool verifyLevelIntegrity(gd::string p0, int p1) {
        if (LevelTools::verifyLevelIntegrity(p0, p1))
            return true;
            
        return Config::get().get<bool>("level_edit", false);
    }
};

class $modify(EditLevelLayer) {
    void onShare(CCObject* sender) {
        if (Config::get().get<bool>("no_c_mark", false)) m_level->m_originalLevel = 0;

        EditLevelLayer::onShare(sender);
    }
};

class $modify(ShareLevelLayer) {
    struct Fields {
        GJGameLevel* m_level = nullptr;
    };

    bool init(GJGameLevel* p0) {
        m_fields->m_level = p0;

        return ShareLevelLayer::init(p0);
    }

    void onShare(cocos2d::CCObject* sender) {
        auto level = m_fields->m_level;
		auto isVerifiedRaw = level->m_isVerifiedRaw;
		auto isVerified = level->m_isVerified;

        if (Config::get().get<bool>("verify_hack", false)) {
		    level->m_isVerifiedRaw = true;
		    level->m_isVerified = true;
        }

		ShareLevelLayer::onShare(sender);

		level->m_isVerifiedRaw = isVerifiedRaw;
		level->m_isVerified = isVerified;
    }
};

class $modify(SongSelectNode) {
    void audioNext(cocos2d::CCObject* p0) {
        if (Config::get().get<bool>("default_song_bypass", false)) {
            m_selectedSongID++;
            getLevelSettings()->m_level->m_audioTrack = m_selectedSongID;

            return SongSelectNode::updateAudioLabel();
        }

        SongSelectNode::audioNext(p0);
    }

    void audioPrevious(cocos2d::CCObject* p0) {
        if (Config::get().get<bool>("default_song_bypass", false)) {
            m_selectedSongID--;
            getLevelSettings()->m_level->m_audioTrack = m_selectedSongID;
            
            return SongSelectNode::updateAudioLabel();
        }

        SongSelectNode::audioPrevious(p0);
    }
};

class $modify(LevelEditorLayer) {
    bool init(GJGameLevel *p0, bool p1) {
        bool ret = LevelEditorLayer::init(p0, p1);

        playerTrail1.clear();
        playerTrail2.clear();

        return ret;
    }

    void updateEditor(float dt) {
        LevelEditorLayer::updateEditor(dt);

        auto& config = Config::get();

        if (config.get<bool>("show_hitboxes", false) && config.get<bool>("draw_trail", false)) {            
            drawTrail(m_debugDrawNode);
        }
    }

    void onPlaytest() {
        LevelEditorLayer::onPlaytest();
        playerTrail1.clear();
        playerTrail2.clear();
    }
};


class $modify(OptionsLayer) {
    void musicSliderChanged(cocos2d::CCObject* sender) {
        auto& config = Config::get();

        OptionsLayer::musicSliderChanged(sender);
        
        if (!config.get<bool>("allow_low_volume", false))
            return;
        
        auto value = geode::cast::typeinfo_cast<SliderThumb*>(sender)->getValue();

        auto* audio_engine = FMODAudioEngine::sharedEngine();
        if (value < 0.03f)
            audio_engine->setBackgroundMusicVolume(value);
    }

    void sfxSliderChanged(cocos2d::CCObject* sender) {
        auto& config = Config::get();

        OptionsLayer::sfxSliderChanged(sender);
        
        if (!config.get<bool>("allow_low_volume", false))
            return;

        auto value = geode::cast::typeinfo_cast<SliderThumb*>(sender)->getValue();

        auto* audio_engine = FMODAudioEngine::sharedEngine();
        if (value < 0.03f)
            audio_engine->setEffectsVolume(value);
    }
};
