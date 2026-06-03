#define MINIAUDIO_IMPLEMENTATION
#include "audio_engine.h"

namespace AudioEngine {
    AudioManager::AudioManager() {
        config = Config();
        voices = {};
        output_buffer = {};
        ma_device_config ma_device_config = ma_device_config_init(ma_device_type_playback);
        ma_device_config.playback.format = ma_format_f32;
        ma_device_config.playback.channels = 6;
        ma_device_config.sampleRate = 48000;
        ma_device_config.dataCallback = data_callback;
        ma_device_config.pUserData = this;
        ma_channel channelMap[6] = {MA_CHANNEL_FRONT_LEFT, MA_CHANNEL_FRONT_RIGHT, MA_CHANNEL_FRONT_CENTER, MA_CHANNEL_LFE, MA_CHANNEL_SIDE_LEFT, MA_CHANNEL_SIDE_RIGHT};
        ma_device_config.playback.pChannelMap = channelMap;
        if (ma_device_init(nullptr, &ma_device_config, &device) != MA_SUCCESS){
            throw std::runtime_error("Failed to initialize audio device.");
        }
        if (ma_device_start(&device) != MA_SUCCESS){
            throw std::runtime_error("Failed to start audio device.");
        }
    }

    AudioManager::~AudioManager(){
        ma_device_uninit(&device);
    }

    void AudioManager::play_sound(const fs::path& path) {
        std::lock_guard<std::mutex> lock(voice_mutex);
        if (!exists(path)) {
            throw std::runtime_error("that is why");
        }

        ma_channel decoderChannelMap[6] = {MA_CHANNEL_FRONT_LEFT, MA_CHANNEL_FRONT_RIGHT, MA_CHANNEL_FRONT_CENTER, MA_CHANNEL_LFE, MA_CHANNEL_SIDE_LEFT, MA_CHANNEL_SIDE_RIGHT};
        ma_decoder_config audio_config = ma_decoder_config_init(ma_format_f32,6,48000);
        audio_config.pChannelMap = decoderChannelMap;
        ma_decoder decoder;


        if (ma_decoder_init_file(path.string().c_str(), &audio_config, &decoder) != MA_SUCCESS){
            throw std::runtime_error("Failed to load audio file.");
        }
        ma_uint64 frame_count = 0;
        if (ma_decoder_get_length_in_pcm_frames(&decoder, &frame_count) != MA_SUCCESS){
            ma_decoder_uninit(&decoder);
            throw std::runtime_error("Failed to determine audio length.");
        }
        std::vector<float> samples(
            static_cast<size_t>(frame_count * decoder.outputChannels)
        );
        ma_uint64 frames_read = 0;
        ma_decoder_read_pcm_frames(&decoder,samples.data(),frame_count,&frames_read);
        samples.resize(static_cast<size_t>(frames_read * decoder.outputChannels));
        Voice voice = Voice{.at_sample = 0, .samples = samples, .channels = static_cast<int16_t>(decoder.outputChannels)};
        ma_decoder_uninit(&decoder);
        voices.push_back(voice);
    }

    void AudioManager::data_callback(ma_device* device,void* output,const void* input, ma_uint32 frameCount){
        auto* manager = static_cast<AudioManager*>(device->pUserData);
        std::lock_guard<std::mutex> lock(manager->voice_mutex);
        float* out = static_cast<float*>(output);

        for (ma_uint32 frame = 0; frame < frameCount; frame++){
            float left = 0.0f;
            float right = 0.0f;
            float center = 0.0f;
            float low_frequency = 0.0f;
            float surround_left = 0.0f;
            float surround_right = 0.0f;

            for (Voice& voice : manager->voices){
                if (voice.at_sample + 1 >= voice.samples.size()){
                    continue;
                }
                float sample = voice.samples.at(voice.at_sample);
                left += sample;
                voice.at_sample++;

                float sample1 = voice.samples.at(voice.at_sample);
                right += sample1;
                voice.at_sample++;

                float sample2 = voice.samples.at(voice.at_sample);
                center += sample2;
                voice.at_sample++;

                float sample3 = voice.samples.at(voice.at_sample);
                low_frequency += sample3;
                voice.at_sample++;

                float sample4 = voice.samples.at(voice.at_sample);
                surround_left += sample4;
                voice.at_sample++;

                float sample5 = voice.samples.at(voice.at_sample);
                surround_right += sample5;
                voice.at_sample++;
            }

            out[frame * 6] = left;
            out[frame * 6 + 1] = right;
            out[frame * 6 + 2] = center;
            out[frame * 6 + 3] = low_frequency;
            out[frame * 6 + 4] = surround_left;
            out[frame * 6 + 5] = surround_right;
        }
    }
}
