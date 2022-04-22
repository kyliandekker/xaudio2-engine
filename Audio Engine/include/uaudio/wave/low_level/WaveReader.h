#pragma once

#include <complex>

#include <uaudio/wave/high_level/WaveConfig.h>

namespace uaudio
{
    class WaveFormat;

    enum class WAVE_LOADING_STATUS
    {
        STATUS_FAILED_OPENING_FILE,
        STATUS_FAILED_LOADING_CHUNK,
        STATUS_FAILED_NO_WAVE_FILE,
        STATUS_SUCCESSFUL,
    };
    enum class WAVE_SAVING_STATUS
    {
        STATUS_FAILED_OPENING_FILE,
        STATUS_SUCCESSFUL,
    };

    class WaveReader
    {
    public:
        static WAVE_LOADING_STATUS LoadSound(const char* a_FilePath, WaveFormat& a_WaveFormat, FILE*& a_File, Wave_Config a_WaveConfig = Wave_Config());
        static WAVE_SAVING_STATUS SaveSound(const char* a_FilePath, const WaveFormat& a_WaveFormat);
    };
}
