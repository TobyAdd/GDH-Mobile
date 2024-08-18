#include "replayEngine.hpp"
#include "hacks.hpp"

ReplayEngine engine;

unsigned ReplayEngine::get_frame() {
    auto& hacks = Hacks::get();
    auto pl = GameManager::sharedState()->getPlayLayer();

    if (pl)
        return static_cast<unsigned>(pl->m_gameState.m_levelTime * hacks.fps_value);
    return 0;
} 


void ReplayEngine::handle_recording(GJBaseGameLayer* self, bool player) {
    if (!player && !self->m_gameState.m_isDualMode)
        return;

    unsigned frame = get_frame();

    bool frameExists = std::find_if(replay.begin(), replay.end(), [&](const auto &data)
                                    { return data.frame == frame && data.player == player; }) != replay.end();

    if (frameExists)
    {
        return;
    }

    replay.push_back({frame,
                      player ? self->m_player1->m_position.x : self->m_player2->m_position.x,
                      player ? self->m_player1->m_position.y : self->m_player2->m_position.y,
                      player ? self->m_player1->getRotation() : self->m_player2->getRotation(),
                      player ? self->m_player1->m_yVelocity : self->m_player2->m_yVelocity,
                      player});
}

void ReplayEngine::handle_recording2(bool hold, int button, bool player) {
    replay2.push_back({get_frame(), hold, button, player});
}

void ReplayEngine::handle_playing(GJBaseGameLayer* self)
{
    unsigned frame = get_frame();

    if (accuracy_fix) {
        while (index < (int)replay.size() && frame >= replay[index].frame)
        {
            if (replay[index].player)
            {
                self->m_player1->m_position.x = replay[index].x;
                self->m_player1->m_position.y = replay[index].y;
                if (rotation_fix)
                    self->m_player1->setRotation(replay[index].rotation);
                self->m_player1->m_yVelocity = replay[index].y_accel;
            }
            else
            {
                self->m_player2->m_position.x = replay[index].x;
                self->m_player2->m_position.y = replay[index].y;
                if (rotation_fix)
                    self->m_player2->setRotation(replay[index].rotation);
                self->m_player2->m_yVelocity = replay[index].y_accel;
            }
            index++;
        }
    }


    while (index2 < (int)replay2.size() && frame >= replay2[index2].frame)
    {
        self->handleButton(replay2[index2].hold, replay2[index2].button, replay2[index2].player);
        index2++; 
    }
}

void ReplayEngine::handle_reseting(PlayLayer* self) {
    int lastCheckpointFrame = get_frame();

    if (mode == state::record) {
        remove_actions(lastCheckpointFrame);

        if (!replay2.empty() && replay2.back().hold) {
            handle_recording2(false, replay2.back().button, replay2.back().player);
        }
    }
    else if (mode == state::play) {
        index = 0;
        index2 = 0;
    }
}

std::string ReplayEngine::save(std::string name)
{
    if (replay2.empty())
        return "Replay doesn't have actions";

    auto& hacks = Hacks::get();
    std::ofstream file(folderMacroPath / std::string(name + ".re"), std::ios::binary);

    file.write(reinterpret_cast<char *>(&hacks.fps_value), sizeof(hacks.fps_value));

    unsigned replay_size = replay.size();
    unsigned replay2_size = replay2.size();

    file.write(reinterpret_cast<char *>(&replay_size), sizeof(replay_size));
    file.write(reinterpret_cast<char *>(&replay2_size), sizeof(replay2_size));

    file.write(reinterpret_cast<char *>(&replay[0]), sizeof(replay_data) * replay_size);
    file.write(reinterpret_cast<char *>(&replay2[0]), sizeof(replay_data2) * replay2_size);

    file.close();
    return "Replay saved";
}

std::string ReplayEngine::load(std::string name)
{
    if (!replay2.empty())
        return "Please clear replay before loading another";

    auto& hacks = Hacks::get();
    std::ifstream file(folderMacroPath / std::string(name + ".re"), std::ios::binary);
    if (!file)
        return "Replay doesn't exist";

    file.read(reinterpret_cast<char *>(&hacks.fps_value), sizeof(hacks.fps_value));

    unsigned replay_size = 0;
    unsigned replay2_size = 0;

    file.read(reinterpret_cast<char *>(&replay_size), sizeof(replay_size));
    file.read(reinterpret_cast<char *>(&replay2_size), sizeof(replay2_size));

    replay.resize(replay_size);
    replay2.resize(replay2_size);

    file.read(reinterpret_cast<char *>(&replay[0]), sizeof(replay_data) * replay_size);
    file.read(reinterpret_cast<char *>(&replay2[0]), sizeof(replay_data2) * replay2_size);

    file.close();
    return "Replay loaded";
}

void ReplayEngine::remove_actions(unsigned frame)
{
    auto check = [&](replay_data &action) -> bool
    {
        return action.frame > frame;
    };
    replay.erase(remove_if(replay.begin(), replay.end(), check), replay.end());

    auto check2 = [&](replay_data2 &action) -> bool
    {
        return action.frame >= frame;
    };
    replay2.erase(remove_if(replay2.begin(), replay2.end(), check2), replay2.end());
}