#pragma once

#include <complex>
#include <fstream>

#include "WavFormat.h"

namespace uaudio
{
    class WaveReader
    {
    public:
        static WavFormat LoadSound(const char* a_FilePath);
    protected:
        static uint32_t CalculateRiffChunkSize(WavFormat& a_WavFormat);

        static void SetRIFFChunk(WavFormat& a_WavFormat, unsigned char a_ChunkId[CHUNK_ID_SIZE], uint32_t a_ChunkSize, unsigned char a_Format[4]);
        static void SetFMTChunk(WavFormat& a_WavFormat, unsigned char a_ChunkId[CHUNK_ID_SIZE], uint32_t a_ChunkSize, uint16_t a_AudioFormat, uint16_t a_NumChannels, uint32_t a_SampleRate, uint32_t a_ByteRate, uint16_t a_BlockAlign, uint16_t a_BitsPerSample);
        static void SetDataChunk(WavFormat& a_WavFormat, unsigned char a_ChunkId[CHUNK_ID_SIZE], uint32_t a_ChunkSize, unsigned char* a_Data);
        static void SetAcidChunk(WavFormat& a_WavFormat, unsigned char a_ChunkId[CHUNK_ID_SIZE], uint32_t a_ChunkSize, uint32_t a_TypeOfFile, uint16_t a_RootNote, uint32_t a_NumOfBeats, uint16_t a_MeterDenominator, uint16_t a_MeterNumerator, float a_Tempo);
        static void SetBextChunk(WavFormat& a_WavFormat, unsigned char a_ChunkId[CHUNK_ID_SIZE], uint32_t a_ChunkSize, char a_Description[256], char a_Originator[32], char a_OriginatorReference[32], char a_OriginationDate[10], char a_OriginationTime[8], uint32_t a_TimeReferenceLow, uint32_t a_TimeReferenceHigh, uint16_t a_Version, unsigned char a_Umid[64], uint16_t a_LoudnessValue, uint16_t a_LoudnessRange, uint16_t a_MaxTruePeakLevel, uint16_t a_MaxMomentaryLoudness, uint16_t a_MaxShortTermLoudness, unsigned char a_Reserved[180]);

        static void Convert32To16(WavFormat& a_WavFormat);
        static void Convert24To16(WavFormat& a_WavFormat);
    };
}