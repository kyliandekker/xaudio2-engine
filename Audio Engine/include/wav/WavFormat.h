#pragma once

#include <intsafe.h>
#include <string>
#include <vector>

#include "Includes.h"

namespace uaudio
{
#define CHUNK_ID_SIZE 4

    // http://soundfile.sapp.org/doc/WaveFormat/
    // https://www.kvraudio.com/forum/viewtopic.php?t=172636
    // https://tech.ebu.ch/docs/tech/tech3285.pdf
    // http://www-mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html

#pragma pack(push, 1)
    class Chunk
    {
    public:
        Chunk() = default;
        Chunk(unsigned char a_ChunkID[CHUNK_ID_SIZE], uint32_t a_ChunkSize);
        Chunk(const Chunk &rhs);

        ~Chunk();

        Chunk &operator=(const Chunk &rhs);

        unsigned char chunkId[CHUNK_ID_SIZE] = {};
        uint32_t chunkSize = 0;
    };

    class RIFF_Chunk : public Chunk
    {
    public:
        RIFF_Chunk();
        RIFF_Chunk(unsigned char a_ChunkID[CHUNK_ID_SIZE], uint32_t a_ChunkSize, unsigned char a_Format[CHUNK_ID_SIZE]);
        RIFF_Chunk(const RIFF_Chunk &rhs);

        ~RIFF_Chunk();

        RIFF_Chunk &operator=(const RIFF_Chunk &rhs);

        unsigned char format[CHUNK_ID_SIZE] = {};
    };

    /*
     * ** The riff chunk goes a little something like this: TOTAL SIZE: 4 + 8 (chunkid and chunksize)
     * ** but the chunksize is actually everything from the riff chunk to the end of the file.
     **
     ** 4 bytes                     Char array saying 'RIFF'.
     ** 4 bytes (char * 4)          Usually says WAVE.
     **
     */

    class FMT_Chunk : public Chunk
    {
    public:
        FMT_Chunk();
        FMT_Chunk(unsigned char a_ChunkID[CHUNK_ID_SIZE], uint32_t a_ChunkSize, uint16_t a_AudioFormat, uint16_t a_NumChannels, uint32_t a_SampleRate, uint32_t a_ByteRate, uint16_t a_BlockAlign, uint16_t a_BitsPerSample);
        FMT_Chunk(const FMT_Chunk &rhs);

        ~FMT_Chunk();

        FMT_Chunk &operator=(const FMT_Chunk &rhs);

        uint16_t audioFormat = 0;
        uint16_t numChannels = 0;
        uint32_t sampleRate = 0;
        uint32_t byteRate = 0;
        uint16_t blockAlign = 0;
        uint16_t bitsPerSample = 0;
    };

    /*
     * ** The fmt chunk goes a little something like this: TOTAL SIZE: 16 + 8 (chunkid and chunksize)
     **
     ** 4 bytes                     Char array saying 'fmt '.
     ** 4 bytes (long)              Length of chunk starting at next byte.
     **
     ** 2 bytes (short)             Format of the audio.
     **                                 1 means PCM (WAVE_FORMAT_PCM)
     **                                 Anything else means it is compressed a bit.
     **
     ** 2 bytes (short)             Number of channels.
     **                                 1 means mono (no stereo info).
     **                                 2 means stereo.
     **
     ** 4 bytes (long)              The sample rate/sampling frequency/sampling rate/samples per seconds/hertz
     **                                 This is in Hz.
     **
     ** 4 bytes (long)              The byte rate/bytes per second.
     **
     ** 2 bytes (short)             The number of bytes for one sample including all channels.
     **
     ** 2 bytes (short)             The bits per sample. (8-bit, 16-bit, 24-bit, 32-bit)
     **
     **
     */

    class DATA_Chunk : public Chunk
    {
    public:
        DATA_Chunk();
        DATA_Chunk(unsigned char a_ChunkID[CHUNK_ID_SIZE], uint32_t a_ChunkSize, unsigned char *a_Data);
        DATA_Chunk(const DATA_Chunk &rhs);

        ~DATA_Chunk();

        DATA_Chunk &operator=(const DATA_Chunk &rhs);

        unsigned char *data = nullptr;
    };

    /*
     * ** The data chunk goes a little something like this: ? + 8 (chunkid and chunksize)
     **
     ** 4 bytes                     Char array saying 'data'.
     ** 4 bytes (long)              Length of chunk starting at next byte.
     **
     ** ? bytes (?)                 Actual wav data:
     **                                 can be 24-bit integers, 16-bit integers, 8-bit integers or floats.
     **
     */

