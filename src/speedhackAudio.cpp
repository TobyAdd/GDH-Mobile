#include "speedhackAudio.hpp"
#include "hacks.hpp"

namespace speedhackAudio {
    void update() {
        auto& config = Config::get();

        FMOD::Channel *channel;
        FMOD::System *system = FMODAudioEngine::sharedEngine()->m_system;
        for (auto i = 0; i < 2; i++) {
            system->getChannel(126 + i, &channel);
            if (channel) {
                FMOD::Sound *sound;
                channel->getCurrentSound(&sound);
                float freq;
                sound->getDefaults(&freq, nullptr);
                if (config.get<bool>("speedhack_enabled", false) && config.get<bool>("speedhack_audio", false))
                    channel->setFrequency(freq * config.get<float>("speedhack_value", 1.f));
                else
                    channel->setFrequency(freq * 1.f);
            }
        }
    }
}
