#pragma once

#include <vector>
#include <cstdlib>
#include <stdint.h>

class cBuffer
{
public:

	cBuffer();

	std::vector<uint8_t> _buffer;

	cBuffer(size_t size);

	void writeUInt32LE(std::size_t index, int32_t value);

	void writeUInt32LE(int32_t value);

	uint32_t readUInt32LE(std::size_t index);

	uint32_t readUInt32LE();
};