    class ACID_Chunk : public Chunk
    {
    public:
        ACID_Chunk();
        ACID_Chunk(
            unsigned char a_ChunkID[CHUNK_ID_SIZE],
            uint32_t a_ChunkSize,
            uint32_t a_TypeOfFile,
            uint16_t a_RootNote,
            uint16_t a_Unknown1,
            float a_Unknown2,
            uint32_t a_NumBeats,
            uint16_t a_MeterDenominator,
            uint16_t a_MeterNumerator,
            float a_Tempo);
        ACID_Chunk(const ACID_Chunk &rhs);

        ~ACID_Chunk();

        ACID_Chunk &operator=(const ACID_Chunk &rhs);

        uint32_t typeOfFile = 0x5;
        uint16_t rootNote = 0x3c;
        uint16_t unknown1 = 0x8000;
        float unknown2 = 0;
        uint32_t num_of_beats = 0;
        uint16_t meter_denominator = 4;
        uint16_t meter_numerator = 4;
        float tempo = 0.0f;
    };

    /*
     * ** The acid chunk goes a little something like this: TOTAL SIZE: 24 + 8 (chunkid and chunksize)
     **
     ** 4 bytes                     Char array saying 'acid'.
     ** 4 bytes (long)              Length of chunk starting at next byte.
     **
     ** 4 bytes (long)              type of file:
     **                                 this appears to be a bit mask, however some combinations
     **                                 are probably impossible and/or qualified as "errors".
     **
     **                                 0x01 On: One Shot         Off: Loop
     **                                 0x02 On: Root note is Set Off: No root
     **                                 0x04 On: Stretch is On,   Off: Strech is OFF
     **                                 0x08 On: Disk Based       Off: Ram based
     **                                 0x10 On: ??????????       Off: ????????? (Acidizer puts that ON)
     **
     ** 2 bytes (short)             root note
     **                                 if type 0x10 is OFF : [C,C#,(...),B] -> [0x30 to 0x3B]
     **                                 if type 0x10 is ON  : [C,C#,(...),B] -> [0x3C to 0x47]
     **                                 (both types fit on same MIDI pitch albeit different octaves, so who cares)
     **
     ** 2 bytes (short)             ??? always set to 0x8000.
     ** 4 bytes (float)             ??? seems to be always 0.
     ** 4 bytes (long)              number of beats.
     ** 2 bytes (short)             meter denominator.   //always 4 in SF/ACID
     ** 2 bytes (short)             meter numerator.     //always 4 in SF/ACID
     **                                 //are we sure about the order?? usually its num/denom
     ** 4 bytes (float)             The bpm (tempo).
     **
     */

    class BEXT_Chunk : public Chunk
    {
    public:
        BEXT_Chunk();
        BEXT_Chunk(
            unsigned char a_ChunkID[CHUNK_ID_SIZE],
            uint32_t a_ChunkSize,
            char a_Description[256],
            char a_Originator[32],
            char a_OriginatorReference[32],
            char a_OriginationDate[10],
            char a_OriginationTime[8],
            uint32_t a_TimeReferenceLow,
            uint32_t a_TimeReferenceHigh,
            uint16_t a_Version,
            unsigned char a_Umid[64],
            uint16_t a_LoudnessValue,
            uint16_t a_LoudnessRange,
            uint16_t a_MaxTruePeakLevel,
            uint16_t a_MaxMomentaryLoudness,
            uint16_t a_MaxShortTermLoudness,
            unsigned char a_Reserved[180]);
        BEXT_Chunk(const BEXT_Chunk &rhs);

        ~BEXT_Chunk();

        BEXT_Chunk &operator=(const BEXT_Chunk &rhs);

        char description[256] = {};
        char originator[32] = {};
        char originator_reference[32] = {};
        char origination_date[10] = {};
        char origination_time[8] = {};
        uint32_t time_reference_low = 0;
        uint32_t time_reference_high = 0;
        uint16_t version = 0;
        unsigned char umid[64] = {};
        uint16_t loudness_value = 0;
        uint16_t loudness_range = 0;
        uint16_t max_true_peak_level = 0;
        uint16_t max_momentary_loudness = 0;
        uint16_t max_short_term_loudness = 0;
        unsigned char reserved[180] = {};
    };

