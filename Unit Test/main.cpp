#include <xaudio2_engine/wav/WavConverter.h>
#include <array>
#include <assert.h>
#include <cstdio>
#include <chrono>
#include <thread>

void wait()
{
	std::this_thread::sleep_for(std::chrono::nanoseconds(10));
	std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::seconds(1));
}

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

void MonoToStereo()
{
	print_yellow();
	printf("** MONO TO STEREO **\n");
	print_white();
	wait();
	{
		print_yellow();
		printf("** 16-bit **\n");
		print_white();

		printf("Array before conversion: ");

		std::array<unsigned char, 2> dat = {
			2,
			0,
		};
		unsigned char* data = dat.data();

		uint32_t size = dat.size();
		uint16_t blockalign = 2;

		printf("[");
		for (uint32_t i = 0; i < size; i++)
		{
			printf("%i", data[i]);
			if (i != size - 1)
				printf(", ");
		}
		printf("]\n");
		std::array<unsigned char, dat.size() * 2> EXPECTED = {
			2,
			0,
			2,
			0,
		};

		printf("Size before conversion: %i\n", size);

		data = wav::wav_converter::ConvertMonoToStereo(data, size, blockalign);

		printf("Size after conversion: %i\n", size);

		printf("Array after conversion: ");
		printf("[");
		for (uint32_t i = 0; i < size; i++)
		{
			printf("%i", data[i]);
			if (i != size - 1)
				printf(", ");
			assert(data[i] == EXPECTED[i]);
		}
		printf("]\n");
	}
	wait();
	{
		print_yellow();
		printf("** 24-bit **\n");
		print_white();

		printf("Array before conversion: ");

		std::array<unsigned char, 3> dat = {
			2,
			0,
			5,
		};
		unsigned char* data = dat.data();

		uint32_t size = dat.size();
		uint16_t blockalign = 3;

		printf("[");
		for (uint32_t i = 0; i < size; i++)
		{
			printf("%i", data[i]);
			if (i != size - 1)
				printf(", ");
		}
		printf("]\n");
		std::array<unsigned char, dat.size() * 2> EXPECTED = {
			2,
			0,
			5,
			2,
			0,
			5,
		};

		printf("Size before conversion: %i\n", size);

		data = wav::wav_converter::ConvertMonoToStereo(data, size, blockalign);

		printf("Size after conversion: %i\n", size);

		printf("Array after conversion: ");
		printf("[");
		for (uint32_t i = 0; i < size; i++)
		{
			printf("%i", data[i]);
			if (i != size - 1)
				printf(", ");
			assert(data[i] == EXPECTED[i]);
		}
		printf("]\n");
	}
	wait();
	{
		print_yellow();
		printf("** 32-bit **\n");
		print_white();

		printf("Array before conversion: ");

		std::array<unsigned char, 4> dat = {
			2,
			0,
			5,
			1,
		};
		unsigned char* data = dat.data();

		uint32_t size = dat.size();
		uint16_t blockalign = 4;

		printf("[");
		for (uint32_t i = 0; i < size; i++)
		{
			printf("%i", data[i]);
			if (i != size - 1)
				printf(", ");
		}
		printf("]\n");
		std::array<unsigned char, dat.size() * 2> EXPECTED = {
			2,
			0,
			5,
			1,
			2,
			0,
			5,
			1,
		};

		printf("Size before conversion: %i\n", size);

		data = wav::wav_converter::ConvertMonoToStereo(data, size, blockalign);

		printf("Size after conversion: %i\n", size);

		printf("Array after conversion: ");
		printf("[");
		for (uint32_t i = 0; i < size; i++)
		{
			printf("%i", data[i]);
			if (i != size - 1)
				printf(", ");
			assert(data[i] == EXPECTED[i]);
		}
		printf("]\n");
	}
	wait();
	print_green();
	printf("PASSED V\n");
	print_white();
}

void StereoToMono()
{
	print_yellow();
	printf("** STEREO TO MONO **\n");
	print_white();
	wait();
	{
		print_yellow();
		printf("** 16-bit **\n");
		print_white();

		printf("Array before conversion: ");

		std::array<unsigned char, 4> dat = {
			2,
			0,
			2,
			0,
		};
		unsigned char* data = dat.data();

		uint32_t size = dat.size();
		uint16_t blockalign = 4;

		printf("[");
		for (uint32_t i = 0; i < size; i++)
		{
			printf("%i", data[i]);
			if (i != size - 1)
				printf(", ");
		}
		printf("]\n");
		std::array<unsigned char, dat.size() / 2> EXPECTED = {
			2,
			0,
		};

		printf("Size before conversion: %i\n", size);

		data = wav::wav_converter::ConvertStereoToMono(data, size, blockalign);

		printf("Size after conversion: %i\n", size);

		printf("Array after conversion: ");
		printf("[");
		for (uint32_t i = 0; i < size; i++)
		{
			printf("%i", data[i]);
			if (i != size - 1)
				printf(", ");
			assert(data[i] == EXPECTED[i]);
		}
		printf("]\n");
	}
	wait();
	{
		print_yellow();
		printf("** 24-bit **\n");
		print_white();

		printf("Array before conversion: ");

		std::array<unsigned char, 6> dat = {
			2,
			0,
			6,
			2,
			0,
			6,
		};
		unsigned char* data = dat.data();

		uint32_t size = dat.size();
		uint16_t blockalign = 6;

		printf("[");
		for (uint32_t i = 0; i < size; i++)
		{
			printf("%i", data[i]);
			if (i != size - 1)
				printf(", ");
		}
		printf("]\n");
		std::array<unsigned char, dat.size() / 2> EXPECTED = {
			2,
			0,
			6,
		};

		printf("Size before conversion: %i\n", size);

		data = wav::wav_converter::ConvertStereoToMono(data, size, blockalign);

		printf("Size after conversion: %i\n", size);

		printf("Array after conversion: ");
		printf("[");
		for (uint32_t i = 0; i < size; i++)
		{
			printf("%i", data[i]);
			if (i != size - 1)
				printf(", ");
			assert(data[i] == EXPECTED[i]);
		}
		printf("]\n");
	}
	wait();
	{
		print_yellow();
		printf("** 32-bit **\n");
		print_white();

		printf("Array before conversion: ");

		std::array<unsigned char, 8> dat = {
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
		uint16_t blockalign = 8;

		printf("[");
		for (uint32_t i = 0; i < size; i++)
		{
			printf("%i", data[i]);
			if (i != size - 1)
				printf(", ");
		}
		printf("]\n");
		std::array<unsigned char, dat.size() / 2> EXPECTED = {
			2,
			0,
			6,
			4,
		};

		printf("Size before conversion: %i\n", size);

		data = wav::wav_converter::ConvertStereoToMono(data, size, blockalign);

		printf("Size after conversion: %i\n", size);

		printf("Array after conversion: ");
		printf("[");
		for (uint32_t i = 0; i < size; i++)
		{
			printf("%i", data[i]);
			if (i != size - 1)
				printf(", ");
			assert(data[i] == EXPECTED[i]);
		}
		printf("]\n");
	}
	wait();
	print_green();
	printf("PASSED V\n");
	print_white();
}

int main()
{
	MonoToStereo();
	wait();
	StereoToMono();
	wait();
	printf("\033[0;37m");
}