#include "bin_data_interpreter.h"
#include "bin_data_interpreter.h"
#include <custom_exceptions.hpp>
#include <stl_utils.h>


#pragma region Pusher methods:
void BinaryDataInterpreter::PushInt32(std::vector<uint8_t>& data, const int32_t number)
{
	//An int32 has 4 byte. Get it
	const uint8_t byte1 = (number >> 24) & 0xFF;
	const uint8_t byte2 = (number >> 16) & 0xFF;
	const uint8_t byte3 = (number >> 8) & 0xFF;
	const uint8_t byte4 = (number) & 0xFF;

	data.push_back(byte1);
	data.push_back(byte2);
	data.push_back(byte3);
	data.push_back(byte4);
}

void BinaryDataInterpreter::PushUInt8(std::vector<uint8_t>& data, const uint8_t number)
{
	data.push_back(number);
}

void BinaryDataInterpreter::PushUInt16(std::vector<uint8_t>& data, const uint16_t number)
{
	//An uint16 has 2 byte. Get it
	const uint8_t byte1 = (number >> 8) & 0xFF;
	const uint8_t byte2 = (number) & 0xFF;

	data.push_back(byte1);
	data.push_back(byte2);
}

void BinaryDataInterpreter::PushUInt32(std::vector<uint8_t>& data, const uint32_t number)
{
	//An uint32 has 4 byte. Get it
	const uint8_t byte1 = (number >> 24) & 0xFF;
	const uint8_t byte2 = (number >> 16) & 0xFF;
	const uint8_t byte3 = (number >> 8) & 0xFF;
	const uint8_t byte4 = (number) & 0xFF;

	data.push_back(byte1);
	data.push_back(byte2);
	data.push_back(byte3);
	data.push_back(byte4);
}

void BinaryDataInterpreter::PushString(std::vector<uint8_t>& data, const std::string& str)
{
	BinaryDataInterpreter::PushUInt32(data, static_cast<uint32_t>(str.size()));
	data.insert(data.end(), str.begin(), str.end());
}

void BinaryDataInterpreter::PushBoolean(std::vector<uint8_t>& data, const bool boolean)
{
	BinaryDataInterpreter::PushUInt8(data, boolean);
}

void BinaryDataInterpreter::PushFloat(std::vector<uint8_t>& data, const float& val_float)
{
	const unsigned char* ptr = reinterpret_cast<const unsigned char*>(&val_float);
	for (size_t i = 0; i < sizeof(float); ++i)
		data.push_back(ptr[i]);
}

void BinaryDataInterpreter::PushDouble(std::vector<uint8_t>& data, const double& val_double)
{
	const unsigned char* ptr = reinterpret_cast<const unsigned char*>(&val_double);
	for (size_t i = 0; i < sizeof(double); ++i)
		data.push_back(ptr[i]);
}

void BinaryDataInterpreter::PushIVec2(std::vector<uint8_t>& data, const glm::ivec2& val_ivec2)
{
	BinaryDataInterpreter::PushInt32(data, val_ivec2.x);
	BinaryDataInterpreter::PushInt32(data, val_ivec2.y);
}

void BinaryDataInterpreter::PushUInt8List(std::vector<uint8_t>& data, const std::list<uint8_t>& uint8List)
{
	BinaryDataInterpreter::PushUInt32(data, static_cast<uint32_t>(uint8List.size()));
	for (auto const& item : uint8List)
		BinaryDataInterpreter::PushUInt8(data, item);
}

void BinaryDataInterpreter::PushUInt32List(std::vector<uint8_t>& data, const std::list<uint32_t>& uint32List)
{
	BinaryDataInterpreter::PushUInt32(data, static_cast<uint32_t>(uint32List.size()));
	for (auto const& item : uint32List)
		BinaryDataInterpreter::PushUInt32(data, item);
}

void BinaryDataInterpreter::PushIVec2List(std::vector<uint8_t>& data, const std::list<glm::ivec2>& ivec2List)
{
	BinaryDataInterpreter::PushUInt32(data, static_cast<uint32_t>(ivec2List.size()));
	for (auto const& item : ivec2List)
		BinaryDataInterpreter::PushIVec2(data, item);
}
#pragma endregion

#pragma region Extractor methods:
int32_t BinaryDataInterpreter::ExtractInt32(const std::vector<uint8_t>& data, uint32_t& nOffset)
{
	//An int32 has 4 byte. Compose it (big endian)
	const int byte1 = (data.at(0 + nOffset) << 24);
	const int byte2 = ((data.at(static_cast<uint64_t>(1) + nOffset) & 0xFF) << 16);
	const int byte3 = ((data.at(static_cast<uint64_t>(2) + nOffset) & 0xFF) << 8);
	const int byte4 = (data.at(static_cast<uint64_t>(3) + nOffset) & 0xFF);
	const int32_t integer = byte1 | byte2 | byte3 | byte4;
	nOffset += 4;
	return integer;
}