    /*
     * ** The bext chunk goes a little something like this: TOTAL SIZE: 602/604 + 8 (chunkid and chunksize)
     **
     ** 4 bytes                     Char array saying 'bext'.
     ** 4 bytes (long)              Length of chunk starting at next byte.
     **
     ** 256 bytes (char * 256)      Description of the file.
     **
     ** 32 bytes (char * 32)        Contains the name of the originator/ producer of the audio file.
     ** 32 bytes (char * 32)        Unambiguous reference allocated by the originating organization.
     **
     ** 10 bytes (char * 10)        The date of creation of the audio sequence.
     **                                 The format shall be « ‘,year’,-,’month,’-‘,day,’»
     **								    with 4 characters for the year and 2 characters per other item.
     **								        Year is defined from 0000 to 9999
     **								        Month is defined from 1 to 12
     **								        Day is defined from 1 to 28, 29, 30 or 31
     **                                         The separator between the items can be anything but it is recommended
     **											that one of the following characters be used:
     **											‘-’ hyphen ‘_’ underscore ‘:’ colon ‘ ’ space ‘.’ stop
     **
     ** 8 bytes (char * 8)          Contains the time of creation of the audio sequence.
     **									The format shall be « ‘hour’-‘minute’-‘second’» with 2 characters per item.
     **										Hour is defined from 0 to 23.
     **										Minute and second are defined from 0 to 59.
     **											The separator between the items can be anything but it is recommended
     **											that one of the following characters be used:
     **											‘-’ hyphen ‘_’ underscore ‘:’ colon ‘ ’ space ‘.’ stop
     **
     ** 4 bytes (long)              Contains the time-code of the sequence. It is a 64-bit
     **								value which contains the first sample count since midnight. The number
     **								of samples per second depends on the sample frequency which is defined
     **								in the field <nSamplesPerSec> from the <format chunk>.
     **
     ** 4 bytes (long)              Contains the time-code of the sequence. It is a 64-bit
     **								value which contains the first sample count since midnight. The number
     **								of samples per second depends on the sample frequency which is defined
     **								in the field <nSamplesPerSec> from the <format chunk>.
     **
     ** 2 bytes (short)             An unsigned binary number giving the version of the BWF. This number is
     **								particularly relevant for the carriage of the UMID and loudness
     **								information. For Version 1 it shall be set to 0001h and for Version 2 it
     **								shall be set to 0002h.
     **
     ** 64 bytes (uchar * 64)       Contains a UMID (Unique Material Identifier) to standard
     **								SMPTE 330M [1]. If only a 32 byte "basic UMID" is used, the last 32 bytes
     **								should be set to zero. (The length of the UMID is given internally.)
     **
     ** 2 bytes (short)             A 16-bit signed integer, equal to round(100x the Integrated Loudness
     **								Value of the file in LUFS).
     **
     ** 2 bytes (short)             A 16-bit signed integer, equal to round(100x the Loudness Range of the
     **								file in LU).
     **
     ** 2 bytes (short)             A 16-bit signed integer, equal to round(100x the Maximum True Peak
     **								Value of the file in dBTP).
     **
     ** 2 bytes (short)             A 16-bit signed integer, equal to round(100x the highest value of the
     **								Momentary Loudness Level of the file in LUFS).
     **
     ** 2 bytes (short)             A 16-bit signed integer, equal to round(100x the highest value of the
     **								Short-term Loudness Level of the file in LUFS).
     **
     ** 180 bytes (char * 180)      180 bytes reserved for extension. If the Version field is set to 0001h or
     **								0002h, these 180 bytes shall be set to a NULL (zero) value.
     **
     */

#pragma pack(pop)

    class WavFormat
    {
    public:
        WavFormat() = default;
        WavFormat(const WavFormat &rhs);

        ~WavFormat();

        WavFormat &operator=(const WavFormat &rhs);

        // riff
        RIFF_Chunk riffChunk = RIFF_Chunk();
        // fmt
        FMT_Chunk fmtChunk = FMT_Chunk();
        // data
        DATA_Chunk dataChunk = DATA_Chunk();

        uint32_t GetBufferSize() const;
        uint32_t GetBitRate() const;

        // EXTRA

        // acid
        ACID_Chunk acidChunk = ACID_Chunk();

        // bext
        BEXT_Chunk bextChunk = BEXT_Chunk();

        // other chunks
        /*
         * fact
         * junk
         */
        std::vector<Chunk, UAUDIO_DEFAULT_ALLOCATOR<Chunk>> otherChunks = std::vector<Chunk, UAUDIO_DEFAULT_ALLOCATOR<Chunk>>();

        std::string m_FilePath = "";

        // These variables are necessary because we have no way to know if the chunks are actually filled or not.
        bool
            filledRiffChunk = false,
            filledFmtChunk = false,
            filledDataChunk = false,
            filledAcidChunk = false,
            filledBextChunk = false;
    };
}