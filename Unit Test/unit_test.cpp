#include <wave/WaveConverter.h>
#include <array>
#include <vector>

#include "doctest.h"
#include "utils/Logger.h"
#include "wave/WaveFile.h"
#include "wave/WaveReader.h"

void random_mono_to_stereo(uint16_t block_align)
{
	uaudio::logger::print_cyan();
	printf("[%i bit (random)]\n", block_align * 8);
	uaudio::logger::print_white();

	srand((unsigned)time(0));

	std::vector<unsigned char> dat;
	uint32_t size = (rand() % 10) + 1;
	size *= block_align;

	for (uint32_t i = 0; i < size; i++)
		dat.push_back(rand() % 100);

	unsigned char *data = dat.data();

	std::vector<unsigned char> EXPECTED(static_cast<uint64_t>(size) * 2);

	int newIndex = 0;
	for (uint16_t i = 0; i <= static_cast<uint16_t>(size) - block_align; i += block_align)
	{
		int echo = 0;
		while (echo < 2)
		{
			for (uint16_t j = 0; j < block_align; j++)
			{
				EXPECTED[newIndex] = data[i + j];
				newIndex++;
			}
			echo++;
		}
	}

	CHECK(size == dat.size());
	data = uaudio::conversion::ConvertMonoToStereo(data, size, block_align);
	CHECK(size == EXPECTED.size());

	for (uint32_t i = 0; i < size; i++)
		CHECK(data[i] == EXPECTED[i]);

	uaudio::logger::print_green();
	printf("SUCCESS!\n");
	uaudio::logger::print_white();
}

void random_stereo_to_mono(uint16_t block_align)
{
	uaudio::logger::print_cyan();
	printf("[%i bit (random)]\n", block_align / 2 * 8);
	uaudio::logger::print_white();

	srand((unsigned)time(0));

	std::vector<unsigned char> dat;
	uint32_t size = (rand() % 10) + 1;
	size *= block_align;

	for (uint32_t i = 0; i < size; i++)
		dat.push_back(rand() % 100);

	unsigned char *data = dat.data();

	std::vector<unsigned char> EXPECTED(size / 2);

	int newIndex = 0;
	for (uint16_t i = 0; i <= size - block_align; i += block_align)
	{
		for (uint16_t j = 0; j < block_align / 2; j++)
		{
			EXPECTED[newIndex] = data[i + j];
			newIndex++;
		}
	}

	CHECK(size == dat.size());
	data = uaudio::conversion::ConvertStereoToMono(data, size, block_align);
	CHECK(size == EXPECTED.size());

	for (uint32_t i = 0; i < size; i++)
		CHECK(data[i] == EXPECTED[i]);

	uaudio::logger::print_green();
	printf("SUCCESS!\n");
	uaudio::logger::print_white();
}

