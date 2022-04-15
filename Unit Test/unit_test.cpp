#include <wav/WaveConverter.h>
#include <array>
#include <vector>

#include "doctest.h"
#include "wav/WaveFile.h"
#include "wav/WaveReader.h"

void print_yellow()
{
	printf("\033[0;33m");
}

void print_white()
{
	printf("\033[0;37m");
}

void print_green()
{
	printf("\033[0;32m");
}

void random_mono_to_stereo(uint16_t block_align)
{
	print_yellow();
	printf("** %i bit (random) **\n", block_align * 8);
	print_white();

	srand((unsigned)time(0));

	std::vector<unsigned char> dat;
	uint32_t size = (rand() % 10) + 1;
	size *= block_align;

	for (uint32_t i = 0; i < size; i++)
		dat.push_back(rand() % 100);

	unsigned char* data = dat.data();

	std::vector<unsigned char> EXPECTED(size * 2);

	int newIndex = 0;
	for (uint16_t i = 0; i <= size - block_align; i += block_align)
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

	print_green();
	printf("SUCCESS!\n");
	print_white();
}

void random_stereo_to_mono(uint16_t block_align)
{
	print_yellow();
	printf("** %i bit (random) **\n", block_align * 8);
	print_white();

	block_align *= 2;

	srand((unsigned)time(0));

	std::vector<unsigned char> dat;
	uint32_t size = (rand() % 10) + 1;
	size *= block_align;

	for (uint32_t i = 0; i < size; i++)
		dat.push_back(rand() % 100);

	unsigned char* data = dat.data();

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

	print_green();
	printf("SUCCESS!\n");
	print_white();
}

TEST_CASE("Testing Hash Function")
{
	const std::string _stringLit = "Rs_239Ksa*--A";
	const char* _charptr = "Rs_239Ksa*--a";

	SUBCASE("Lowercase and uppercase")
	{
		print_yellow();
		printf("** LOWERCASE AND UPPERCASE IN HASH FUNCTION **\n");
		print_white();

		uaudio::Hash stdStringA = uaudio::GetHash(_stringLit);
		uaudio::Hash stdStringB = uaudio::GetHash(_stringLit.c_str());
		uaudio::Hash charPtrA = uaudio::GetHash(_charptr);

		CHECK(stdStringA == stdStringB);
		CHECK(stdStringB != charPtrA);

		print_green();
		printf("SUCCESS!\n");
		print_white();
	}
	SUBCASE("Collision test")
	{
		print_yellow();
		printf("** COLLISION TEST IN HASH FUNCTION **\n");
		print_white();

		std::array<const char*, 51> tests = { "Cotn_Containers.png", "Cotn_Background.png", "Cotn_Bat.png",
			"Cotn_Cadence.png", "Cotn_Font.png", "Cotn_Ghost.png", "Cotn_MB_Bat.png", "Cotn_MB_Dragon.png", "Cotn_MB_Minotaur.png",
			"Cotn_Monkey.png", "Cotn_Shopkeeper.png", "Cotn_Skeleton.png", "Cotn_Slime.png", "Cotn_Traps.png", "Cotn_Walls.png",
			"Cotn_Zombie.png", "Custom_EntitySpawn.png", "ReworkShort_Background.png","test.json", "Zawarudo.wav",
			"Deltarune - Field of Hopes and Dreams.wav_new.wav", "Headhunterz - Home (Extended).wav", "Undefiant - Best I Ever Could (Extended Mix).wav",
			"Barrel.json","Crate.json", "Player.json","miniboss1.json", "miniboss2.json","miniboss3.json",
			"miniboss4.json","normal1.json", "normal2.json","normal3.json", "normal4.json","normal5.json", "normal6.json",
			"secret1.json", "secret2.json","secret3.json", "secret4.json","shop1.json", "start0.json","hello", "h","he", "hel","metronome_01.wav",
			"metronome_02.wav", "default_sound.wav", "default_soundtrack.wav", "fontawesome.ttf"
		};

		std::vector<uaudio::Hash> hashes;

		hashes.reserve(tests.size());

		for (size_t i = 0; i < tests.size(); i++)
			hashes.push_back(uaudio::GetHash(tests[i]));

		for (size_t i = 0; i < tests.size(); i++)
			for (size_t j = 0; j < tests.size(); j++)
				if (i != j)
				{
					printf("Tested %s against %s\n", tests[i], tests[j]);
					CHECK(hashes[i] != hashes[j]);
				}

		print_green();
		printf("SUCCESS!\n");
		print_white();
	}
}

