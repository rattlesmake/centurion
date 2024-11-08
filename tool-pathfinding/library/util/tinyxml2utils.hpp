/*
* ---------------------------
* CLASS READER AND EDITOR FOR CENTURION
* [2019] - [2020] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <tinyxml2.h>
#include <string>

namespace tinyxml2 {

	// This function tries to parse a given string child.
	// It returns an empty string if the process failed.
	static std::string TryParseFirstChildStrContent(XMLElement* e, const char* childTag)
	{
		std::string out = "";
		if (e != NULL)
		{
			if (e->FirstChildElement(childTag) != NULL)
			{
				if (e->FirstChildElement(childTag)->GetText() != NULL)
				{
					out = e->FirstChildElement(childTag)->GetText();
				}
			}
		}
		return out;
	}

	// This function tries to parse a given integer child.
	// It returns zero if the process failed.
	static int TryParseFirstChildIntContent(XMLElement* e, const char* childTag)
	{
		int out = 0;
		if (e != NULL)
		{
			if (e->FirstChildElement(childTag) != NULL)
			{
				if (e->FirstChildElement(childTag)->GetText() != NULL)
				{
					out = e->FirstChildElement(childTag)->IntText();
				}
			}
		}
		return out;
	}

	static std::string TryParseStrAttribute(XMLElement* e, const char* attribute)
	{
		std::string out = "";
		if (e != NULL)
		{
			if (e->Attribute(attribute) != NULL)
			{
				out = e->Attribute(attribute);
			}
		}
		return out;
	}

	static int TryParseIntAttribute(XMLElement* e, const char* attribute)
	{
		int out = 0;
		if (e != NULL)
		{
			if (e->Attribute(attribute) != NULL)
			{
				out = e->IntAttribute(attribute);
			}
		}
		return out;
	}

	static bool TryParseBoolAttribute(XMLElement* e, const char* attribute)
	{
		bool out = false;
		if (e != NULL)
		{
			if (e->Attribute(attribute) != NULL)
			{
				out = e->BoolAttribute(attribute);
			}
		}
		return out;
	}
};
