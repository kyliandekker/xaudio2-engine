#pragma once

#include <complex>

#include "WaveFormat.h"
#include "Wave_Config.h"

namespace uaudio
{
    enum class WAVE_LOADING_STATUS
    {
        STATUS_FAILED_OPENING_FILE,
        STATUS_FAILED_LOADING_CHUNK,
        STATUS_FAILED_NO_WAVE_FILE,
        STATUS_SUCCESSFUL,
    };

    class WaveReader
    {
    public:
        static WAVE_LOADING_STATUS LoadSound(const char *a_FilePath, WaveFormat &a_WavFormat, FILE *&a_File, WAVE_CONFIG a_WavConfig = UAUDIO_DEFAULT_WAV_CONFIG);

    protected:
        static void SetRIFFChunk(WaveFormat &a_WavFormat, unsigned char a_ChunkId[CHUNK_ID_SIZE], uint32_t a_ChunkSize, unsigned char a_Format[4]);
        static void SetFMTChunk(WaveFormat &a_WavFormat, unsigned char a_ChunkId[CHUNK_ID_SIZE], uint32_t a_ChunkSize, uint16_t a_AudioFormat, uint16_t a_NumChannels, uint32_t a_SampleRate, uint32_t a_ByteRate, uint16_t a_BlockAlign, uint16_t a_BitsPerSample);
        static void SetDataChunk(WaveFormat &a_WavFormat, unsigned char a_ChunkId[CHUNK_ID_SIZE], uint32_t a_ChunkSize, unsigned char *a_Data);
        static void SetAcidChunk(WaveFormat &a_WavFormat, unsigned char a_ChunkId[CHUNK_ID_SIZE], uint32_t a_ChunkSize, uint32_t a_TypeOfFile, uint16_t a_RootNote, uint32_t a_NumOfBeats, uint16_t a_MeterDenominator, uint16_t a_MeterNumerator, float a_Tempo);
        static void SetBextChunk(WaveFormat &a_WavFormat, unsigned char a_ChunkId[CHUNK_ID_SIZE], uint32_t a_ChunkSize, char a_Description[256], char a_Originator[32], char a_OriginatorReference[32], char a_OriginationDate[10], char a_OriginationTime[8], uint32_t a_TimeReferenceLow, uint32_t a_TimeReferenceHigh, uint16_t a_Version, unsigned char a_Umid[64], uint16_t a_LoudnessValue, uint16_t a_LoudnessRange, uint16_t a_MaxTruePeakLevel, uint16_t a_MaxMomentaryLoudness, uint16_t a_MaxShortTermLoudness, unsigned char a_Reserved[180]);
        static void SetFACTChunk(WaveFormat &a_WavFormat, unsigned char a_ChunkId[CHUNK_ID_SIZE], uint32_t a_ChunkSize, uint32_t a_SampleLength);
        static void SetSMPLChunk(WaveFormat &a_WavFormat, unsigned char a_ChunkId[CHUNK_ID_SIZE], uint32_t a_ChunkSize, uint32_t a_Manufacturer, uint32_t a_Product, uint32_t a_SamplePeriod, uint32_t a_MidiUnityNode, uint32_t a_MidiPitchFraction, uint32_t a_SmpteFormat, uint32_t a_SmpteOffset, uint32_t a_NumSampleLoops, uint32_t a_SamplerData, SMPL_Sample_Loop *a_Samples);
        static void SetCUEChunk(WaveFormat &a_WavFormat, unsigned char a_ChunkId[CHUNK_ID_SIZE], uint32_t a_ChunkSize, uint32_t a_NumCuePoints, CUE_Point *a_QuePoints);

        static void Convert32To16(WaveFormat &a_WavFormat);
        static void Convert24To16(WaveFormat &a_WavFormat);
    };
}
