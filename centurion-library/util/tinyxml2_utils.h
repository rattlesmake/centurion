/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <tinyxml2.h>
#include <string>

using namespace tinyxml2;

namespace tinyxml2 
{
	// This function tries to parse a given string child.
	// It returns an empty string if the process failed.
	std::string TryParseFirstChildStrContent(XMLElement* e, const char* childTag);

	// This function tries to parse a given integer child.
	// It returns zero if the process failed.
	int TryParseFirstChildIntContent(XMLElement* e, const char* childTag, const int defaultValue = 0);

	std::string TryParseStrAttribute(XMLElement* e, const char* attribute);

	int TryParseIntAttribute(XMLElement* e, const char* attribute, const int defaultValue = 0);
	
	float TryParseFloatAttribute(XMLElement* e, const char* attribute, const float defaultValue = 0.f);

	bool TryParseBoolAttribute(XMLElement* e, const char* attribute, const bool defaultValue = false);

	int TryParseDynamicIntAttribute(XMLElement* e, const char* attribute);

	float TryParseDynamicFloatAttribute(XMLElement* e, const char* attribute);
};

