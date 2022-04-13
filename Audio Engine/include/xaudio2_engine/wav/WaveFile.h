#pragma once

#include <complex>
#include <string>
#include <fstream>

#include "./WavFormat.h"

class WaveFile
{
public:
    WaveFile();
    WaveFile(const WaveFile &rhs);
    WaveFile(const char* a_FilePath);

    void AddAcidChunk(float a_Tempo);

    virtual ~WaveFile();

    uint32_t CalculateRiffChunkSize();

    WaveFile &operator=(const WaveFile &rhs);

    const char *GetSoundTitle() const;

    void Read(uint32_t a_StartingPoint, uint32_t &a_ElementCount, unsigned char *&a_Buffer) const;

    static float GetDuration(uint32_t a_ChunkSize, uint32_t a_ByteRate);
    static std::string FormatDuration(float a_Duration);
    bool IsEndOfFile(uint32_t a_StartingPoint) const;

    bool IsLooping() const;
    void SetLooping(bool a_Looping);

    void SetVolume(float a_Volume);
    float GetVolume() const;

    const WavFormat &GetWavFormat() const;
protected:
    void SetRIFFChunk(unsigned char a_ChunkId[4], uint32_t a_ChunkSize, unsigned char a_Format[4]);
    void SetFMTChunk(unsigned char a_ChunkId[4], uint32_t a_ChunkSize, uint16_t a_AudioFormat, uint16_t a_NumChannels, uint32_t a_SampleRate, uint32_t a_ByteRate, uint16_t a_BlockAlign, uint16_t a_BitsPerSample);
    void SetDataChunk(unsigned char a_ChunkId[4], uint32_t a_ChunkSize, unsigned char* a_Data);
    void SetAcidChunk(unsigned char a_ChunkId[4], uint32_t a_ChunkSize, uint32_t a_TypeOfFile, uint16_t a_RootNote, uint32_t a_NumOfBeats, uint16_t a_MeterDenominator, uint16_t a_MeterNumerator, float a_Tempo);
    void SetBextChunk(unsigned char a_ChunkId[4], uint32_t a_ChunkSize, char a_Description[256], char a_Originator[32], char a_OriginatorReference[32], char a_OriginationDate[10], char a_OriginationTime[8], uint32_t a_TimeReferenceLow, uint32_t a_TimeReferenceHigh, uint16_t a_Version, unsigned char a_Umid[64], uint16_t a_LoudnessValue, uint16_t a_LoudnessRange, uint16_t a_MaxTruePeakLevel, uint16_t a_MaxMomentaryLoudness, uint16_t a_MaxShortTermLoudness, unsigned char a_Reserved[180]);

    void Convert32To16();
    void Convert24To16();

    bool m_Looping = false;
    float m_Volume = 1.0f;

    FILE *m_File = nullptr;

    WavFormat m_WavFile = WavFormat();

    std::string m_SoundTitle = "";
};