TEST_CASE("Testing Hash Function")
{
	const std::string _stringLit = "Rs_239Ksa*--A";
	const char *_charptr = "Rs_239Ksa*--a";

	SUBCASE("Lowercase and uppercase")
	{
		uaudio::logger::print_cyan();
		printf("[LOWERCASE AND UPPERCASE IN HASH FUNCTION]\n");
		uaudio::logger::print_white();

		UAUDIO_DEFAULT_HASH stdStringA = UAUDIO_DEFAULT_HASH_FUNCTION(_stringLit);
		UAUDIO_DEFAULT_HASH stdStringB = UAUDIO_DEFAULT_HASH_FUNCTION(_stringLit.c_str());
		UAUDIO_DEFAULT_HASH charPtrA = UAUDIO_DEFAULT_HASH_FUNCTION(_charptr);

		CHECK(stdStringA == stdStringB);
		CHECK(stdStringB != charPtrA);

		uaudio::logger::print_green();
		printf("SUCCESS!\n");
		uaudio::logger::print_white();
	}
	SUBCASE("Collision test")
	{
		uaudio::logger::print_cyan();
		printf("[COLLISION TEST IN HASH FUNCTION]\n");
		uaudio::logger::print_white();

		std::array<const char *, 51> tests = {"Cotn_Containers.png", "Cotn_Background.png", "Cotn_Bat.png",
											  "Cotn_Cadence.png", "Cotn_Font.png", "Cotn_Ghost.png", "Cotn_MB_Bat.png", "Cotn_MB_Dragon.png", "Cotn_MB_Minotaur.png",
											  "Cotn_Monkey.png", "Cotn_Shopkeeper.png", "Cotn_Skeleton.png", "Cotn_Slime.png", "Cotn_Traps.png", "Cotn_Walls.png",
											  "Cotn_Zombie.png", "Custom_EntitySpawn.png", "ReworkShort_Background.png", "test.json", "Zawarudo.wav",
											  "Deltarune - Field of Hopes and Dreams.wav_new.wav", "Headhunterz - Home (Extended).wav", "Undefiant - Best I Ever Could (Extended Mix).wav",
											  "Barrel.json", "Crate.json", "Player.json", "miniboss1.json", "miniboss2.json", "miniboss3.json",
											  "miniboss4.json", "normal1.json", "normal2.json", "normal3.json", "normal4.json", "normal5.json", "normal6.json",
											  "secret1.json", "secret2.json", "secret3.json", "secret4.json", "shop1.json", "start0.json", "hello", "h", "he", "hel", "metronome_01.wav",
											  "metronome_02.wav", "default_sound.wav", "default_soundtrack.wav", "fontawesome.ttf"};

		std::vector<UAUDIO_DEFAULT_HASH> hashes;

		hashes.reserve(tests.size());

		for (size_t i = 0; i < tests.size(); i++)
			hashes.push_back(UAUDIO_DEFAULT_HASH_FUNCTION(tests[i]));

		for (size_t i = 0; i < tests.size(); i++)
			for (size_t j = 0; j < tests.size(); j++)
				if (i != j)
				{
					printf("Tested %s against %s\n", tests[i], tests[j]);
					CHECK(hashes[i] != hashes[j]);
				}

		uaudio::logger::print_green();
		printf("SUCCESS!\n");
		uaudio::logger::print_white();
	}
}

