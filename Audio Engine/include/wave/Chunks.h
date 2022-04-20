#pragma once

#include <cstdint>

#include "WaveFormat.h"

namespace uaudio
{
	// BLOCK ALIGN SETTINGS
	constexpr auto BLOCK_ALIGN_16_BIT_MONO = 2;
	constexpr auto BLOCK_ALIGN_24_BIT_MONO = 3;
	constexpr auto BLOCK_ALIGN_32_BIT_MONO = 4;
	constexpr auto BLOCK_ALIGN_16_BIT_STEREO = BLOCK_ALIGN_16_BIT_MONO * 2;
	constexpr auto BLOCK_ALIGN_24_BIT_STEREO = BLOCK_ALIGN_24_BIT_MONO * 2;
	constexpr auto BLOCK_ALIGN_32_BIT_STEREO = BLOCK_ALIGN_32_BIT_MONO * 2;

	// BPS SETTINGS
	constexpr auto WAVE_BPS_8 = 8;
	constexpr auto WAVE_BPS_16 = 16;
	constexpr auto WAVE_BPS_24 = 24;
	constexpr auto WAVE_BPS_32 = 32;

	// SAMPLE RATE SETTINGS
	constexpr auto WAVE_SAMPLE_RATE_44100 = 44100;
	constexpr auto WAVE_SAMPLE_RATE_48000 = 48000;
	constexpr auto WAVE_SAMPLE_RATE_88200 = 88200;
	constexpr auto WAVE_SAMPLE_RATE_192000 = 192000;

	// MONO/STEREO.
	constexpr auto WAVE_CHANNELS_MONO = 1;
	constexpr auto WAVE_CHANNELS_STEREO = 2;

	// WAV FORMATS.
	constexpr auto WAV_FORMAT_UNKNOWN = 1;
	constexpr auto WAV_FORMAT_PCM = 1;
	constexpr auto WAV_FORMAT_UNCOMPRESSED = 1;
	constexpr auto WAV_FORMAT_MICROSOFT_ADPCM = 2;
	constexpr auto WAV_FORMAT_ITU_G711_ALAW = 6;
	constexpr auto WAV_FORMAT_ITU_G711_ÂΜLAW = 7;
	constexpr auto WAV_FORMAT_IMA_ADPCM = 17;
	constexpr auto WAV_FORMAT_ITU_G723_ADPCM = 20;
	constexpr auto WAV_FORMAT_ITU_G723_ADPCM_YAMAHA = 20;
	constexpr auto WAV_FORMAT_YAMAHA = 20;
	constexpr auto WAV_FORMAT_GSM_610 = 49;
	constexpr auto WAV_FORMAT_ITU_G721_ADPCM = 64;
	constexpr auto WAV_FORMAT_MPEG = 80;;

	constexpr auto RIFF_CHUNK_ID = "RIFF";
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

#pragma pack(push, 1)
	constexpr auto FMT_CHUNK_ID = "fmt ";
	constexpr auto FMT_CHUNK_FORMAT = "WAVE";
	struct FMT_Chunk
	{
		FMT_Chunk(FMT_Chunk* a_Data)
		{
			if (a_Data != nullptr)
			{
				audioFormat = a_Data->audioFormat;
				numChannels = a_Data->numChannels;
				sampleRate = a_Data->sampleRate;
				byteRate = a_Data->byteRate;
				blockAlign = a_Data->blockAlign;
				bitsPerSample = a_Data->bitsPerSample;
			}
		}
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

	constexpr auto DATA_CHUNK_ID = "data";
	struct DATA_Chunk
	{
		DATA_Chunk(DATA_Chunk* a_Data)
		{
			if (a_Data != nullptr)
			{
				data = reinterpret_cast<unsigned char*>(a_Data);
			}
		}
		unsigned char* data = nullptr;
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

	constexpr auto ACID_CHUNK_ID = "acid";
	struct ACID_Chunk
	{
		ACID_Chunk(ACID_Chunk* a_Data)
		{
			if (a_Data != nullptr)
			{
				type_of_file = a_Data->type_of_file;
				root_note = a_Data->root_note;
				unknown1 = a_Data->unknown1;
				unknown2 = a_Data->unknown2;
				num_of_beats = a_Data->num_of_beats;
				meter_denominator = a_Data->meter_denominator;
				meter_numerator = a_Data->meter_numerator;
				tempo = a_Data->tempo;
			}
		}
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

	constexpr auto BEXT_CHUNK_ID = "bext";
	struct BEXT_Chunk
	{
		BEXT_Chunk(BEXT_Chunk* a_Data)
		{
			if (a_Data != nullptr)
			{
				memcpy(description, a_Data->description, sizeof(description));
				memcpy(originator, a_Data->originator, sizeof(originator));
				memcpy(originator_reference, a_Data->originator_reference, sizeof(originator_reference));
				memcpy(origination_date, a_Data->origination_date, sizeof(origination_date));
				memcpy(origination_time, a_Data->origination_time, sizeof(origination_time));
				time_reference_low = a_Data->time_reference_low;
				time_reference_high = a_Data->time_reference_high;
				version = a_Data->version;
				memcpy(umid, a_Data->umid, sizeof(umid));
				loudness_value = a_Data->loudness_value;
				loudness_range = a_Data->loudness_range;
				max_true_peak_level = a_Data->max_true_peak_level;
				max_momentary_loudness = a_Data->max_momentary_loudness;
				max_short_term_loudness = a_Data->max_short_term_loudness;
				memcpy(reserved, a_Data->reserved, sizeof(reserved));
			}
		}
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
	** The fact chunk goes a little something like this: TOTAL SIZE: 4 + 8 (chunkid and chunksize)
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

	constexpr auto FACT_CHUNK_ID = "fact";
	struct FACT_Chunk
	{
		FACT_Chunk(FACT_Chunk* a_Data)
		{
			if (a_Data != nullptr)
			{
				sample_length = a_Data->sample_length;
			}
		}
		uint32_t sample_length = 0;
	};
#pragma pack(pop)
}
