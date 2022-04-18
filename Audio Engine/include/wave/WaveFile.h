#pragma once

#include <complex>
#include <string>
#include <fstream>

#include "WaveReader.h"
#include "WaveFormat.h"

namespace uaudio
{
    class WaveFile
    {
    public:
        WaveFile();
        WaveFile(const char *a_FilePath, WAVE_CONFIG a_Config = WAVE_CONFIG());
        WaveFile(const WaveFile &rhs);

        WaveFile &operator=(const WaveFile &rhs);

        void LoadSound(const char *a_FilePath);

        void Save(const char *a_FilePath);

        virtual ~WaveFile();

        const char *GetSoundTitle() const;

        void Read(uint32_t a_StartingPoint, uint32_t &a_ElementCount, unsigned char *&a_Buffer) const;

        static float GetDuration(uint32_t a_ChunkSize, uint32_t a_ByteRate);
        static std::string FormatDuration(float a_Duration, bool a_Miliseconds = true);
        bool IsEndOfFile(uint32_t a_StartingPoint) const;

        bool IsLooping() const;
        void SetLooping(bool a_Looping);

        void SetVolume(float a_Volume);
        float GetVolume() const;

        const WaveFormat &GetWavFormat() const;

    protected:
        bool m_Looping = false;
        float m_Volume = 1.0f;

        FILE *m_File = nullptr;

        WaveFormat m_WaveFile = {};
        WAVE_CONFIG m_Config;
    };
}