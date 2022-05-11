#include <uaudio/wave/low_level/WaveConverter.h>
#include <array>
#include <vector>

#include "doctest.h"
#include <uaudio/utils/Logger.h>
#include <uaudio/wave/high_level/WaveChunks.h>
#include <uaudio/wave/low_level/WaveReader.h>

#include <uaudio/wave/low_level/WaveFormat.h>

void PRINT_ARRAY(const char *text, std::vector<unsigned char> dat)
{
	printf("%s\n", text);
	uaudio::logger::print_white();
	printf("[");
	for (size_t i = 0; i < dat.size(); i++)
	{
		uaudio::logger::print_yellow();
		printf("%i", dat[i]);
		uaudio::logger::print_white();
		if (i != dat.size() - 1)
			printf(", ");
	}
	uaudio::logger::print_white();
	printf("]\n");
}

void random_mono_to_stereo(uint16_t block_align)
{
	uaudio::logger::log_info("%s[MONO TO STEREO %i-BIT (random)]%s", uaudio::logger::COLOR_CYAN, block_align * 8, uaudio::logger::COLOR_WHITE);

	srand((unsigned)time(0));

	std::vector<unsigned char> dat;
	uint32_t size = (rand() % 10) + 1;
	size *= block_align;

	unsigned char* new_data = reinterpret_cast<unsigned char*>(malloc(uaudio::conversion::CalculateMonoToStereoSize(size)));

	for (uint32_t i = 0; i < size; i++)
		dat.push_back(rand() % 100);

	PRINT_ARRAY("INPUT: ", dat);

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

	PRINT_ARRAY("EXPECTED: ", EXPECTED);

	CHECK(size == dat.size());
	uaudio::conversion::ConvertMonoToStereo(new_data, data, size, block_align);
	CHECK(size == EXPECTED.size());

	for (uint32_t i = 0; i < size; i++)
		CHECK(new_data[i] == EXPECTED[i]);

	PRINT_ARRAY("RESULT: ", EXPECTED);

	uaudio::logger::log_success("%s[MONO TO STEREO %i-BIT (random)]%s\n", uaudio::logger::COLOR_CYAN, block_align * 8, uaudio::logger::COLOR_WHITE);
}

void random_stereo_to_mono(uint16_t block_align)
{
	uaudio::logger::log_info("%s[STEREO TO MONO %i-BIT (random)]%s", uaudio::logger::COLOR_CYAN, block_align / 2 * 8, uaudio::logger::COLOR_WHITE);

	srand((unsigned)time(0));

	std::vector<unsigned char> dat;
	uint32_t size = (rand() % 10) + 1;
	size *= block_align;

	unsigned char* new_data = reinterpret_cast<unsigned char*>(malloc(uaudio::conversion::CalculateStereoToMonoSize(size)));

	for (uint32_t i = 0; i < size; i++)
		dat.push_back(rand() % 100);

	PRINT_ARRAY("INPUT: ", dat);

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

	PRINT_ARRAY("EXPECTED: ", EXPECTED);

	CHECK(size == dat.size());
	uaudio::conversion::ConvertStereoToMono(new_data, data, size, block_align);
	CHECK(size == EXPECTED.size());

	for (uint32_t i = 0; i < size; i++)
		CHECK(new_data[i] == EXPECTED[i]);

	PRINT_ARRAY("RESULT: ", EXPECTED);

	uaudio::logger::log_success("%s[STEREO TO MONO %i-BIT (random)]%s\n", uaudio::logger::COLOR_CYAN, block_align / 2 * 8, uaudio::logger::COLOR_WHITE);
}

