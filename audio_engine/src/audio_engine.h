#pragma once
#include "vendor/miniaudio/miniaudio.h"
#include <filesystem>
#include <vector>
namespace fs = std::filesystem;

namespace AudioEngine {
    struct Config {
        uint16_t ChannelCount = 2;
        uint64_t SampleRate = 48000;
    };

    struct Voice {
        uint64_t at_sample;
        std::vector<float> samples;
        int16_t channels;
    };

    class AudioManager {
    public:
        AudioManager();
        ~AudioManager();
        static void data_callback(ma_device* device, void* output, const void* input, ma_uint32 frameCount);
        void play_sound(const fs::path& path);
        Config config;
        std::vector<Voice> voices;
        std::vector<float> output_buffer;
        ma_device device;
    };
}