#pragma once

#include <dsound.h>
#include <string>
#include <vector>

class BGM
{
public:
    static BGM* get_ton();
    static void initialize(HWND hwnd);
    static void finalize(); // For memory leak check.

    bool load(const std::string& filename);
    void play(const int a_volume = 100);
    void stop();
private:
    static BGM* single_ton_;
    // hide ctor
    BGM(HWND hwnd);
    BGM(const BGM&) { }
    void operator=(const BGM&) { }

    bool open_wave(
        const std::string& filepath,
        WAVEFORMATEX& waveFormatEx,
        std::vector<char>* ppData,
        DWORD& dataSize);

    int per_to_decibel(const int percent);

    LPDIRECTSOUND8 dx8sound_ { nullptr };
    LPDIRECTSOUNDBUFFER8 dx8sound_buffer_ { nullptr };
};

