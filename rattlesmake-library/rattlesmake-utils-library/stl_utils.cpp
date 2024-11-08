#include "stl_utils.h"
#include "encode_utils.h"

#include <iostream>
#include <sstream>
#include <algorithm>

std::vector<uint8_t> std::stringToVector(const std::string& str)
{
	return std::vector<uint8_t>(str.begin(), str.end());
}

std::string std::vectorToString(const std::vector<uint8_t>& vec)
{
	return std::string(vec.begin(), vec.end());
}

void std::ltrim(std::string& s)
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
		return !std::isspace(ch);
	}));
}
void std::rtrim(std::string& s)
{
	s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
		return !std::isspace(ch);
	}).base(), s.end());
}
void std::trim(std::string& s)
{
	ltrim(s);
	rtrim(s);
}
std::string std::ltrim_copy(std::string s)
{
	ltrim(s);
	return s;
}
std::string std::rtrim_copy(std::string s)
{
	rtrim(s);
	return s;
}
std::string std::trim_copy(std::string s)
{
	trim(s);
	return s;
}
std::string std::uniform_string(std::string s)
{
	std::replace(s.begin(), s.end(), '\\', '/');
	while (s.find("//") != string::npos)
	{
		s.replace(s.find("//"), 2, "/");
	}
	return s;
}
std::string std::remove_extension(const std::string& filename)
{
	if (filename.empty() == true)
		return "";
	size_t lastdot = filename.find_last_of(".");
	if (lastdot == std::string::npos)
		return filename;
	return filename.substr(0, lastdot);
}
bool std::is_number(std::string str)
{
	const size_t length = (uint32_t)str.length();
	for (uint32_t ch = 0; ch < length; ch++)
	{
		if (std::isdigit(str[ch]) == false)
			return false;
	}
	return true;
}
bool std::split_string(const std::string& str, std::string& firstHalf, std::string& secondHalf, const char delimiter)
{
	std::string::size_type pos = str.find(delimiter);
	if (str.npos != pos)
	{
		firstHalf = str.substr(0, pos);
		secondHalf = str.substr(pos + 1);
		return true;
	}
	return false;
}
std::vector<std::string> std::split_string(std::string input, char del)
{
	std::vector<std::string> output;
	std::istringstream f(input);

	std::string s;
	while (std::getline(f, s, del)) {
		s = std::trim_copy(s);
		if (s.empty()) continue;
		output.push_back(s);
	}
	return output;
}