uint8_t BinaryDataInterpreter::ExtractUInt8(const std::vector<uint8_t>& data, uint32_t& nOffset)
{
	return data.at(0 + nOffset++);
}

uint16_t BinaryDataInterpreter::ExtractUInt16(const std::vector<uint8_t>& data, uint32_t& nOffset)
{
	//An uint32 has 2 byte. Compose it (big endian)
	const int byte1 = ((data.at(0 + nOffset) & 0xFF) << 8);
	const int byte2 = (data.at(static_cast<uint64_t>(1) + nOffset) & 0xFF);
	const uint16_t integer = byte1 | byte2;
	nOffset += 2;
	return integer;
}

uint32_t BinaryDataInterpreter::ExtractUInt32(const std::vector<uint8_t>& data, uint32_t& nOffset)
{
	//An uint32 has 4 byte. Compose it (big endian)
	const int byte1 = (data.at(0 + nOffset) << 24);
	const int byte2 = ((data.at(static_cast<uint64_t>(1) + nOffset) & 0xFF) << 16);
	const int byte3 = ((data.at(static_cast<uint64_t>(2) + nOffset) & 0xFF) << 8);
	const int byte4 = (data.at(static_cast<uint64_t>(3) + nOffset) & 0xFF);
	const uint32_t integer = byte1 | byte2 | byte3 | byte4;
	nOffset += 4;
	return integer;
}

std::string BinaryDataInterpreter::ExtractString(const std::vector<uint8_t>& data, uint32_t& nOffset)
{
	const uint32_t strLen = BinaryDataInterpreter::ExtractUInt32(data, nOffset);
	const std::string str{ data.begin() + nOffset, data.begin() + nOffset + strLen };
	nOffset += strLen;
	return str;
}

bool BinaryDataInterpreter::ExtractBoolean(const std::vector<uint8_t>& data, uint32_t& nOffset)
{
	const uint8_t result = BinaryDataInterpreter::ExtractUInt8(data, nOffset);
	if (result > 1)
		throw BinaryDeserializerException("Parsed value isn't a bool! Something gone wrong.");
	return static_cast<bool>(result);
}

float BinaryDataInterpreter::ExtractFloat(const std::vector<uint8_t>& data, uint32_t& nOffset)
{
	float f = 0.0f;
	memcpy(&f, &(data.at(nOffset)), sizeof(float));
	nOffset += sizeof(float);
	return f;
}

double BinaryDataInterpreter::ExtractDouble(const std::vector<uint8_t>& data, uint32_t& nOffset)
{
	double d = 0.0;
	memcpy(&d, &(data.at(nOffset)), sizeof(double));
	nOffset += sizeof(double);
	return d;
}

glm::ivec2 BinaryDataInterpreter::ExtractIVec2(const std::vector<uint8_t>& data, uint32_t& nOffset)
{
	const int x = BinaryDataInterpreter::ExtractInt32(data, nOffset);
	const int y = BinaryDataInterpreter::ExtractInt32(data, nOffset);
	return glm::ivec2(x, y);
}

std::list<uint8_t> BinaryDataInterpreter::ExtractUint8List(const std::vector<uint8_t>& data, uint32_t& nOffset)
{
	std::list<uint8_t> extractedList;
	const uint32_t listSize = BinaryDataInterpreter::ExtractUInt32(data, nOffset);
	for (uint32_t i = 0; i < listSize; i++)
		extractedList.push_back(BinaryDataInterpreter::ExtractUInt8(data, nOffset));
	return extractedList;
}

std::list<uint32_t> BinaryDataInterpreter::ExtractUint32List(const std::vector<uint8_t>& data, uint32_t& nOffset)
{
	std::list<uint32_t> extractedList;
	const uint32_t listSize = BinaryDataInterpreter::ExtractUInt32(data, nOffset);
	for (uint32_t i = 0; i < listSize; i++)
		extractedList.push_back(BinaryDataInterpreter::ExtractUInt32(data, nOffset));
	return extractedList;
}

std::list<glm::ivec2> BinaryDataInterpreter::ExtractIVec2List(const std::vector<uint8_t>& data, uint32_t& nOffset)
{
	std::list<glm::ivec2> extractedList;
	const uint32_t listSize = BinaryDataInterpreter::ExtractUInt32(data, nOffset);
	for (uint32_t i = 0; i < listSize; i++)
		extractedList.push_back(BinaryDataInterpreter::ExtractIVec2(data, nOffset));
	return extractedList;

}
#pragma endregion

BinaryDataInterpreter::BinaryDataInterpreter(void)
{
}

BinaryDataInterpreter::~BinaryDataInterpreter(void)
{
}
