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
    WaveFile(const WaveFile &rhs);
    void SetRIFFChunk(unsigned char chunkId[4], uint32_t chunkSize, unsigned char format[4]);
    void SetFMTChunk(unsigned char chunkId[4], uint32_t chunkSize, uint16_t audioFormat, uint16_t numChannels, uint32_t sampleRate, uint32_t byteRate, uint16_t blockAlign, uint16_t bitsPerSample);
    void SetDataChunk(unsigned char chunkId[4], uint32_t chunkSize, unsigned char *data);
    void SetAcidChunk(unsigned char chunkId[4], uint32_t chunkSize, uint32_t type_of_file, uint16_t root_note, uint32_t num_of_beats, uint16_t meter_denominator, uint16_t meter_numerator, float tempo);
    void SetBextChunk(unsigned char chunkId[4], uint32_t chunkSize, unsigned char description[256], unsigned char originator[32], unsigned char originator_reference[32], unsigned char origination_date[10], unsigned char origination_time[8], uint32_t time_reference_low, uint32_t time_reference_high, uint16_t version, unsigned char umid[64], uint16_t loudness_value, uint16_t loudness_range, uint16_t max_true_peak_level, uint16_t max_momentary_loudness, uint16_t max_short_term_loudness, unsigned char reserved[180]);
    WaveFile(const char *a_FilePath);
    void Convert32To16();
    void Convert24To16();
    virtual ~WaveFile();

    WaveFile &operator=(const WaveFile &rhs);

    const char *GetSoundTitle() const;
    void Read(uint32_t a_StartingPoint, uint32_t &a_ElementCount, unsigned char *&a_Buffer) const;
    float GetDuration() const;
    static std::string FormatDuration(float a_Duration);
    bool IsEndOfFile(uint32_t a_StartingPoint) const;
    bool IsLooping() const;
    void SetLooping(bool a_Looping);
    void SetVolume(float a_Volume);
    float GetVolume() const;
    const WavFormat &GetWavFormat() const;

protected:
    bool m_Looping = false;
    float m_Volume = 1.0f;

    FILE *m_File = nullptr;

    WavFormat m_WavFile = WavFormat();

    std::string m_SoundTitle = "";
};