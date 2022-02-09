#pragma once

#include <cstdint>

namespace effects
{
	unsigned char* change_volume(unsigned char* data, uint32_t size, float volume);
	unsigned char* change_panning(unsigned char* data, uint32_t size, float amount);
}