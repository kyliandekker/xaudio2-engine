#pragma once

// http://soundfile.sapp.org/doc/WaveFormat/
// https://www.kvraudio.com/forum/viewtopic.php?t=172636
// https://tech.ebu.ch/docs/tech/tech3285.pdf
// http://www-mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html
class WavFormat
{
public:
    // riff
    unsigned char chunkId[4]; // RIFF
    uint32_t chunkSize; // Riff chunk size.
    unsigned char format[4]; // WAVE
    // fmt
    unsigned char subchunk1Id[4];
    uint32_t subchunk1Size;
    uint16_t audioFormat;
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
    // data
    unsigned char subchunk2Id[4];
    uint32_t subchunk2Size;
    unsigned char* data;
    uint32_t bufferSize;

    // EXTRA

    // acid
    unsigned char subchunk3Id[4];
    uint32_t subchunk3Size;
    uint32_t type_of_file;
    uint16_t root_note;
    int num_of_beats;
    uint16_t meter_denominator;
    uint16_t meter_numerator;
    float tempo;

    // bext
    unsigned char subchunk4Id[4];
    uint32_t subchunk4Size;
    unsigned char description[256];
    unsigned char originator[32];
    unsigned char originator_reference[32];
    unsigned char origination_date[10];
    unsigned char origination_time[8];
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
 * ** The acid chunk goes a little something like this:
**
** 4 bytes          'acid'
** 4 bytes (int)     length of chunk starting at next byte
**
** 4 bytes (int)     type of file:
**        this appears to be a bit mask,however some combinations
**        are probably impossible and/or qualified as "errors"
**
**        0x01 On: One Shot         Off: Loop
**        0x02 On: Root note is Set Off: No root
**        0x04 On: Stretch is On,   Off: Strech is OFF
**        0x08 On: Disk Based       Off: Ram based
**        0x10 On: ??????????       Off: ????????? (Acidizer puts that ON)
**
** 2 bytes (short)      root note
**        if type 0x10 is OFF : [C,C#,(...),B] -> [0x30 to 0x3B]
**        if type 0x10 is ON  : [C,C#,(...),B] -> [0x3C to 0x47]
**         (both types fit on same MIDI pitch albeit different octaves, so who cares)
**
** 2 bytes (short)      ??? always set to 0x8000
** 4 bytes (float)      ??? seems to be always 0
** 4 bytes (int)        number of beats
** 2 bytes (short)      meter denominator   //always 4 in SF/ACID
** 2 bytes (short)      meter numerator     //always 4 in SF/ACID
**                      //are we sure about the order?? usually its num/denom
** 4 bytes (float)      tempo
**
 */