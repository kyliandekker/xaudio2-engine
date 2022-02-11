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
    void SetRIFFChunk(unsigned char chunkId[4], uint32_t chunkSize, unsigned char format[4]);
    void SetFMTChunk(unsigned char chunkId[4], uint32_t chunkSize, uint16_t audioFormat, uint16_t numChannels, uint32_t sampleRate, uint32_t byteRate, uint16_t blockAlign, uint16_t bitsPerSample);
    void SetDataChunk(unsigned char chunkId[4], uint32_t chunkSize, unsigned char* data);
    void SetAcidChunk(unsigned char chunkId[4], uint32_t chunkSize, uint32_t type_of_file, uint16_t root_note, int num_of_beats, uint16_t meter_denominator, uint16_t meter_numerator, float tempo);
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