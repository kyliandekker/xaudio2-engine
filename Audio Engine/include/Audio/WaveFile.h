#pragma once

#include <complex>
#include <string>
#include <fstream>
#include <vector>

#include "WavFormat.h"

class WaveFile
{
public:
    WaveFile();
    WaveFile(const WaveFile& rhs);
    WaveFile(const char* a_FilePath);
    void Convert32To16();
    void Convert24To16();
    virtual ~WaveFile();

    WaveFile& operator=(const WaveFile& rhs);

    float GetFFTDuration(int a_BlockLength) const;
    float GetFFTFrequencyResolution(int a_BlockLength) const;

    const char* GetSoundTitle() const;
    void Read(uint32_t a_StartingPoint, uint32_t& a_ElementCount, unsigned char*& a_Buffer) const;
    float GetDuration() const;
    static std::string FormatDuration(float a_Duration);
    bool IsEndOfFile(uint32_t a_StartingPoint) const;
    bool IsLooping() const;
    void SetLooping(bool a_Looping);
    void SetVolume(float a_Volume);
    float GetVolume() const;
    const WavFormat& GetWavFormat() const;
    bool GetChunk(std::vector<std::complex<double>>& signal);
protected:
    bool m_Looping = false;
    float m_Volume = 1.0f;

    FILE* m_File = nullptr;

    WavFormat m_WavFile = WavFormat();

    std::string m_SoundTitle = "";

    int m_prevEnd = 0;
};