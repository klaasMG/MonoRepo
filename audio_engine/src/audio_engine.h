#pragma once
#include "vendor/miniaudio/miniaudio.h"
#include <filesystem>
namespace fs = std::filesystem;

namespace AudioEngine {
    class Voice {
        public:
        Voice();
    };

    class AudioManager {
    public:
        AudioManager();
        void play_sound();
    };
}