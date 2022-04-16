#pragma once

#include <complex>
#include <string>
#include <fstream>

#include "WavFormat.h"

namespace uaudio
{
    class WaveFile
    {
    public:
        WaveFile();
        WaveFile(const char* a_FilePath);
        WaveFile(const WaveFile& rhs);

        WaveFile& operator=(const WaveFile& rhs);

        void LoadSound(const char* a_FilePath);

        void AddAcidChunk(float a_Tempo);

        virtual ~WaveFile();

        const char* GetSoundTitle() const;

        void Read(uint32_t a_StartingPoint, uint32_t& a_ElementCount, unsigned char*& a_Buffer) const;

        static float GetDuration(uint32_t a_ChunkSize, uint32_t a_ByteRate);
        static std::string FormatDuration(float a_Duration);
        bool IsEndOfFile(uint32_t a_StartingPoint) const;

        bool IsLooping() const;
        void SetLooping(bool a_Looping);

        void SetVolume(float a_Volume);
        float GetVolume() const;

        const WavFormat& GetWavFormat() const;
    protected:
        bool m_Looping = false;
        float m_Volume = 1.0f;

        WavFormat m_WavFile = {};
    };
}