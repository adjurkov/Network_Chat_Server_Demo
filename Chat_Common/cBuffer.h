#pragma once

#include <vector>
#include <cstdlib>
#include <stdint.h>

class cBuffer
{
public:
	std::vector<uint8_t> _buffer;
	int writeIndex;
	int readIndex;

	cBuffer();
	cBuffer(size_t size);
	cBuffer(std::vector<uint8_t> buffer);

	void writeIntBE(std::size_t index, int32_t value);
	void writeIntBE(int32_t value);
	uint32_t readIntBE(std::size_t index);
	uint32_t readIntBE();

	void writeShortBE(std::size_t index, int16_t value);
	void writeShortBE(int16_t value);
	int16_t readShortBE(std::size_t index);
	int16_t readShortBE();

	void writeString(std::size_t index, std::vector<uint8_t> value);
	void writeString(std::vector<uint8_t> value);
	std::vector<uint8_t> readString(std::size_t index);
	std::vector<uint8_t> readString();
};





