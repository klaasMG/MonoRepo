#pragma once
#include "vendor/miniaudio/miniaudio.h"
#include <filesystem>
#include <vector>
#include <mutex>
namespace fs = std::filesystem;

enum class Channel : uint8_t{
    FrontLeft,
    FrontRight,
    FrontCenter,
    LFE,
    SurroundLeft,
    SurroundRight,
    BackLeft,
    BackRight
};

namespace AudioEngine {
    struct Config {
        uint16_t ChannelCount = 6;
        uint64_t SampleRate = 48000;
    };

    struct Voice {
        uint64_t at_sample;
        std::vector<float> samples;
        int16_t channels;
        std::vector<Channel> channel_layout;
    };

    class AudioManager {
    public:
        AudioManager();
        ~AudioManager();
        static void data_callback(ma_device* device, void* output, const void* input, ma_uint32 frameCount);
        void play_sound(const fs::path& path, const float& gain = 1.0);
        void set_gain(const float& gain);
        uint8_t voice_cycle = 0;
        Config config;
        std::mutex voice_mutex;
        std::vector<Voice> voices;
        std::vector<float> output_buffer;
        ma_device device;
        float gain = 1.0;
    };
}