TEST_CASE("Audio Conversion")
{
	SUBCASE("Mono to Stereo (pre-defined)")
	{
		print_yellow();
		printf("** MONO TO STEREO (pre-defined) **\n");
		print_white();
		SUBCASE("16-bit")
		{
			print_yellow();
			printf("** 16 bit (pre-defined) **\n");
			print_white();

			const uint16_t block_align = 2;

			std::array<unsigned char, block_align> dat = {
				2,
				0,
			};
			unsigned char* data = dat.data();

			uint32_t size = dat.size();

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

			print_green();
			printf("SUCCESS!\n");
			print_white();
		}
		SUBCASE("24-bit")
		{
			print_yellow();
			printf("** 24 bit (pre-defined) **\n");
			print_white();

			const uint16_t block_align = 3;

			std::array<unsigned char, block_align> dat = {
				2,
				0,
				5,
			};
			unsigned char* data = dat.data();

			uint32_t size = dat.size();

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

			print_green();
			printf("SUCCESS!\n");
			print_white();
		}
		SUBCASE("32-bit")
		{
			print_yellow();
			printf("** 32 bit (pre-defined) **\n");
			print_white();

			const uint16_t block_align = 4;

			std::array<unsigned char, block_align> dat = {
				2,
				0,
				5,
				1,
			};
			unsigned char* data = dat.data();

			uint32_t size = dat.size();

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

			print_green();
			printf("SUCCESS!\n");
			print_white();
		}
	}
	SUBCASE("Mono to Stereo (random)")
	{
		print_yellow();
		printf("** MONO TO STEREO (random) **\n");
		print_white();

		SUBCASE("16-bit")
		{
			random_mono_to_stereo(2);
		}
		SUBCASE("24-bit")
		{
			random_mono_to_stereo(3);
		}
		SUBCASE("32-bit")
		{
			random_mono_to_stereo(4);
		}
	}
	SUBCASE("Stereo to Mono (pre-defined)")
	{
		print_yellow();
		printf("** STEREO TO MONO **\n");
		print_white();
		SUBCASE("16-bit")
		{
			print_yellow();
			printf("** 16 bit (pre-defined) **\n");
			print_white();

			const uint16_t block_align = 4;

			std::array<unsigned char, block_align> dat = {
				2,
				0,
				2,
				0,
			};
			unsigned char* data = dat.data();

			uint32_t size = dat.size();

			std::array<unsigned char, dat.size() / 2> EXPECTED = {
				2,
				0,
			};

			CHECK(size == dat.size());
			data = uaudio::conversion::ConvertStereoToMono(data, size, block_align);
			CHECK(size == EXPECTED.size());

			for (uint32_t i = 0; i < size; i++)
				CHECK(data[i] == EXPECTED[i]);

			print_green();
			printf("SUCCESS!\n");
			print_white();
		}
		SUBCASE("24-bit")
		{
			print_yellow();
			printf("** 24 bit (pre-defined) **\n");
			print_white();

			const uint16_t block_align = 6;

			std::array<unsigned char, block_align> dat = {
				2,
				0,
				6,
				2,
				0,
				6,
			};
			unsigned char* data = dat.data();

			uint32_t size = dat.size();

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

			print_green();
			printf("SUCCESS!\n");
			print_white();
		}
		SUBCASE("32-bit")
		{
			print_yellow();
			printf("** 32 bit (pre-defined) **\n");
			print_white();

			const uint16_t block_align = 8;

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
			unsigned char* data = dat.data();

			uint32_t size = dat.size();

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

			print_green();
			printf("SUCCESS!\n");
			print_white();
		}
	}
	SUBCASE("Stereo to Mono (random)")
	{
		print_yellow();
		printf("** STEREO TO MONO (random) **\n");
		print_white();
		SUBCASE("16-bit")
		{
			random_stereo_to_mono(2);
		}
		SUBCASE("24-bit")
		{
			random_stereo_to_mono(3);
		}
		SUBCASE("32-bit")
		{
			random_stereo_to_mono(4);
		}
	}
}

