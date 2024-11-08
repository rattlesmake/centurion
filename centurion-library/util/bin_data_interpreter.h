/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <list>
#include <string>
#include <vector>
#include <glm.hpp>

class BinaryDataInterpreter
{
public:
	~BinaryDataInterpreter(void);
	BinaryDataInterpreter(const BinaryDataInterpreter& other) = delete;
	BinaryDataInterpreter& operator=(const BinaryDataInterpreter& other) = delete;

	static void PushInt32(std::vector<uint8_t>& data, const int32_t number);
	static void PushUInt8(std::vector<uint8_t>& data, const uint8_t number);
	static void PushUInt16(std::vector<uint8_t>& data, const uint16_t number);
	static void PushUInt32(std::vector<uint8_t>& data, const uint32_t number);
	static void PushString(std::vector<uint8_t>& data, const std::string& str);
	static void PushBoolean(std::vector<uint8_t>& data, const bool boolean);
	static void PushFloat(std::vector<uint8_t>& data, const float& val_float);
	static void PushDouble(std::vector<uint8_t>& data, const double& val_double);
	static void PushIVec2(std::vector<uint8_t>& data, const glm::ivec2& val_ivec2);
	static void PushUInt8List(std::vector<uint8_t>& data, const std::list<uint8_t>& uint8List);
	static void PushUInt32List(std::vector<uint8_t>& data, const std::list<uint32_t>& uint32List);
	static void PushIVec2List(std::vector<uint8_t>& data, const std::list<glm::ivec2>& ivec2List);

	[[nodiscard]] static int32_t ExtractInt32(const std::vector<uint8_t>& data, uint32_t& nOffset);
	[[nodiscard]] static uint8_t ExtractUInt8(const std::vector<uint8_t>& data, uint32_t& nOffset);
	[[nodiscard]] static uint16_t ExtractUInt16(const std::vector<uint8_t>& data, uint32_t& nOffset);
	[[nodiscard]] static uint32_t ExtractUInt32(const std::vector<uint8_t>& data, uint32_t& nOffset);
	[[nodiscard]] static std::string ExtractString(const std::vector<uint8_t>& data, uint32_t& nOffset);
	[[nodiscard]] static bool ExtractBoolean(const std::vector<uint8_t>& data, uint32_t& nOffset);
	[[nodiscard]] static float ExtractFloat(const std::vector<uint8_t>& data, uint32_t& nOffset);
	[[nodiscard]] static double ExtractDouble(const std::vector<uint8_t>& data, uint32_t& nOffset);
	[[nodiscard]] static glm::ivec2 ExtractIVec2(const std::vector<uint8_t>& data, uint32_t& nOffset);
	[[nodiscard]] static std::list<uint8_t> ExtractUint8List(const std::vector<uint8_t>& data, uint32_t& nOffset);
	[[nodiscard]] static std::list<uint32_t> ExtractUint32List(const std::vector<uint8_t>& data, uint32_t& nOffset);
	[[nodiscard]] static std::list<glm::ivec2> ExtractIVec2List(const std::vector<uint8_t>& data, uint32_t& nOffset);
private:
	BinaryDataInterpreter(void);
};
