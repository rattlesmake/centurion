#include "encode_utils.h"

#include <string>
#include <algorithm>
#include <iostream>
#include <math_utils.h>
#include <stl_utils.h>
#include <string_view>

void Encode::ToLowerCase(std::string& text)
{
	std::transform(text.begin(), text.end(), text.begin(), ::tolower);
}

void Encode::ToLowercase(std::string* text)
{
	std::transform((*text).begin(), (*text).end(), (*text).begin(), ::tolower);
}

std::size_t Encode::GetStringSize(const char* src)
{
	std::string_view s(src);
	return s.size();
}

std::wstring Encode::GetWideString(const char* src)
{

	std::wstring ws = L"";
	std::size_t len = GetStringSize(src);

	for (int i = 0; i < len; i++)
	{
		int u = (int)src[i];
		if (u < 0) u = 256 + u;

		if (u <= 126)
		{
			ws += (wchar_t)u;
		}
		else if (u > 126 && i < len - 1)
		{
			int u2 = (int)src[i + 1];
			if (u2 < 0) u2 = 256 + u2;
			ws += (wchar_t)u2 + 64;
			i++;
		}
	}

	return ws;
}

const char* Encode::HexCharToBin(char c)
{
	// TODO handle default / error
	switch (toupper(c))
	{
	case '0': return "0000";
	case '1': return "0001";
	case '2': return "0010";
	case '3': return "0011";
	case '4': return "0100";
	case '5': return "0101";
	case '6': return "0110";
	case '7': return "0111";
	case '8': return "1000";
	case '9': return "1001";
	case 'A': return "1010";
	case 'B': return "1011";
	case 'C': return "1100";
	case 'D': return "1101";
	case 'E': return "1110";
	case 'F': return "1111";
	}
	return "0000";
}

std::string Encode::HexStrToBinStr(const std::string& hex)
{
	// TODO use a loop from <algorithm> or smth
	std::string bin;
	for (unsigned i = 0; i != hex.length(); ++i)
		bin += HexCharToBin(hex[i]);
	return bin;
}

std::string Encode::GenerateRandomString(const uint32_t length)
{
	std::string str = std::string(length, '_');
	std::string s1 = "0123456789";
	std::string s2 = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	std::string s3 = "abcdefghijklmnopqrstuvwxyz";

	std::array<std::vector<char>, 3> a;
	a[0] = std::vector<char>(s1.begin(), s1.end());
	a[1] = std::vector<char>(s2.begin(), s2.end());
	a[2] = std::vector<char>(s3.begin(), s3.end());

	// first char always alphabetical
	
	auto randomizer = Math::Randomizer<char>();
	str[0] = randomizer.Extract(a[2]);

	for (size_t i = 1; i < length; i++)
	{
		unsigned int idx = Math::runif_discrete(0, 2);
		str[i] = randomizer.Extract(a[idx]);
	}

	return str;
}

void Encode::NormalizePath(std::string& s)
{
	if (s[0] == '\\')
		s = s.substr(1);
	std::replace(s.begin(), s.end(), '\\', '/');
	std::transform(s.begin(), s.end(), s.begin(), ::tolower);
}

std::string Encode::FixImageName(std::string image_name)
{
	if (image_name.empty()) 
		return "";
	Encode::NormalizePath(image_name);
	return std::remove_extension(image_name);
}
