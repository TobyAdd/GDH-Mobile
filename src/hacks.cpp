#include "hacks.hpp"

void Hacks::save(const std::filesystem::path &path) {
    matjson::Value shit;
    
    shit["noclip"] = noclip;
    shit["unlock_items"] = unlock_items;
    shit["no_respawn_blink"] = no_respawn_blink;
    shit["no_death_effect"] = no_death_effect;
    shit["safe_mode"] = safe_mode;
    shit["allow_low_volume"] = allow_low_volume;
    shit["auto_pickup_coins"] = auto_pickup_coins;
    shit["jump_hack"] = jump_hack;
    shit["force_platformer"] = force_platformer;
    shit["hide_pause_button"] = hide_pause_button;
    shit["no_camera_move"] = no_camera_move;
    shit["no_camera_zoom"] = no_camera_zoom;
    shit["no_shader_layer"] = no_shader_layer;
    shit["no_particles"] = no_particles;
    shit["no_short_number"] = no_short_number;
    shit["no_glow"] = no_glow;
    shit["no_mirror_portal"] = no_mirror_portal;
    shit["no_lighting"] = no_lighting;
    shit["show_hitboxes"] = show_hitboxes;
    shit["draw_trail"] = draw_trail;
    shit["trail_size"] = trail_size;
    shit["main_levels"] = main_levels;
    shit["copy_hack"] = copy_hack;
    shit["level_edit"] = level_edit;
    shit["no_c_mark"] = no_c_mark;
    shit["verify_hack"] = verify_hack;
    shit["default_song_bypass"] = default_song_bypass;
    shit["scale_bypass"] = scale_bypass;
    shit["fps_value"] = fps_value;
    shit["speedhack_audio"] = speedhack_audio;
    shit["speedhack_value"] = speedhack_value;

    std::string file_contents = shit.dump();

    std::ofstream save_file(path);

    if (save_file.is_open()) {
        save_file << file_contents;
        save_file.close();
    }
}

void Hacks::load(const std::filesystem::path &path) {
    std::ifstream load_file(path);

    if (load_file.is_open()) {
        std::string file_contents((std::istreambuf_iterator<char>(load_file)), std::istreambuf_iterator<char>());
        load_file.close();

        auto shit = matjson::parse(file_contents);

        noclip = shit["noclip"].as<bool>();
        unlock_items = shit["unlock_items"].as<bool>();
        no_respawn_blink = shit["no_respawn_blink"].as<bool>();
        no_death_effect = shit["no_death_effect"].as<bool>();
        safe_mode = shit["safe_mode"].as<bool>();
        allow_low_volume = shit["allow_low_volume"].as<bool>();
        auto_pickup_coins = shit["auto_pickup_coins"].as<bool>();
        jump_hack = shit["jump_hack"].as<bool>();
        force_platformer = shit["force_platformer"].as<bool>();
        hide_pause_button = shit["hide_pause_button"].as<bool>();
        no_camera_move = shit["no_camera_move"].as<bool>();
        no_camera_zoom = shit["no_camera_zoom"].as<bool>();
        no_shader_layer = shit["no_shader_layer"].as<bool>();
        no_particles = shit["no_particles"].as<bool>();
        no_short_number = shit["no_short_number"].as<bool>();
        no_glow = shit["no_glow"].as<bool>();
        no_mirror_portal = shit["no_mirror_portal"].as<bool>();
        no_lighting = shit["no_lighting"].as<bool>();
        show_hitboxes = shit["show_hitboxes"].as<bool>();
        draw_trail = shit["draw_trail"].as<bool>();
        trail_size = shit["trail_size"].as<int>();
        main_levels = shit["main_levels"].as<bool>();
        copy_hack = shit["copy_hack"].as<bool>();
        level_edit = shit["level_edit"].as<bool>();
        no_c_mark = shit["no_c_mark"].as<bool>();
        verify_hack = shit["verify_hack"].as<bool>();
        default_song_bypass = shit["default_song_bypass"].as<bool>();
        scale_bypass = shit["scale_bypass"].as<bool>();
        fps_value = shit["fps_value"].as<float>();
        speedhack_audio = shit["speedhack_audio"].as<bool>();
        speedhack_value = shit["speedhack_value"].as<float>();
    }
}