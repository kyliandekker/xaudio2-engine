#pragma once
#include <intsafe.h>
#include <vector>

// http://soundfile.sapp.org/doc/WaveFormat/
// https://www.kvraudio.com/forum/viewtopic.php?t=172636
// https://tech.ebu.ch/docs/tech/tech3285.pdf
// http://www-mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html

#pragma pack(push,1)
struct Chunk
{
    unsigned char chunkId[4];
    uint32_t chunkSize;
};

struct RIFF_Chunk : Chunk
{
    unsigned char format[4];
};

/*
 * ** The riff chunk goes a little something like this: TOTAL SIZE: 4 + 8 (chunkid and chunksize)
 * ** but the chunksize is actually everything from the riff chunk to the end of the file.
 **
 ** 4 bytes                     Char array saying 'RIFF'.
 ** 4 bytes (char * 4)          Usually says WAVE.
 **
 **
 */

struct FMT_Chunk : Chunk
{
    uint16_t audioFormat;
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
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

struct DATA_Chunk : Chunk
{
    unsigned char* data;
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

struct ACID_Chunk : Chunk
{
    uint32_t type_of_file = 0x5;
    uint16_t root_note = 0x3c;
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

struct BEXT_Chunk : Chunk
{
    char description[256];
    char originator[32];
    char originator_reference[32];
    char origination_date[10];
    char origination_time[8];
    uint32_t time_reference_low;
    uint32_t time_reference_high;
    uint16_t version;
    unsigned char umid[64];
    uint16_t loudness_value;
    uint16_t loudness_range;
    uint16_t max_true_peak_level;
    uint16_t max_momentary_loudness;
    uint16_t max_short_term_loudness;
    unsigned char reserved[180];
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

struct WavFormat
{
    // riff
    RIFF_Chunk riffChunk = {};
    // fmt
    FMT_Chunk fmtChunk = {};
    // data
    DATA_Chunk dataChunk = {};

    uint32_t bufferSize = 0;

    // EXTRA

    // acid
    ACID_Chunk acidChunk = {};

    // bext
    BEXT_Chunk bextChunk = {};

    // other chunks
		/*
		 * fact
		 * junk
		*/
    std::vector<Chunk> otherChunks = std::vector<Chunk>();

    // These variables are necessary because we have no way to know if the chunks are actually filled or not.
    bool
        filledRiffChunk = false,
        filledFmtChunk = false,
        filledDataChunk = false,
        filledAcidChunk = false,
        filledBextChunk = false;
};