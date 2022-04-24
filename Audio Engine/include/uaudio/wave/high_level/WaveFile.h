#pragma once

#include <complex>
#include <string>

#include <uaudio/Includes.h>

#include <uaudio/wave/high_level/WaveConfig.h>
#include <uaudio/wave/low_level/WaveFormat.h>

namespace uaudio
{
    class WaveFile
    {
    public:
        WaveFile();
        WaveFile(const char *a_FilePath, const WaveConfig &a_WaveConfig);
        WaveFile(const WaveFile &rhs);

        WaveFile &operator=(const WaveFile &rhs);

        ~WaveFile() = default;

        void Read(uint32_t a_StartingPoint, uint32_t &a_ElementCount, unsigned char *&a_DataBuffer) const;

        bool IsEndOfBuffer(uint32_t a_StartingPoint) const;

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