TEST_CASE("Audio Loading")
{
	SUBCASE("Existing file")
	{
		print_yellow();
		printf("** OPENING EXISTING VALID FILE **\n");
		print_white();
		uaudio::WavFormat file = uaudio::WaveReader::LoadSound("3 Steps Ahead - Drop It (Happy Mix).wav");

		CHECK(file.filledRiffChunk);

		CHECK(strcmp(std::string(&file.riffChunk.chunkId[0], &file.riffChunk.chunkId[0] + std::size(file.riffChunk.chunkId)).c_str(), "RIFF") == 0);
		CHECK(file.riffChunk.chunkSize == 39962972);
		CHECK(strcmp(std::string(&file.riffChunk.format[0], &file.riffChunk.format[0] + std::size(file.riffChunk.format)).c_str(), "WAVE") == 0);

		CHECK(file.filledFmtChunk);

		CHECK(strcmp(std::string(&file.fmtChunk.chunkId[0], &file.fmtChunk.chunkId[0] + std::size(file.fmtChunk.chunkId)).c_str(), "fmt ") == 0);
		CHECK(file.fmtChunk.chunkSize == 16);
		CHECK(file.fmtChunk.audioFormat == 1);
		CHECK(file.fmtChunk.numChannels == 2);
		CHECK(file.fmtChunk.sampleRate == 44100);
		CHECK(file.fmtChunk.byteRate == 176400);
		CHECK(file.fmtChunk.blockAlign == 4);
		CHECK(file.fmtChunk.bitsPerSample == 16);

		CHECK(file.filledDataChunk);

		CHECK(strcmp(std::string(&file.dataChunk.chunkId[0], &file.dataChunk.chunkId[0] + std::size(file.dataChunk.chunkId)).c_str(), "data") == 0);
		CHECK(file.dataChunk.chunkSize == 39962928);

		print_green();
		printf("SUCCESS!\n");
		print_white();
	}
	SUBCASE("Existing invalid file")
	{
		print_yellow();
		printf("** OPENING EXISTING INVALID FILE **\n");
		print_white();
		uaudio::WavFormat file = uaudio::WaveReader::LoadSound("Broken.wav");

		CHECK(!file.filledRiffChunk);
		CHECK(!file.filledFmtChunk);
		CHECK(!file.filledDataChunk);
		CHECK(!file.filledAcidChunk);
		CHECK(!file.filledBextChunk);

		print_green();
		printf("SUCCESS!\n");
		print_white();
	}
	SUBCASE("Non-existing file")
	{
		print_yellow();
		printf("** OPENING NON-EXISTENT FILE**\n");
		print_white();
		uaudio::WavFormat file = uaudio::WaveReader::LoadSound("test.wav");

		CHECK(!file.filledRiffChunk);
		CHECK(!file.filledFmtChunk);
		CHECK(!file.filledDataChunk);
		CHECK(!file.filledAcidChunk);
		CHECK(!file.filledBextChunk);

		print_green();
		printf("SUCCESS!\n");
		print_white();
	}
}