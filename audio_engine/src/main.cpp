#include "audio_engine.h"
#include <thread>
#include <chrono>

int main(int argc, char* argv[]) {
    AudioEngine::AudioManager audio_manager;
    audio_manager.play_sound("audio_engine/src/pure-tone.wav");
    std::this_thread::sleep_for(std::chrono::seconds(100));
}
