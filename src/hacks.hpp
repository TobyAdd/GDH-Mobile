#pragma once
#include <map>
#include <string>

static const auto folderPath = geode::Mod::get()->getSaveDir();
static const auto folderMacroPath = folderPath / "Macros";
static const auto fileDataPath = folderPath / "config.json";


class Hacks {
public:
    static Hacks& get() {
        static Hacks instance;
        return instance;
    }

    Hacks& operator=(const Hacks&) = delete;
    Hacks(const Hacks&) = delete;
    
    bool noclip = false;
    bool unlock_items = false;
    bool no_respawn_blink = false;
    bool no_death_effect = false;
    bool safe_mode = false;

    bool allow_low_volume = false;
    bool auto_pickup_coins = false;
    bool jump_hack = false;
    bool force_platformer = false;
    bool hide_pause_button = false;
    bool no_camera_move = false;
    bool no_camera_zoom = false;
    bool no_shader_layer = false;
    bool no_particles = false;
    bool no_short_number = false;
    bool no_bg_flash = false;
    bool no_glow = false;
    bool no_mirror_portal = false;
    bool no_portal_lighting = false;

    bool show_hitboxes = false;
    bool draw_trail = false;
    int trail_size = 240;
    
    bool main_levels = false;

    bool copy_hack = false;
    bool level_edit = false;
    bool no_c_mark = false;
    bool verify_hack = false;
    bool default_song_bypass = false;
    bool scale_bypass = false;

    bool fps_enabled = false;
    float fps_value = 240.f;

    bool speedhack_enabled = false;
    bool speedhack_audio = false;
    float speedhack_value = 1.0f;

private:
    Hacks() = default;

protected:
    std::map<std::string, bool> m_hacks{};
};