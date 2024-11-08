/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/
#pragma once

#include <string>

namespace Encode 
{
	void ToLowerCase(std::string& text);

	void ToLowercase(std::string* text);

	[[nodiscard]] std::size_t GetStringSize(const char* src);

	[[nodiscard]] std::wstring GetWideString(const char* src);

	[[nodiscard]] const char* HexCharToBin(char c);

	[[nodiscard]] std::string HexStrToBinStr(const std::string& hex);

	[[nodiscard]] std::string GenerateRandomString(const uint32_t length = 5);

	void NormalizePath(std::string& s);

	[[nodiscard]] std::string FixImageName(std::string image_name);
};
