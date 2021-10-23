#include "cBuffer.h"
#include <vector>
#include <cstdlib>
#include <stdint.h>


cBuffer::cBuffer()
{
	writeIndex = 0;
	readIndex = 0;
}
cBuffer::cBuffer(size_t size)
{
	_buffer.resize(size);
	writeIndex = 0;
	readIndex = 0;
}

cBuffer::cBuffer(std::vector<uint8_t> buffer)
{
	_buffer = buffer;
	writeIndex = 0;
	readIndex = 0;
}

void cBuffer::writeShortBE(std::size_t index, int16_t value)
{
	_buffer[index + 1] = value;
	writeIndex++;
	_buffer[index] = value >> 8;
	writeIndex++;
}
void cBuffer::writeShortBE(int16_t value)
{
	writeShortBE(writeIndex, value);
}
int16_t cBuffer::readShortBE(std::size_t index)
{
	int16_t value = _buffer[index + 1];
	readIndex++;
	value |= _buffer[index] << 8;
	readIndex++;

	return value;
}
int16_t cBuffer::readShortBE()
{
	return readShortBE(readIndex);
}

void cBuffer::writeIntBE(std::size_t index, int32_t value)
{
	//if (writeIndex > readIndex)
	//{
	_buffer[index + 3] = value;
	writeIndex++;
	_buffer[index + 2] = value >> 8;
	writeIndex++;
	_buffer[index + 1] = value >> 16;
	writeIndex++;
	_buffer[index] = value >> 24;
	writeIndex++;
	//}


}
void cBuffer::writeIntBE(int32_t value)
{
	writeIntBE(writeIndex, value);
}
uint32_t cBuffer::readIntBE(std::size_t index)
{
	uint32_t value = _buffer[index + 3];
	readIndex++;
	value |= _buffer[index + 2] << 8;
	readIndex++;
	value |= _buffer[index + 1] << 16;
	readIndex++;
	value |= _buffer[index] << 24;
	readIndex++;

	return value;
}
uint32_t cBuffer::readIntBE()
{
	return readIntBE(readIndex);
}

void cBuffer::writeString(std::size_t index, std::vector<uint8_t> value)
{
	for (int i = 0; i < value.size(); i++)
	{
		_buffer[writeIndex] = value[i];
		writeIndex++;
	}
}
void cBuffer::writeString(std::vector<uint8_t> value)
{
	writeString(writeIndex, value);
}
std::vector<uint8_t> cBuffer::readString(std::size_t index)
{
	std::vector<uint8_t> value;

	while (_buffer.at(readIndex) != 0)
	{
		value.push_back(_buffer[readIndex]);
		readIndex++;
	}
	return value;
}
std::vector<uint8_t> cBuffer::readString()
{
	return readString(readIndex);
}

