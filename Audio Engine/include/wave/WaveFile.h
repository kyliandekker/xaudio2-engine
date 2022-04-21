#pragma once

#include <complex>
#include <string>

#include "Includes.h"

#include "WaveConfig.h"
#include "WaveFormat.h"

namespace uaudio
{
    class WaveFile
    {
    public:
        WaveFile();
        WaveFile(const char *a_FilePath, const Wave_Config &a_WaveConfig);
        WaveFile(const WaveFile &rhs);

        WaveFile &operator=(const WaveFile &rhs);

        ~WaveFile() = default;

        const char *GetSoundTitle() const;

        void Read(uint32_t a_StartingPoint, uint32_t &a_ElementCount, unsigned char *&a_Buffer) const;

        static float GetDuration(uint32_t a_ChunkSize, uint32_t a_ByteRate);
        static std::string FormatDuration(float a_Duration, bool a_Milliseconds = true);
        bool IsEndOfFile(uint32_t a_StartingPoint) const;

        bool IsLooping() const;
        void SetLooping(bool a_Looping);

        void SetVolume(float a_Volume);
        float GetVolume() const;

        void SetEndPosition(uint32_t a_EndPosition);
        uint32_t GetEndPosition() const;

        void SetStartPosition(uint32_t a_StartPosition);
        uint32_t GetStartPosition() const;

        const WaveFormat &GetWaveFormat() const;

    protected:
        bool m_Looping = false;
        float m_Volume = UAUDIO_DEFAULT_VOLUME;

        uint32_t m_EndPosition = 0, m_StartPosition = 0;

        FILE *m_File = nullptr;

        WaveFormat m_WaveFormat = {};
    };
}