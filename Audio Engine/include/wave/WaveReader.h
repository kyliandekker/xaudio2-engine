#pragma once

#include <complex>

#include "Chunks.h"
#include "WaveFormat.h"

#define DEFAULT_CHUNKS DATA_CHUNK_ID, FMT_CHUNK_ID

namespace uaudio
{
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
        static WAVE_LOADING_STATUS LoadSound(const char* a_FilePath, WaveFormat& a_WavFormat, FILE*& a_File, std::vector<const char*> a_Chunks = { DEFAULT_CHUNKS });
        static WAVE_SAVING_STATUS SaveSound(const char* a_FilePath, const WaveFormat& a_WaveFormat);
    };
}