TEST_CASE("Testing Hash Function")
{
	const std::string _stringLit = "Rs_239Ksa*--A";
	const char *_charptr = "Rs_239Ksa*--a";

	SUBCASE("Lowercase and uppercase")
	{
		uaudio::logger::log_info("%s[LOWERCASE AND UPPERCASE IN HASH FUNCTION]%s", uaudio::logger::COLOR_CYAN, uaudio::logger::COLOR_WHITE);

		uaudio::UAUDIO_DEFAULT_HASH stdStringA = uaudio::UAUDIO_DEFAULT_HASH_FUNCTION(_stringLit.c_str());
		uaudio::UAUDIO_DEFAULT_HASH stdStringB = uaudio::UAUDIO_DEFAULT_HASH_FUNCTION(_stringLit.c_str());
		uaudio::UAUDIO_DEFAULT_HASH charPtrA = uaudio::UAUDIO_DEFAULT_HASH_FUNCTION(_charptr);

		CHECK(stdStringA == stdStringB);
		CHECK(stdStringB != charPtrA);

		uaudio::logger::log_success("%s[LOWERCASE AND UPPERCASE IN HASH FUNCTION]%s\n", uaudio::logger::COLOR_CYAN, uaudio::logger::COLOR_WHITE);
	}
	SUBCASE("Collision test")
	{
		uaudio::logger::log_info("%s[COLLISION TEST IN HASH FUNCTION]%s", uaudio::logger::COLOR_CYAN, uaudio::logger::COLOR_WHITE);

		std::array<const char *, 74> tests = {
			"Cotn_Containers.wav",
			"Cotn_Background.wav",
			"Cotn_Bat.wav",
			"Test.wav",
			"Bloodfire ft. Roma Bush - Higher To The Sky (Edit).wav",
			"Bloodfire ft. Roma Bush - Higher To The Sky (Extended).wav",
			"D'ort - fReAk sHoW.wav",
			"fReAk sHoW.wav",
			"D'ort - ENGE balkan swa.wav",
			"ENGE balkan swa.wav",
			"D'ort - Ой, кололи кабана (Steek het zwijn).wav",
			"D'ort - Cirque du D'ort.wav",
			"D'ort - Lyulin Extended Mix.wav",
			"D'ort - Gekapseisd Extended Mix.wav",
			"D'ort - D'accordéon Extended Mix.wav",
			"D'ort - Balkan Extended Mix.wav",
			"D'ort - Tartaros Extended Mix.wav",
			"D'ort - Minotaur Extended Mix.wav",
			"D'ort - Ecclesia Extended Mix.wav",
			"D'ort - Adath Extended Mix.wav",
			"Dr. Rude x Runeforce - Wave of Destruction (Extended) 100222.wav",
			"2 Brothers on the 4th Floor - Fairytales (Re-Style Remix) (Extended Mix).wav",
			"2 Brothers on the 4th Floor - Fairytales (Re-Style Remix).wav",
			"Rayvolt - Crashing Waves (Extended Mix).wav",
			"Rayvolt - Crashing Waves.wav",
			"Re-Style & Rayvolt - Breathless (Extended Mix)",
			"Re-Style & Rayvolt - Enrapture (Extended Mix).wav",
			"Re-Style - Kryptonite ft Cammie Robinson (Extended Mix).wav",
			"Re-Style - Towards The Sun (Vertex & Rayvolt Remix) (Extended Mix).wav",
			"Rayvolt - The Heat (Extended Mix).wav",
			"Rayvolt - Uprising (Extended Mix).wav",
			"Relianze - Sustain Life (Extended Mix).wav",
			"Rayvolt - Crackin' The Bass (Extended Mix).wav",
			"Rayvolt - Overcome (Extended Mix).wav",
			"Rayvolt - Revolutionary (Extended Mix).wav",
			"D'ort & Undefiant - First Light (Extended Mix).wav",
			"Hysta & Advanced Dealer - Atlantic (Extended Mix).wav",
			"Juju Rush - Catching Fire (Extended Mix).wav",
			"D'ort - Redemption (Extended Mix).wav",
			"D'ort - Reflections (Extended Mix).wav",
			"D'ort - Tonight (Extended Mix).wav",
			"D'ort - Mystic (Extended Mix).wav",
			"D'ort - Piratenfestijn (Extended Mix).wav",
			"Death Punch - Nowhere Warm (Extended Mix).wav",
			"D'ort - Arcade (Extended Mix).wav",
			"Bionator Project - Headbanger (Extended Mix).wav",
			"Crypton & Rayvolt - If You Know (Extended Mix).wav",
			"Cyclon & Vertex - One (Extended Mix).wav",
			"Access One - Zodiac (Extended Mix).wav",
			"Advanced Dealer - Exploration Space (Extended Mix).wav",
			"Vertex - Run It Up (Extended Mix).wav",
			"Vertex - Radiance (Extended Mix).wav",
			"Vertex - Get Down (Extended Mix).wav",
			"Vertex - Helix ft Lune (Extended Mix).wav",
			"Undefiant - If I Fall (Extended Mix).wav",
			"Undefiant - Wild Love (Extended Mix).wav",
			"Runeforce - Partycrasher (Extended Mix).wav",
			"Runeforce ft Lune - Lonely Soldier (Extended Mix).wav",
			"Undefiant - Be My Light (Extended Mix).wav",
			"Runeforce - Delirium (Extended Mix).wav",
			"Runeforce - From Within (Extended Mix).wav",
			"Re-Style x Runeforce - A New Dawn (Extended Mix).wav",
			"Re-Style x Runeforce - A New Dawn (Festival Mix).wav",
			"Re-Style & Vertex - Shadow World (Extended Mix).wav",
			"Donnie & René Froger - Bon Gepakt (D'ort's Frenchcore Is Ook Volksmuziek Edit) (Extended Mix).wav",
			"Relianze - Turn the Levels Up (Master v.6) (3-9-2021).wav",
			"ENDER X MASSIVE DISORDER - NUMB (Stem Mix-MasTer) 090921.wav",
			"Valido & MELOLIFE - Fight For You (Original Mix) 21-10 Master.wav",
			"Melolife & Dr.Donkz - Masters Of The Soul (Extended Mix).mp3",
			"D'ort - Gekapseisd.wav",
			"Adaro - I'm Alive (Rebirth 2019 Anthem) (Re-Style & Vertex Remix).wav",
			"Re-Style__Sefa_-_Shadow_World.mp3",
			"Melolife & Valido - Afraid Of My Heart (Original Mix) 16bit master.wav",
			"Code Red & D'ort feat. Ceros - Chainsaw (Extended Mix).wav",
		};

		std::vector<uaudio::UAUDIO_DEFAULT_HASH> hashes;

		hashes.reserve(tests.size());

		for (size_t i = 0; i < tests.size(); i++)
			hashes.push_back(uaudio::GetHash(tests[i]));

		for (size_t i = 0; i < tests.size(); i++)
		{
			const char *color = i % 2 == 0 ? uaudio::logger::COLOR_YELLOW : uaudio::logger::COLOR_BLUE;
			uaudio::logger::log_info("%s[COLLISION TEST IN HASH FUNCTION]%s Testing %s%s%s", uaudio::logger::COLOR_CYAN, uaudio::logger::COLOR_WHITE, color, tests[i], uaudio::logger::COLOR_WHITE);
			for (size_t j = 0; j < tests.size(); j++)
				if (i != j)
					CHECK(hashes[i] != hashes[j]);
		}

		uaudio::logger::log_success("%s[COLLISION TEST IN HASH FUNCTION]%s\n", uaudio::logger::COLOR_CYAN, uaudio::logger::COLOR_WHITE);
	}
}

