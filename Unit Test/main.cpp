#include <xaudio2_engine/wav/WavConverter.h>
#include <array>
#include <assert.h>
#include <cstdio>

void MonoToStereo()
{
	printf("\033[0;33m");
	printf("** MONO TO STEREO **\n");
	printf("\033[0;37m");

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
	std::array<unsigned char, 4> EXPECTED = {
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
	printf("\033[0;32m");
	printf("PASSED V\n");
}

void StereoToMono()
{
	printf("\033[0;33m");
	printf("** STEREO TO MONO **\n");
	printf("\033[0;37m");

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
	std::array<unsigned char, 2> EXPECTED = {
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
	printf("\033[0;32m");
	printf("PASSED V\n");
}

int main()
{
	MonoToStereo();
	StereoToMono();
	printf("\033[0;37m");
}