TEST_CASE("Audio Conversion")
{
	SUBCASE("Mono to Stereo (pre-defined)")
	{
		uaudio::logger::print_cyan();
		printf("[MONO TO STEREO (pre-defined)]\n");
		uaudio::logger::print_white();
		SUBCASE("16-bit")
		{
			uaudio::logger::print_cyan();
			printf("[16 bit (pre-defined)]\n");
			uaudio::logger::print_white();

			const uint16_t block_align = uaudio::BLOCK_ALIGN_16_BIT_MONO;

			std::array<unsigned char, block_align> dat = {
				2,
				0,
			};
			unsigned char *data = dat.data();

			uint32_t size = static_cast<uint32_t>(dat.size());

			std::array<unsigned char, block_align * 2> EXPECTED = {
				2,
				0,
				2,
				0,
			};

			CHECK(size == dat.size());
			data = uaudio::conversion::ConvertMonoToStereo(data, size, block_align);
			CHECK(size == EXPECTED.size());

			for (uint32_t i = 0; i < size; i++)
				CHECK(data[i] == EXPECTED[i]);

			uaudio::logger::print_green();
			printf("SUCCESS!\n");
			uaudio::logger::print_white();
		}
		SUBCASE("24-bit")
		{
			uaudio::logger::print_cyan();
			printf("[24 bit (pre-defined)]\n");
			uaudio::logger::print_white();

			const uint16_t block_align = uaudio::BLOCK_ALIGN_24_BIT_MONO;

			std::array<unsigned char, block_align> dat = {
				2,
				0,
				5,
			};
			unsigned char *data = dat.data();

			uint32_t size = static_cast<uint32_t>(dat.size());

			std::array<unsigned char, block_align * 2> EXPECTED = {
				2,
				0,
				5,
				2,
				0,
				5,
			};

			CHECK(size == dat.size());
			data = uaudio::conversion::ConvertMonoToStereo(data, size, block_align);
			CHECK(size == EXPECTED.size());

			for (uint32_t i = 0; i < size; i++)
				CHECK(data[i] == EXPECTED[i]);

			uaudio::logger::print_green();
			printf("SUCCESS!\n");
			uaudio::logger::print_white();
		}
		SUBCASE("32-bit")
		{
			uaudio::logger::print_cyan();
			printf("[32 bit (pre-defined)]\n");
			uaudio::logger::print_white();

			const uint16_t block_align = uaudio::BLOCK_ALIGN_32_BIT_MONO;

			std::array<unsigned char, block_align> dat = {
				2,
				0,
				5,
				1,
			};
			unsigned char *data = dat.data();

			uint32_t size = static_cast<uint32_t>(dat.size());

			std::array<unsigned char, block_align * 2> EXPECTED = {
				2,
				0,
				5,
				1,
				2,
				0,
				5,
				1,
			};

			CHECK(size == dat.size());
			data = uaudio::conversion::ConvertMonoToStereo(data, size, block_align);
			CHECK(size == EXPECTED.size());

			for (uint32_t i = 0; i < size; i++)
				CHECK(data[i] == EXPECTED[i]);

			uaudio::logger::print_green();
			printf("SUCCESS!\n");
			uaudio::logger::print_white();
		}
	}
	SUBCASE("Mono to Stereo (random)")
	{
		uaudio::logger::print_cyan();
		printf("[MONO TO STEREO (random)]\n");
		uaudio::logger::print_white();

		SUBCASE("16-bit")
		{
			random_mono_to_stereo(uaudio::BLOCK_ALIGN_16_BIT_MONO);
		}
		SUBCASE("24-bit")
		{
			random_mono_to_stereo(uaudio::BLOCK_ALIGN_24_BIT_MONO);
		}
		SUBCASE("32-bit")
		{
			random_mono_to_stereo(uaudio::BLOCK_ALIGN_32_BIT_MONO);
		}
	}
	SUBCASE("Stereo to Mono (pre-defined)")
	{
		uaudio::logger::print_cyan();
		printf("[STEREO TO MONO]\n");
		uaudio::logger::print_white();
		SUBCASE("16-bit")
		{
			uaudio::logger::print_cyan();
			printf("[16 bit (pre-defined)]\n");
			uaudio::logger::print_white();

			const uint16_t block_align = uaudio::BLOCK_ALIGN_16_BIT_STEREO;

			std::array<unsigned char, block_align> dat = {
				2,
				0,
				2,
				0,
			};
			unsigned char *data = dat.data();

			uint32_t size = static_cast<uint32_t>(dat.size());

			std::array<unsigned char, dat.size() / 2> EXPECTED = {
				2,
				0,
			};

			CHECK(size == dat.size());
			data = uaudio::conversion::ConvertStereoToMono(data, size, block_align);
			CHECK(size == EXPECTED.size());

			for (uint32_t i = 0; i < size; i++)
				CHECK(data[i] == EXPECTED[i]);

			uaudio::logger::print_green();
			printf("SUCCESS!\n");
			uaudio::logger::print_white();
		}
		SUBCASE("24-bit")
		{
			uaudio::logger::print_cyan();
			printf("[24 bit (pre-defined)]\n");
			uaudio::logger::print_white();

			const uint16_t block_align = uaudio::BLOCK_ALIGN_24_BIT_STEREO;

			std::array<unsigned char, block_align> dat = {
				2,
				0,
				6,
				2,
				0,
				6,
			};
			unsigned char *data = dat.data();

			uint32_t size = static_cast<uint32_t>(dat.size());

			std::array<unsigned char, dat.size() / 2> EXPECTED = {
				2,
				0,
				6,
			};

			CHECK(size == dat.size());
			data = uaudio::conversion::ConvertStereoToMono(data, size, block_align);
			CHECK(size == EXPECTED.size());

			for (uint32_t i = 0; i < size; i++)
				CHECK(data[i] == EXPECTED[i]);

			uaudio::logger::print_green();
			printf("SUCCESS!\n");
			uaudio::logger::print_white();
		}
		SUBCASE("32-bit")
		{
			uaudio::logger::print_cyan();
			printf("[32 bit (pre-defined)]\n");
			uaudio::logger::print_white();

			const uint16_t block_align = uaudio::BLOCK_ALIGN_32_BIT_STEREO;

			std::array<unsigned char, block_align> dat = {
				2,
				0,
				6,
				4,
				2,
				0,
				6,
				4,
			};
			unsigned char *data = dat.data();

			uint32_t size = static_cast<uint32_t>(dat.size());

			std::array<unsigned char, dat.size() / 2> EXPECTED = {
				2,
				0,
				6,
				4,
			};

			CHECK(size == dat.size());
			data = uaudio::conversion::ConvertStereoToMono(data, size, block_align);
			CHECK(size == EXPECTED.size());

			for (uint32_t i = 0; i < size; i++)
				CHECK(data[i] == EXPECTED[i]);

			uaudio::logger::print_green();
			printf("SUCCESS!\n");
			uaudio::logger::print_white();
		}
	}
	SUBCASE("Stereo to Mono (random)")
	{
		uaudio::logger::print_cyan();
		printf("[STEREO TO MONO (random)]\n");
		uaudio::logger::print_white();
		SUBCASE("16-bit")
		{
			random_stereo_to_mono(uaudio::BLOCK_ALIGN_16_BIT_STEREO);
		}
		SUBCASE("24-bit")
		{
			random_stereo_to_mono(uaudio::BLOCK_ALIGN_24_BIT_STEREO);
		}
		SUBCASE("32-bit")
		{
			random_stereo_to_mono(uaudio::BLOCK_ALIGN_32_BIT_STEREO);
		}
	}
}

