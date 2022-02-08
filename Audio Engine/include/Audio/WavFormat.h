#pragma once

// http://soundfile.sapp.org/doc/WaveFormat/
class WavFormat
{
public:
    // riff
    unsigned char chunkId[4]; // RIFF
    uint32_t chunkSize; // Riff chunk size.
    unsigned char format[4]; // WAVE
    // fmt
    unsigned char subchunk1Id[4]; // fmt
    uint32_t subchunk1Size; // fmt chunk size
    uint16_t audioFormat; // 1
    uint16_t numChannels; // 2
    uint32_t sampleRate; // 
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
    // data
    unsigned char subchunk2Id[4];
    uint32_t subchunk2Size;
    unsigned char* data;
    uint32_t bufferSize;
};