TEST_CASE("Audio Conversion")
{
	SUBCASE("Mono to Stereo (pre-defined)")
	{
		SUBCASE("16-bit")
		{
			uaudio::logger::log_info("%s[MONO TO STEREO 16-BIT (pre-defined)]%s", uaudio::logger::COLOR_CYAN, uaudio::logger::COLOR_WHITE);

			const uint16_t block_align = uaudio::BLOCK_ALIGN_16_BIT_MONO;

			std::vector<unsigned char> dat = {
				2,
				0,
			};
			unsigned char *data = dat.data();

			PRINT_ARRAY("INPUT: ", dat);

			uint32_t size = static_cast<uint32_t>(dat.size());

			unsigned char* new_data = reinterpret_cast<unsigned char*>(malloc(uaudio::conversion::CalculateMonoToStereoSize(size)));

			std::vector<unsigned char> EXPECTED = {
				2,
				0,
				2,
				0,
			};

			PRINT_ARRAY("EXPECTED: ", EXPECTED);

			CHECK(size == dat.size());
			uaudio::conversion::ConvertMonoToStereo(new_data, data, size, block_align);
			CHECK(size == EXPECTED.size());

			for (uint32_t i = 0; i < size; i++)
				CHECK(new_data[i] == EXPECTED[i]);

			PRINT_ARRAY("RESULT: ", EXPECTED);

			uaudio::logger::log_success("%s[MONO TO STEREO 16-BIT (pre-defined)]%s\n", uaudio::logger::COLOR_CYAN, uaudio::logger::COLOR_WHITE);
		}
		SUBCASE("24-bit")
		{
			uaudio::logger::log_info("%s[MONO TO STEREO 24-BIT (pre-defined)]%s", uaudio::logger::COLOR_CYAN, uaudio::logger::COLOR_WHITE);

			const uint16_t block_align = uaudio::BLOCK_ALIGN_24_BIT_MONO;

			std::vector<unsigned char> dat = {
				2,
				0,
				5,
			};

			PRINT_ARRAY("INPUT: ", dat);

			unsigned char *data = dat.data();

			uint32_t size = static_cast<uint32_t>(dat.size());

			unsigned char* new_data = reinterpret_cast<unsigned char*>(malloc(uaudio::conversion::CalculateMonoToStereoSize(size)));

			std::vector<unsigned char> EXPECTED = {
				2,
				0,
				5,
				2,
				0,
				5,
			};

			PRINT_ARRAY("EXPECTED: ", EXPECTED);

			CHECK(size == dat.size());
			uaudio::conversion::ConvertMonoToStereo(new_data, data, size, block_align);
			CHECK(size == EXPECTED.size());

			for (uint32_t i = 0; i < size; i++)
				CHECK(new_data[i] == EXPECTED[i]);

			PRINT_ARRAY("RESULT: ", EXPECTED);

			uaudio::logger::log_success("%s[MONO TO STEREO 24-BIT (pre-defined)]%s\n", uaudio::logger::COLOR_CYAN, uaudio::logger::COLOR_WHITE);
		}
		SUBCASE("32-bit")
		{
			uaudio::logger::log_info("%s[MONO TO STEREO 32-BIT (pre-defined)]%s", uaudio::logger::COLOR_CYAN, uaudio::logger::COLOR_WHITE);

			const uint16_t block_align = uaudio::BLOCK_ALIGN_32_BIT_MONO;

			std::vector<unsigned char> dat = {
				2,
				0,
				5,
				1,
			};

			PRINT_ARRAY("INPUT: ", dat);

			unsigned char *data = dat.data();

			uint32_t size = static_cast<uint32_t>(dat.size());

			unsigned char* new_data = reinterpret_cast<unsigned char*>(malloc(uaudio::conversion::CalculateMonoToStereoSize(size)));

			std::vector<unsigned char> EXPECTED = {
				2,
				0,
				5,
				1,
				2,
				0,
				5,
				1,
			};

			PRINT_ARRAY("EXPECTED: ", EXPECTED);

			CHECK(size == dat.size());
			uaudio::conversion::ConvertMonoToStereo(new_data, data, size, block_align);
			CHECK(size == EXPECTED.size());

			for (uint32_t i = 0; i < size; i++)
				CHECK(new_data[i] == EXPECTED[i]);

			PRINT_ARRAY("RESULT: ", EXPECTED);

			uaudio::logger::log_success("%s[MONO TO STEREO 32-BIT (pre-defined)]%s\n", uaudio::logger::COLOR_CYAN, uaudio::logger::COLOR_WHITE);
		}
	}
	SUBCASE("Mono to Stereo (random)")
	{
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
		SUBCASE("16-bit")
		{
			uaudio::logger::log_info("%s[STEREO TO MONO 16-BIT (pre-defined)]%s", uaudio::logger::COLOR_CYAN, uaudio::logger::COLOR_WHITE);

			const uint16_t block_align = uaudio::BLOCK_ALIGN_16_BIT_STEREO;

			std::vector<unsigned char> dat = {
				2,
				0,
				2,
				0,
			};

			PRINT_ARRAY("INPUT: ", dat);

			unsigned char *data = dat.data();

			uint32_t size = static_cast<uint32_t>(dat.size());

			unsigned char* new_data = reinterpret_cast<unsigned char*>(malloc(uaudio::conversion::CalculateStereoToMonoSize(size)));

			std::vector<unsigned char> EXPECTED = {
				2,
				0,
			};

			PRINT_ARRAY("EXPECTED: ", EXPECTED);

			CHECK(size == dat.size());
			uaudio::conversion::ConvertStereoToMono(new_data, data, size, block_align);
			CHECK(size == EXPECTED.size());

			for (uint32_t i = 0; i < size; i++)
				CHECK(new_data[i] == EXPECTED[i]);

			PRINT_ARRAY("RESULT: ", EXPECTED);

			uaudio::logger::log_success("%s[STEREO TO MONO 16-BIT (pre-defined)]%s\n", uaudio::logger::COLOR_CYAN, uaudio::logger::COLOR_WHITE);
		}
		SUBCASE("24-bit")
		{
			uaudio::logger::log_info("%s[STEREO TO MONO 24-BIT (pre-defined)]%s", uaudio::logger::COLOR_CYAN, uaudio::logger::COLOR_WHITE);

			const uint16_t block_align = uaudio::BLOCK_ALIGN_24_BIT_STEREO;

			std::vector<unsigned char> dat = {
				2,
				0,
				6,
				2,
				0,
				6,
			};

			PRINT_ARRAY("INPUT: ", dat);

			unsigned char *data = dat.data();

			uint32_t size = static_cast<uint32_t>(dat.size());

			unsigned char* new_data = reinterpret_cast<unsigned char*>(malloc(uaudio::conversion::CalculateStereoToMonoSize(size)));

			std::vector<unsigned char> EXPECTED = {
				2,
				0,
				6,
			};

			PRINT_ARRAY("EXPECTED: ", EXPECTED);

			CHECK(size == dat.size());
			uaudio::conversion::ConvertStereoToMono(new_data, data, size, block_align);
			CHECK(size == EXPECTED.size());

			for (uint32_t i = 0; i < size; i++)
				CHECK(new_data[i] == EXPECTED[i]);

			PRINT_ARRAY("RESULT: ", EXPECTED);

			uaudio::logger::log_success("%s[STEREO TO MONO 24-BIT (pre-defined)]%s\n", uaudio::logger::COLOR_CYAN, uaudio::logger::COLOR_WHITE);
		}
		SUBCASE("32-bit")
		{
			uaudio::logger::log_info("%s[STEREO TO MONO 32-BIT (pre-defined)]%s", uaudio::logger::COLOR_CYAN, uaudio::logger::COLOR_WHITE);

			const uint16_t block_align = uaudio::BLOCK_ALIGN_32_BIT_STEREO;

			std::vector<unsigned char> dat = {
				2,
				0,
				6,
				4,
				2,
				0,
				6,
				4,
			};

			PRINT_ARRAY("INPUT: ", dat);

			unsigned char *data = dat.data();

			uint32_t size = static_cast<uint32_t>(dat.size());

			unsigned char* new_data = reinterpret_cast<unsigned char*>(malloc(uaudio::conversion::CalculateStereoToMonoSize(size)));

			std::vector<unsigned char> EXPECTED = {
				2,
				0,
				6,
				4,
			};

			PRINT_ARRAY("EXPECTED: ", EXPECTED);

			CHECK(size == dat.size());
			uaudio::conversion::ConvertStereoToMono(new_data, data, size, block_align);
			CHECK(size == EXPECTED.size());

			for (uint32_t i = 0; i < size; i++)
				CHECK(new_data[i] == EXPECTED[i]);

			PRINT_ARRAY("RESULT: ", EXPECTED);

			uaudio::logger::log_success("%s[STEREO TO MONO 32-BIT (pre-defined)]%s\n", uaudio::logger::COLOR_CYAN, uaudio::logger::COLOR_WHITE);
		}
	}
	SUBCASE("Stereo to Mono (random)")
	{
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

TEST_CASE("Miscellaneous")
{
	SUBCASE("Clamp")
	{
		uaudio::logger::log_info("%s[CLAMP]%s", uaudio::logger::COLOR_CYAN, uaudio::logger::COLOR_WHITE);

		float value = 5.0f;
		value = uaudio::utils::clamp(value, 0.0f, 4.0f);

		CHECK(value == 4.0f);

		value = -1.0f;
		value = uaudio::utils::clamp(value, 0.0f, 4.0f);

		CHECK(value == 0.0f);

		uaudio::logger::log_success("%s[CLAMP]%s\n", uaudio::logger::COLOR_CYAN, uaudio::logger::COLOR_WHITE);
	}
	SUBCASE("SECONDS TO MILLISECONDS")
	{
		uaudio::logger::log_info("%s[SECONDS TO MILLISECONDS]%s", uaudio::logger::COLOR_CYAN, uaudio::logger::COLOR_WHITE);

		float value = 1.0f;
		value = uaudio::utils::SecondsToMilliseconds(value);

		CHECK(value == 1000.0f);

		value = 2.0f;
		value = uaudio::utils::SecondsToMilliseconds(value);

		CHECK(value == 2000.0f);

		value = 2.5f;
		value = uaudio::utils::SecondsToMilliseconds(value);

		CHECK(value == 2500.0f);

		value = 5.523f;
		value = uaudio::utils::SecondsToMilliseconds(value);

		CHECK(value == 5523.0f);

		float random = fmodf(rand(), 5.0f);
		value = random;
		value = uaudio::utils::SecondsToMilliseconds(value);

		CHECK(value == random * 1000);

		uaudio::logger::log_success("%s[SECONDS TO MILLISECONDS]%s\n", uaudio::logger::COLOR_CYAN, uaudio::logger::COLOR_WHITE);
	}
	SUBCASE("MILLISECONDS TO SECONDS")
	{
		uaudio::logger::log_info("%s[MILLISECONDS TO SECONDS]%s", uaudio::logger::COLOR_CYAN, uaudio::logger::COLOR_WHITE);

		float value = 1000.0f;
		value = uaudio::utils::MillisecondsToSeconds(value);

		CHECK(value == 1.0f);

		value = 2000.0f;
		value = uaudio::utils::MillisecondsToSeconds(value);

		CHECK(value == 2.0f);

		value = 2500.0f;
		value = uaudio::utils::MillisecondsToSeconds(value);

		CHECK(value == 2.5f);

		value = 5523.0f;
		value = uaudio::utils::MillisecondsToSeconds(value);

		CHECK(value == 5.523f);

		float random = fmodf(rand(), 5.0f) * 1000;
		value = random;
		value = uaudio::utils::MillisecondsToSeconds(value);

		CHECK(value == random / 1000);

		uaudio::logger::log_success("%s[MILLISECONDS TO SECONDS]%s\n", uaudio::logger::COLOR_CYAN, uaudio::logger::COLOR_WHITE);
	}
	SUBCASE("SECONDS TO HOURS")
	{
		uaudio::logger::log_info("%s[SECONDS TO HOURS]%s", uaudio::logger::COLOR_CYAN, uaudio::logger::COLOR_WHITE);

		float value = 3600.0f;
		uint32_t hours = uaudio::utils::SecondsToHours(value);

		CHECK(hours == 1);

		value = 1800.0f;
		hours = uaudio::utils::SecondsToHours(value);

		CHECK(hours == 0);

		value = 2700.0f;
		hours = uaudio::utils::SecondsToHours(value);

		CHECK(hours == 0);

		uaudio::logger::log_success("%s[SECONDS TO HOURS]%s\n", uaudio::logger::COLOR_CYAN, uaudio::logger::COLOR_WHITE);
	}
	SUBCASE("SECONDS TO MINUTES")
	{
		uaudio::logger::log_info("%s[SECONDS TO MINUTES]%s", uaudio::logger::COLOR_CYAN, uaudio::logger::COLOR_WHITE);

		float value = 60.0f;
		uint32_t minutes = uaudio::utils::SecondsToMinutes(value);

		CHECK(minutes == 1);

		value = 120.0f;
		minutes = uaudio::utils::SecondsToMinutes(value);

		CHECK(minutes == 2);

		uaudio::logger::log_success("%s[SECONDS TO MINUTES]%s\n", uaudio::logger::COLOR_CYAN, uaudio::logger::COLOR_WHITE);
	}
}

TEST_CASE("Audio Loading")
{
	SUBCASE("Existing file")
	{
		uaudio::logger::log_info("%s[OPENING EXISTING VALID FILE]%s", uaudio::logger::COLOR_CYAN, uaudio::logger::COLOR_WHITE);

		uaudio::WaveFormat format;
		uaudio::WaveConfig config;
		config.chunksToLoad.push_back(uaudio::FMT_CHUNK_ID);
		config.chunksToLoad.push_back(uaudio::DATA_CHUNK_ID);
		uaudio::WaveReader::LoadSound("3 Steps Ahead - Drop It (Happy Mix).wav", format, config);

		bool hasChunk = false;
		format.HasChunk(hasChunk, uaudio::FMT_CHUNK_ID);
		CHECK(hasChunk);

		uaudio::FMT_Chunk fmt_chunk;
		format.GetChunkFromData<uaudio::FMT_Chunk>(fmt_chunk, uaudio::FMT_CHUNK_ID);

		uint32_t size = 0;
		format.GetChunkSize(size, uaudio::FMT_CHUNK_ID);
		CHECK(size == 16);
		CHECK(fmt_chunk.audioFormat == uaudio::WAV_FORMAT_PCM);
		CHECK(fmt_chunk.numChannels == uaudio::WAVE_CHANNELS_STEREO);
		CHECK(fmt_chunk.sampleRate == uaudio::WAVE_SAMPLE_RATE_44100);
		CHECK(fmt_chunk.byteRate == 176400);
		CHECK(fmt_chunk.blockAlign == uaudio::BLOCK_ALIGN_16_BIT_STEREO);
		CHECK(fmt_chunk.bitsPerSample == uaudio::WAVE_BITS_PER_SAMPLE_16);

		format.HasChunk(hasChunk, uaudio::DATA_CHUNK_ID);
		CHECK(hasChunk);

		format.GetChunkSize(size, uaudio::DATA_CHUNK_ID);
		CHECK(size == 39962928);

		uaudio::logger::log_success("%s[OPENING EXISTING VALID FILE]%s\n", uaudio::logger::COLOR_CYAN, uaudio::logger::COLOR_WHITE);
	}
	SUBCASE("Existing file with custom chunk")
	{
		uaudio::logger::log_info("%s[OPENING EXISTING VALID FILE WITH CUSTOM CHUNK]%s", uaudio::logger::COLOR_CYAN, uaudio::logger::COLOR_WHITE);

		uaudio::WaveFormat format;
		uaudio::WaveConfig config;
		config.chunksToLoad.push_back("gudf");
		uaudio::WaveReader::LoadSound("gudf.wav", format, config);

		bool hasChunk = false;
		format.HasChunk(hasChunk, "gudf");
		CHECK(hasChunk);

		uaudio::logger::log_success("%s[OPENING EXISTING VALID FILE WITH CUSTOM CHUNK]%s\n", uaudio::logger::COLOR_CYAN, uaudio::logger::COLOR_WHITE);
	}
	SUBCASE("Existing invalid file")
	{
		uaudio::logger::log_info("%s[OPENING EXISTING INVALID FILE]%s", uaudio::logger::COLOR_CYAN, uaudio::logger::COLOR_WHITE);

		uaudio::WaveFormat format;
		FILE *file = nullptr;
		uaudio::WaveReader::LoadSound("Broken.wav", format);

		uaudio::logger::log_success("%s[OPENING EXISTING INVALID FILE]%s\n", uaudio::logger::COLOR_CYAN, uaudio::logger::COLOR_WHITE);
	}
	SUBCASE("Existing invalid file 2")
	{
		uaudio::logger::log_info("%s[OPENING EXISTING INVALID FILE 2]%s", uaudio::logger::COLOR_CYAN, uaudio::logger::COLOR_WHITE);

		uaudio::WaveFormat format;
		FILE *file = nullptr;
		uaudio::WaveReader::LoadSound("Confused File.wav", format);

		uaudio::logger::log_success("%s[OPENING EXISTING INVALID FILE 2]%s\n", uaudio::logger::COLOR_CYAN, uaudio::logger::COLOR_WHITE);
	}
	SUBCASE("Non-existing format")
	{
		uaudio::logger::log_info("%s[OPENING NON-EXISTENT FILE]%s", uaudio::logger::COLOR_CYAN, uaudio::logger::COLOR_WHITE);

		uaudio::WaveFormat format;
		FILE *file = nullptr;
		uaudio::WaveReader::LoadSound("test.wav", format);

		uaudio::logger::log_success("%s[OPENING NON-EXISTENT FILE]%s\n", uaudio::logger::COLOR_CYAN, uaudio::logger::COLOR_WHITE);
	}
}