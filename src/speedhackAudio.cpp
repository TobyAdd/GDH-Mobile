#include "speedhackAudio.hpp"
#include "hacks.hpp"

namespace speedhackAudio {
    void update() {
        auto& hacks = Hacks::get();

        FMOD::Channel *channel;
        FMOD::System *system = FMODAudioEngine::sharedEngine()->m_system;
        for (auto i = 0; i < 2; i++) {
            system->getChannel(126 + i, &channel);
            if (channel) {
                FMOD::Sound *sound;
                channel->getCurrentSound(&sound);
                float freq;
                sound->getDefaults(&freq, nullptr);
                if (hacks.speedhack_enabled && hacks.speedhack_audio)
                    channel->setFrequency(freq * hacks.speedhack_value);
                else
                    channel->setFrequency(freq * 1.f);
            }
        }
    }
}
