#pragma once

namespace uaudio
{
	constexpr auto CHUNK_ID_SIZE = 4;

	// BLOCK ALIGN SETTINGS
	constexpr uint16_t BLOCK_ALIGN_16_BIT_MONO = 2;
	constexpr uint16_t BLOCK_ALIGN_24_BIT_MONO = 3;
	constexpr uint16_t BLOCK_ALIGN_32_BIT_MONO = 4;
	constexpr uint16_t BLOCK_ALIGN_16_BIT_STEREO = BLOCK_ALIGN_16_BIT_MONO * 2;
	constexpr uint16_t BLOCK_ALIGN_24_BIT_STEREO = BLOCK_ALIGN_24_BIT_MONO * 2;
	constexpr uint16_t BLOCK_ALIGN_32_BIT_STEREO = BLOCK_ALIGN_32_BIT_MONO * 2;

	// BITS_PER_SAMPLE SETTINGS
	constexpr uint16_t WAVE_BITS_PER_SAMPLE_8 = 8;
	constexpr uint16_t WAVE_BITS_PER_SAMPLE_16 = 16;
	constexpr uint16_t WAVE_BITS_PER_SAMPLE_24 = 24;
	constexpr uint16_t WAVE_BITS_PER_SAMPLE_32 = 32;

	// SAMPLE RATE SETTINGS
	constexpr uint32_t WAVE_SAMPLE_RATE_44100 = 44100;
	constexpr uint32_t WAVE_SAMPLE_RATE_48000 = 48000;
	constexpr uint32_t WAVE_SAMPLE_RATE_88200 = 88200;
	constexpr uint32_t WAVE_SAMPLE_RATE_192000 = 192000;

	// MONO/STEREO.
	constexpr uint16_t WAVE_CHANNELS_MONO = 1;
	constexpr uint16_t WAVE_CHANNELS_STEREO = 2;

	// WAV FORMATS.
	constexpr uint16_t WAV_FORMAT_UNKNOWN = 1;
	constexpr uint16_t WAV_FORMAT_PCM = 1;
	constexpr uint16_t WAV_FORMAT_UNCOMPRESSED = 1;
	constexpr uint16_t WAV_FORMAT_MICROSOFT_ADPCM = 2;
	constexpr uint16_t WAV_FORMAT_ITU_G711_ALAW = 6;
	constexpr uint16_t WAV_FORMAT_ITU_G711_ÂΜLAW = 7;
	constexpr uint16_t WAV_FORMAT_IMA_ADPCM = 17;
	constexpr uint16_t WAV_FORMAT_ITU_G723_ADPCM = 20;
	constexpr uint16_t WAV_FORMAT_ITU_G723_ADPCM_YAMAHA = 20;
	constexpr uint16_t WAV_FORMAT_YAMAHA = 20;
	constexpr uint16_t WAV_FORMAT_GSM_610 = 49;
	constexpr uint16_t WAV_FORMAT_ITU_G721_ADPCM = 64;
	constexpr uint16_t WAV_FORMAT_MPEG = 80;

	// CHUNK DEFINITIONS.
	constexpr auto RIFF_CHUNK_ID = "RIFF";
	constexpr auto FMT_CHUNK_ID = "fmt ";
	constexpr auto FMT_CHUNK_FORMAT = "WAVE";
	constexpr auto DATA_CHUNK_ID = "data";
	constexpr auto ACID_CHUNK_ID = "acid";
	constexpr auto BEXT_CHUNK_ID = "bext";
	constexpr auto FACT_CHUNK_ID = "fact";
	constexpr auto CUE_CHUNK_ID = "cue ";
	constexpr auto SMPL_CHUNK_ID = "smpl";
	constexpr auto TLST_CHUNK_ID = "tlst";
}