TEST_CASE("Audio Loading")
{
	SUBCASE("Existing file")
	{
		uaudio::logger::print_cyan();
		printf("[OPENING EXISTING VALID FILE]\n");
		uaudio::logger::print_white();
		uaudio::WaveFormat format;
		FILE *file = nullptr;
		uaudio::WaveReader::LoadSound("3 Steps Ahead - Drop It (Happy Mix).wav", format, file);

		CHECK(format.filledRiffChunk);

		CHECK(strcmp(std::string(&format.riffChunk.chunkId[0], &format.riffChunk.chunkId[0] + std::size(format.riffChunk.chunkId)).c_str(), uaudio::RIFF_CHUNK_ID) == 0);
		CHECK(format.riffChunk.chunkSize == 39962972);
		CHECK(strcmp(std::string(&format.riffChunk.format[0], &format.riffChunk.format[0] + std::size(format.riffChunk.format)).c_str(), uaudio::FMT_CHUNK_FORMAT) == 0);

		CHECK(format.filledFmtChunk);

		CHECK(strcmp(std::string(&format.fmtChunk.chunkId[0], &format.fmtChunk.chunkId[0] + std::size(format.fmtChunk.chunkId)).c_str(), uaudio::FMT_CHUNK_ID) == 0);
		CHECK(format.fmtChunk.chunkSize == uaudio::GetChunkSize<uaudio::FMT_Chunk>());
		CHECK(format.fmtChunk.audioFormat == uaudio::WAV_FORMAT_PCM);
		CHECK(format.fmtChunk.numChannels == uaudio::WAVE_CHANNELS_STEREO);
		CHECK(format.fmtChunk.sampleRate == uaudio::WAVE_SAMPLE_RATE_44100);
		CHECK(format.fmtChunk.byteRate == 176400);
		CHECK(format.fmtChunk.blockAlign == uaudio::BLOCK_ALIGN_16_BIT_STEREO);
		CHECK(format.fmtChunk.bitsPerSample == uaudio::WAVE_BPS_16);

		CHECK(format.filledDataChunk);

		CHECK(strcmp(std::string(&format.dataChunk.chunkId[0], &format.dataChunk.chunkId[0] + std::size(format.dataChunk.chunkId)).c_str(), uaudio::DATA_CHUNK_ID) == 0);
		CHECK(format.dataChunk.chunkSize == 39962928);

		uaudio::logger::print_green();
		printf("SUCCESS!\n");
		uaudio::logger::print_white();
	}
	SUBCASE("Existing invalid file")
	{
		uaudio::logger::print_cyan();
		printf("[OPENING EXISTING INVALID FILE]\n");
		uaudio::logger::print_white();
		uaudio::WaveFormat format;
		FILE *file = nullptr;
		uaudio::WaveReader::LoadSound("Broken.wav", format, file);

		CHECK(!format.filledRiffChunk);
		CHECK(!format.filledFmtChunk);
		CHECK(!format.filledDataChunk);
		CHECK(!format.filledAcidChunk);
		CHECK(!format.filledBextChunk);

		uaudio::logger::print_green();
		printf("SUCCESS!\n");
		uaudio::logger::print_white();
	}
	SUBCASE("Non-existing format")
	{
		uaudio::logger::print_cyan();
		printf("[OPENING NON-EXISTENT format**\n");
		uaudio::logger::print_white();
		uaudio::WaveFormat format;
		FILE *file = nullptr;
		uaudio::WaveReader::LoadSound("test.wav", format, file);

		CHECK(!format.filledRiffChunk);
		CHECK(!format.filledFmtChunk);
		CHECK(!format.filledDataChunk);
		CHECK(!format.filledAcidChunk);
		CHECK(!format.filledBextChunk);

		uaudio::logger::print_green();
		printf("SUCCESS!\n");
		uaudio::logger::print_white();
	}
}