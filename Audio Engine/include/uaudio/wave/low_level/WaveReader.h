#pragma once

#include <complex>

#include <uaudio/wave/high_level/WaveConfig.h>

#include <uaudio/generic/UaudioResult.h>

namespace uaudio
{
    class WaveFormat;

    /*
	 * WHAT IS THIS FILE?
	 * This is the wave reader. It is responsible for loading the chunks of a wave file and creating a WaveFormat.
	 * It is also responsible for saving wave files.
	 * It uses the WaveConfig to determine which chunks need to be stored into memory.
     */
    class WaveReader
    {
    public:
        static UAUDIO_RESULT LoadSound(const char* a_FilePath, WaveFormat& a_WaveFormat, FILE*& a_File, WaveConfig a_WaveConfig = WaveConfig());
        static UAUDIO_RESULT SaveSound(const char* a_FilePath, const WaveFormat& a_WaveFormat);
    };
}
