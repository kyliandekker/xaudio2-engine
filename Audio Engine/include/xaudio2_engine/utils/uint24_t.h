#pragma once

#include <cstdint>

constexpr int INT24_MAX = (1 << 23) - 1;
constexpr int INT24_MIN = -INT24_MAX - 1;

class uint24_t
{
protected:
	uint8_t byt[3] = {
		0,
		0,
		0
	};
public:
    uint24_t() = default;

	uint24_t(int val)
	{
		operator=(val);
	}

    uint24_t& operator=(int val)
	{
		byt[0] = reinterpret_cast<uint8_t*>(&val)[0];
		byt[1] = reinterpret_cast<uint8_t*>(&val)[1];
		byt[2] = reinterpret_cast<uint8_t*>(&val)[2];
		return *this;
	}

    operator int() const
	{
		return (byt[0]) | (byt[1] << 8) | (byt[2] << 16) | (byt[2] & 0x80);
	}
};