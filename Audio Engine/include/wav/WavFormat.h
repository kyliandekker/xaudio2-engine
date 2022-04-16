﻿#pragma once

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
	// https://www.recordingblogs.com/wiki/sample-chunk-of-a-wave-file
	// https://sites.google.com/site/musicgapi/technical-documents/wav-file-format

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

	/*
	** The RIFF chunk goes a little something like this: TOTAL SIZE: 8 (chunkid and chunksize) + 4
	** but the chunksize is actually everything from the riff chunk to the end of the file.
	** Wave file headers follow the standard RIFF file format structure. The first 8 bytes in the file is
	** a standard RIFF chunk header which has a chunk ID of "RIFF" and a chunk size equal to the file size
	** minus the 8 bytes used by the header. The first 4 data bytes in the "RIFF" chunk determines the type
	** of resource found in the RIFF chunk. Wave files always use "WAVE". After the RIFF type comes all of the
	** Wave file chunks that define the audio waveform.
	**
	** 4 bytes						Chunk ID.
	**									Char array saying 'RIFF' (0x52494646).

	** 4 bytes (long)				Chunk Size.
	**									Length of chunk starting at next byte.
	**
	** 4 bytes (char * 4)			Format.
	**									Type of resource. Usually says WAVE (0x57415645).
	**
	*/

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
	** The fmt chunk goes a little something like this: TOTAL SIZE: 16 + 8 (chunkid and chunksize)
	**
	** 4 bytes						Chunk ID.
	**									Char array saying 'fmt ' (0x666D7420).

	** 4 bytes (long)				Chunk Size.
	**									Length of chunk starting at next byte.
	**
	** 2 bytes (short)				Audio Format/Compression Code (1 - 65,535).
	**									Format of the audio/Compression code (1 - 65,535).
	**										1 means PCM (WAVE_FORMAT_PCM)
	**										Anything else means it is compressed a bit.
	**											0 (0x0000)	Unknown
	**											1 (0x0001)	PCM/uncompressed
	**											2 (0x0002)	Microsoft ADPCM
	**											6 (0x0006)	ITU G.711 a-law
	**											7 (0x0007)	ITU G.711 Âµ-law
	**											17 (0x0011)	IMA ADPCM
	**											20 (0x0016)	ITU G.723 ADPCM (Yamaha)
	**											49 (0x0031)	GSM 6.10
	**											64 (0x0040)	ITU G.721 ADPCM
	**											80 (0x0050)	MPEG
	**
	** 2 bytes (short)				Number of Channels/Amount of Channels/Channel Count/Mono/Stereo (1 - 65,535).
	**									The number of channels specifies how many separate audio signals
	**									are encoded in the wave data chunk (1 - 65,535).
	**										1 means mono (no stereo info).
	**										2 means stereo.
	**
	** 4 bytes (long)				The sample rate/sampling frequency/sampling rate/samples per seconds/hertz (1 - 0xFFFFFFFF).
	**									The number of sample slices per second. This value is unaffected by the number of channels.
	**										This is in Hz.
	**
	** 4 bytes (long)				Average Bytes Per Second/Byte rate/bps (1 - 0xFFFFFFFF).
	**									This value indicates how many bytes of wave data must be streamed to a
	**									D/A converter per second in order to play the wave file. This information is
	**									useful when determining if data can be streamed from the source fast enough
	**									to keep up with playback. This value can be easily calculated with the formula:
	**										ByteRate = SampleRate * BlockAlign.
	**
	** 2 bytes (short)				BlockAlign (1 - 65,535).
	**									The number of bytes for one sample including all channels (1 - 65,535).
	**									The number of bytes per sample slice. This value is affected by the number of channels
	**									and can be calculated with the formula:
	**										BlockAlign = SignificantBitsPerSample / 8 * NumChannels.
	**
	** 2 bytes (short)				Bits per Sample/Significant Bits Per Sample (2 - 65,535).
	**									This value specifies the number of bits used to define each sample. This value is usually
	**									If the number of bits is not byte aligned (a multiple of 8) then the number of bytes used
	**									per sample is rounded up to the nearest byte size and the unused bytes are set to 0 and ignored.
	**										(8-bit, 16-bit, 24-bit, 32-bit)
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
	** The data chunk goes a little something like this: 8 (chunkid and chunksize) + ?
	** The Wave Data Chunk contains the digital audio sample data which can be decoded using the
	** format and compression method specified in the Wave Format Chunk. If the Compression Code
	** is 1 (uncompressed PCM), then the Wave Data contains raw sample values.
	** One point about sample data that may cause some confusion is that when samples are represented
	** with 8-bits, they are specified as unsigned values. All other sample bit-sizes are specified as
	** signed values. For example a 16-bit sample can range from -32,768 to +32,767 with a mid-point (silence)
	** at 0.
	**
	** 4 bytes						Chunk ID.
	**									Char array saying 'data' (0x64617461).

	** 4 bytes (long)				Chunk Size.
	**									Length of chunk starting at next byte.
	**
	** ? bytes (?)					Data:
	**									can be 24-bit integers, 16-bit integers, 8-bit integers or floats.
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
	** The acid chunk goes a little something like this: TOTAL SIZE: 24 + 8 (chunkid and chunksize)
	**
	** 4 bytes						Chunk ID.
	**									Char array saying 'acid'.

	** 4 bytes (long)				Chunk Size.
	**									Length of chunk starting at next byte.
	**
	** 4 bytes (long)				Type of file (0 - 0xFFFFFFFF).
	**									this appears to be a bit mask, however some combinations
	**									are probably impossible and/or qualified as "errors".
	**
	**									0x01 On: One Shot         Off: Loop
	**									0x02 On: Root note is Set Off: No root
	**									0x04 On: Stretch is On,   Off: Strech is OFF
	**									0x08 On: Disk Based       Off: Ram based
	**									0x10 On: ??????????       Off: ????????? (Acidizer puts that ON)
	**
	** 2 bytes (short)				Root Note/Root (1 - 65,535).
	**									if type 0x10 is OFF : [C,C#,(...),B] -> [0x30 to 0x3B]
	**									if type 0x10 is ON  : [C,C#,(...),B] -> [0x3C to 0x47]
	**									(both types fit on same MIDI pitch albeit different octaves, so who cares)
	**
	** 2 bytes (short)				??? always set to 0x8000 (1 - 65,535).
	**
	** 4 bytes (float)				??? seems to be always 0 (0 - 0xFFFFFFFF).
	**
	** 4 bytes (long)				Number of Beats/Amount of Beats (0 - 0xFFFFFFFF).
	**									Never seems to be correct but it says the number of beats in the track.
	**
	** 2 bytes (short)				Meter Denominator (1 - 65,535).   //always 4 in SF/ACID
	**
	** 2 bytes (short)				Meter Numerator (1 - 65,535).     //always 4 in SF/ACID
	**									//are we sure about the order?? usually its num/denom
	**
	** 4 bytes (float)				The BPM/Beats Per Minute/Tempo (0 - 0xFFFFFFFF).
	**									This stores the information about how many beats play per minute.
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

		uint32_t type_of_file = 0x5;
		uint16_t root_note = 0x3c;
		uint16_t unknown1 = 0x8000;
		float unknown2 = 0;
		uint32_t num_of_beats = 0;
		uint16_t meter_denominator = 0;
		uint16_t meter_numerator = 0;
		float tempo = 0.0f;
	};

	/*
	** The bext chunk goes a little something like this: TOTAL SIZE: 602/604 + 8 (chunkid and chunksize)
	**
	** 4 bytes						Chunk ID.
	**									Char array saying 'bext'.

	** 4 bytes (long)				Chunk Size.
	**									Length of chunk starting at next byte.
	**
	** 256 bytes (char * 256)		Description.
	**									Description of the file.
	**
	** 32 bytes (char * 32)			Originator/Producer.
	**									Contains the name of the originator/ producer of the audio file.
	**
	** 32 bytes (char * 32)			Originator Reference.
	**									Unambiguous reference allocated by the originating organization.
	**
	** 10 bytes (char * 10)			Origination date/date.
	**									The date of creation of the audio sequence.
	**										The format shall be « ‘,year’,-,’month,’-‘,day,’»
	**										with 4 characters for the year and 2 characters per other item.
	**											Year is defined from 0000 to 9999
	**											Month is defined from 1 to 12
	**											Day is defined from 1 to 28, 29, 30 or 31
	**												The separator between the items can be anything but it is recommended
	**												that one of the following characters be used:
	**												‘-’ hyphen ‘_’ underscore ‘:’ colon ‘ ’ space ‘.’ stop
	**
	** 8 bytes (char * 8)			Origination time/time.
	**									Contains the time of creation of the audio sequence.
	**										The format shall be « ‘hour’-‘minute’-‘second’» with 2 characters per item.
	**											Hour is defined from 0 to 23.
	**											Minute and second are defined from 0 to 59.
	**												The separator between the items can be anything but it is recommended
	**												that one of the following characters be used:
	**												‘-’ hyphen ‘_’ underscore ‘:’ colon ‘ ’ space ‘.’ stop
	**
	** 4 bytes (long)				Time Reference Low (0 - 0xFFFFFFFF).
	**									Contains the time-code of the sequence. It is a 64-bit
	**									value which contains the first sample count since midnight. The number
	**									of samples per second depends on the sample frequency which is defined
	**										in the field <nSamplesPerSec> from the <format chunk>.
	**
	** 4 bytes (long)				Time Reference High (0 - 0xFFFFFFFF).
	**									Contains the time-code of the sequence. It is a 64-bit
	**									value which contains the first sample count since midnight. The number
	**									of samples per second depends on the sample frequency which is defined
	**									in the field <nSamplesPerSec> from the <format chunk>.
	**
	** 2 bytes (short)				Version (1 - 65,535).
	**									An unsigned binary number giving the version of the BWF. This number is
	**									particularly relevant for the carriage of the UMID and loudness
	**									information. For Version 1 it shall be set to 0001h and for Version 2 it
	**									shall be set to 0002h.
	**
	** 64 bytes (uchar * 64)		UMID/Unique Material Identifier.
	**									Contains a UMID (Unique Material Identifier) to standard
	**									SMPTE 330M [1]. If only a 32 byte "basic UMID" is used, the last 32 bytes
	**									should be set to zero. (The length of the UMID is given internally.)
	**
	** 2 bytes (short)				Loudness Value/Loudness (1 - 65,535).
	**									A 16-bit signed integer, equal to round(100x the Integrated Loudness
	**									Value of the file in LUFS).
	**
	** 2 bytes (short)				Loudness Range/Range (1 - 65,535).
	**									A 16-bit signed integer, equal to round(100x the Loudness Range of the
	**									file in LU).
	**
	** 2 bytes (short)				Max True Peak Level (1 - 65,535).
	**									A 16-bit signed integer, equal to round(100x the Maximum True Peak
	**									Value of the file in dBTP).
	**
	** 2 bytes (short)				Max Momentary Loudness (1 - 65,535).
	**									A 16-bit signed integer, equal to round(100x the highest value of the
	**									Momentary Loudness Level of the file in LUFS).
	**
	** 2 bytes (short)				Max Short Term Loudness (1 - 65,535).
	**									A 16-bit signed integer, equal to round(100x the highest value of the
	**									Short-term Loudness Level of the file in LUFS).
	**
	** 180 bytes (char * 180)		Reserved.
	** 									180 bytes reserved for extension. If the Version field is set to 0001h or
	**									0002h, these 180 bytes shall be set to a NULL (zero) value.
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
	** The fact chunk goes a little something like this: TOTAL SIZE: 602/604 + 8 (chunkid and chunksize)
	**
	** 4 bytes						Chunk ID.
	**									Char array saying 'fact' (0x66616374).

	** 4 bytes (long)				Chunk Size.
	**									Length of chunk starting at next byte.
	**
	** 4 bytes (long)				Number of samples/Sample count/Sample length (0 - 0xFFFFFFFF).
	**									specifies the number of samples in the waveform data chunk.
	**									This value can be used with the Samples Per Second value specified in
	**									the format chunk to calculate the waveforms length in seconds.
	**
	*/

	class FACT_Chunk : public Chunk
	{
	public:
		FACT_Chunk();
		FACT_Chunk(unsigned char a_ChunkID[CHUNK_ID_SIZE], uint32_t a_ChunkSize, uint32_t a_SampleLength);
		FACT_Chunk(const FACT_Chunk &rhs);

		FACT_Chunk &operator=(const FACT_Chunk &rhs);

		uint32_t sample_length = 0;
	};

	/*
	**
	**
	** 4 bytes (long)				Cue Point ID (0 - 0xFFFFFFFF).
	**									The Cue Point ID specifies the unique ID that corresponds to one of the defined cue points
	**									in the cue point list. Furthermore, this ID corresponds to any labels defined in the associated
	**									data list chunk which allows text labels to be assigned to the various sample loops.
	**
	** 4 bytes (long)				Type (0 - 0xFFFFFFFF).
	**									The type field defines how the waveform samples will be looped.
	**										0	Loop forward (normal)
	**										1	Alternating loop (forward/backward, also known as Ping Pong)
	**										2	Loop backward (reverse)
	**										3 - 31	Reserved for future standard types
	**										32 - 0xFFFFFFFF	Sampler specific types (defined by manufacturer)
	**
	** 4 bytes (long)				Start (0 - 0xFFFFFFFF).
	**									The start value specifies the byte offset into the waveform data of the first sample to be played in the loop.
	**
	** 4 bytes (long)				End (0 - 0xFFFFFFFF).
	**									The end value specifies the byte offset into the waveform data of the last sample to be played in the loop.
	**
	** 4 bytes (long)				Fraction (0 - 0xFFFFFFFF).
	**									The fractional value specifies a fraction of a sample at which to loop. This allows a loop to be fine tuned
	**									at a resolution greater than one sample. The value can range from 0x00000000 to 0xFFFFFFFF. A value of 0 means
	**									no fraction, a value of 0x80000000 means 1/2 of a sample length. 0xFFFFFFFF is the smallest fraction of a sample
	**									that can be represented.
	**
	** 4 bytes (long)				Play Count (0 - 0xFFFFFFFF).
	**									The play count value determines the number of times to play the loop. A value of 0 specifies an infinite sustain
	**									loop. An infinite sustain loop will continue looping until some external force interrupts playback, such as the
	**									musician releasing the key that triggered the wave's playback. All other values specify an absolute number of times to loop.
	**
	*/

	class SMPL_Sample_Loop
	{
	public:
		SMPL_Sample_Loop();
		SMPL_Sample_Loop(uint32_t a_CuePointId, uint32_t a_Type, uint32_t a_Start, uint32_t a_End, uint32_t a_Fraction, uint32_t a_PlayCount);
		SMPL_Sample_Loop(const SMPL_Sample_Loop &rhs);

		SMPL_Sample_Loop &operator=(const SMPL_Sample_Loop &rhs);

		uint32_t cue_point_id = 0;
		uint32_t type = 0;
		uint32_t start = 0;
		uint32_t end = 0;
		uint32_t fraction = 0;
		uint32_t play_count = 0;
	};

	/*
	** The smpl chunk goes a little something like this: TOTAL SIZE: 602/604 + 8 (chunkid and chunksize)
	**
	** 4 bytes						Chunk ID.
	**									Char array saying 'smpl' (0x736D706C).

	** 4 bytes (long)				Chunk Size (36 + (Num Sample Loops * 24) + Sampler Data).
	**									Length of chunk starting at next byte.
	**
	** 4 bytes (long)				Manufacturer/MMA Code/MIDI Manufacturer's Association Code (0 - 0xFFFFFFFF).
	**									The manufacturer field specifies the MIDI Manufacturer's Association (MMA)
	**									Manufacturer code for the sampler intended to receive this file's waveform.
	**									Each manufacturer of a MIDI product is assigned a unique ID which identifies
	**									the company. If no particular manufacturer is to be specified, a value of 0 should be used.
	**									The value is stored with some extra information to enable translation to the value used in
	**									a MIDI System Exclusive transmission to the sampler. The high byte indicates the number of
	**									low order bytes (1 or 3) that are valid for the manufacturer code. For example, the value
	**									for Digidesign will be 0x01000013 (0x13) and the value for Microsoft will be 0x30000041
	**									(0x00, 0x00, 0x41).
	**										01 Sequential Circuits / Dave Smith Instruments
	**										02 IDP
	**										03 Octave-Plateau / Voyetra
	**										04 Moog Music
	**										05 Passport Designs
	**										06 Lexicon
	**										07 Kurzweil
	**										0F Ensoniq
	**										10 Oberheim
	**										11 Apple Computer
	**										13 Digidesign
	**										15 JL Cooper
	**										18 E-mu
	**										1C Eventide
	**										00 00 09 New England Digital
	**										00 00 16 Opcode
	**										00 00 1B Peavey
	**										00 00 1C 360 Systems
	**										00 00 1F Zeta
	**										00 00 2F Encore Electronics
	**										00 00 3B MOTU
	**										00 00 41 Microsoft
	**										00 00 4D Studio Electronics
	**										00 01 05 M-Audio
	**										00 01 21 Cakewalk
	**										00 01 37 Roger Linn Design
	**										00 01 3F Numark / Alesis
	**										00 01 4D Open Labs
	**										00 01 72 Kilpatrick Audio
	**										00 01 77 Nektar
	**										00 02 14 Intellijel
	**										00 02 1F Madrona Labs
	**										00 02 26 Electro-Harmonix
	**										22 Synthaxe
	**										27 Jellinghaus
	**										29 PPG
	**										2D Hinton Instruments
	**										2F Elka
	**										31 Viscount
	**										33 Clavia
	**										38 Simmons
	**										3A Steinberg
	**										3E Waldorf
	**										3F Quasimidi
	**										00 20 13 Kenton
	**										00 20 1A Fatar / Studiologic
	**										00 20 1F TC Electronic
	**										00 20 29 Novation
	**										00 20 32 Behringer
	**										00 20 33 Access Music
	**										00 20 3A Propellorhead
	**										00 20 3B Red Sound
	**										00 20 4D Vermona
	**										00 20 50 Hartmann
	**										00 20 52 Analogue Systems
	**										00 20 5F Sequentix
	**										00 20 69 Elby Designs
	**										00 20 6B Arturia
	**										00 20 76 Teenage Engineering
	**										00 21 02 Mutable Instruments
	**										00 21 07 Modal Electronics
	**										00 21 09 Native Instruments
	**										00 21 10 ROLI
	**										00 21 1A IK Multimedia
	**										00 21 27 Expert Sleepers
	**										00 21 35 Dreadbox
	**										00 21 41 Marienberg
	**										40 Kawai
	**										41 Roland
	**										42 Korg
	**										43 Yamaha
	**										44 Casio
	**										47 Akai
	**										4C Sony
	**
	** 4 bytes (long)				Product/MMID/Midi Model ID (0 - 0xFFFFFFFF).
	**									The product field specifies the MIDI model ID defined by the manufacturer corresponding
	**									to the Manufacturer field. Contact the manufacturer of the sampler to get the model ID.
	**									If no particular manufacturer's product is to be specified, a value of 0 should be used.
	**
	** 4 bytes (long)				Sample Period (0 - 0xFFFFFFFF).
	**									The sample period specifies the duration of time that passes during the playback of one sample in
	**									nanoseconds (normally equal to 1 / Samplers Per Second, where Samples Per Second is the value
	**									found in the format chunk).
	**
	** 4 bytes (long)				MIDI Unity Note (0 - 127).
	**									The MIDI unity note value has the same meaning as the instrument chunk's MIDI Unshifted Note field
	**									which specifies the musical note at which the sample will be played at it's original sample rate
	**									(the sample rate specified in the format chunk).
	**
	** 4 bytes (long)				MIDI Pitch Fraction (0 - 0xFFFFFFFF).
	**									The MIDI pitch fraction specifies the fraction of a semitone up from the specified MIDI unity note
	**									field. A value of 0x80000000 means 1/2 semitone (50 cents) and a value of 0x00000000 means no fine
	**									tuning between semitones.
	**
	** 4 bytes (long)				SMPTE Format/Society of Motion Pictures and Television E time format/Society of Motion Pictures and Television E format (0 - 0xFFFFFFFF).
	**									The SMPTE format specifies the Society of Motion Pictures and Television E time format used in the following SMPTE Offset field.
	**									If a value of 0 is set, SMPTE Offset should also be set to 0.
	**										0	no SMPTE offset
	**										24	24 frames per second
	**										25	25 frames per second
	**										29	30 frames per second with frame dropping (30 drop)
	**										30	30 frames per second
	**
	** 4 bytes (long)				SMPTE Offset/Society of Motion Pictures and Television E offset (0 - 0xFFFFFFFF).
	**									The SMPTE Offset value specifies the time offset to be used for the synchronization / calibration to the first sample in the waveform.
	**									This value uses a format of 0xhhmmssff where hh is a signed value that specifies the number of hours (-23 to 23), mm is an unsigned
	**									value that specifies the number of minutes (0 to 59), ss is an unsigned value that specifies the number of seconds (0 to 59) and ff
	**									is an unsigned value that specifies the number of frames (0 to -1).
	**
	** 4 bytes (long)				Sample Loops/Number of Samples (0 - 0xFFFFFFFF).
	**									The sample loops field specifies the number Sample Loop definitions in the following list. This value may be set to 0 meaning that no sample loops follow.
	**
	** 4 bytes (long)				Sampler Data (0 - 0xFFFFFFFF).
	**									The sampler data value specifies the number of bytes that will follow this chunk (including the entire sample loop list). This value is greater than 0 when
	**									an application needs to save additional information. This value is reflected in this chunks data size value.

	** 4 bytes (long)				List of Sample Loops/Sample loops.
	**									A list of sample loops is simply a set of consecutive loop descriptions that follow the format described below. The sample loops do not have to be in any particular
	**									order because each sample loop associated cue point position is used to determine the play order. The sampler chunk is optional.
	**
	*/

	class SMPL_Chunk : public Chunk
	{
	public:
		SMPL_Chunk();
		SMPL_Chunk(unsigned char a_ChunkID[CHUNK_ID_SIZE], uint32_t a_ChunkSize, uint32_t a_Manufacturer, uint32_t a_Product, uint32_t a_SamplePeriod, uint32_t a_MidiUnityNode, uint32_t a_MidiPitchFraction, uint32_t a_Smpte_Format, uint32_t a_Smpte_Offset, uint32_t a_NumSampleLoops, uint32_t a_SamplerData);
		SMPL_Chunk(const SMPL_Chunk &rhs);

		SMPL_Chunk &operator=(const SMPL_Chunk &rhs);

		uint32_t manufacturer = 0;
		uint32_t product = 0;
		uint32_t sample_period = 0;
		uint32_t midi_unity_node = 0;
		uint32_t midi_pitch_fraction = 0;
		uint32_t smpte_format = 0;
		uint32_t smpte_offset = 0;
		uint32_t num_sample_loops = 0;
		uint32_t sampler_data = 0;
	};

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

		// fact
		FACT_Chunk factChunk = FACT_Chunk();

		// smpl
		SMPL_Chunk smplChunk = SMPL_Chunk();

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
			filledBextChunk = false,
			filledFactChunk = false,
			filledSmplChunk = false;
	};
}