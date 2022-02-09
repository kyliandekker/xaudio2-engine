#include <iostream>

#include <Windows.h>

#include "Audio/WavConverter.h"

#include "Audio/Effects.h"

unsigned char* effects::change_volume(unsigned char* data, uint32_t size, float volume)
{
	for (uint32_t i = 0; i < size; i += 2)
	{
		short wData;
		wData = MAKEWORD(data[i], data[i + 1]);
		long dwData = wData;
		dwData = dwData * volume;
		if (dwData < -0x8000)
		{
			dwData = -0x8000;
		}
		else if (dwData > 0x7FFF)
		{
			dwData = 0x7FFF;
		}
		wData = LOWORD(dwData);
		data[i] = LOBYTE(wData);
		data[i + 1] = HIBYTE(wData);
	}

	return data;
}