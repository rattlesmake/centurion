#include "tinyxml2_utils.h"
#include <services/pyservice.h>

std::string tinyxml2::TryParseFirstChildStrContent(XMLElement* e, const char* childTag)
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

int tinyxml2::TryParseFirstChildIntContent(XMLElement* e, const char* childTag, const int defaultValue)
{
	int out = defaultValue;
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

std::string tinyxml2::TryParseStrAttribute(XMLElement* e, const char* attribute)
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

float tinyxml2::TryParseFloatAttribute(XMLElement* e, const char* attribute, const float defaultValue)
{
	int out = defaultValue;
	if (e != NULL)
	{
		if (e->Attribute(attribute) != NULL)
		{
			out = e->FloatAttribute(attribute);
		}
	}
	return out;
}

int tinyxml2::TryParseIntAttribute(XMLElement* e, const char* attribute, const int defaultValue)
{
	int out = defaultValue;
	if (e != NULL)
	{
		if (e->Attribute(attribute) != NULL)
		{
			out = e->IntAttribute(attribute);
		}
	}
	return out;
}

bool tinyxml2::TryParseBoolAttribute(XMLElement* e, const char* attribute, const bool defaultValue)
{
	bool out = defaultValue;
	if (e != NULL)
	{
		if (e->Attribute(attribute) != NULL)
		{
			out = e->BoolAttribute(attribute);
		}
	}
	return out;
}

int tinyxml2::TryParseDynamicIntAttribute(XMLElement* e, const char* attribute)
{
	float out = 0;
	if (e != NULL)
	{
		if (e->Attribute(attribute) != NULL)
		{
			std::string s = "__asdfghjkl__ = " + std::string(e->Attribute(attribute));
			PyService::GetInstance().GetInterpreter(PySecurityLevel::Admin).Evaluate(s);
			PyService::GetInstance().GetInterpreter(PySecurityLevel::Admin).Evaluate("__asdfghjkl__", PyInterpreter::PyEvaluationTypes::Float, &out);
		}
	}
	return (int)out;
}
float tinyxml2::TryParseDynamicFloatAttribute(XMLElement* e, const char* attribute)
{
	float out = 0;
	if (e != NULL)
	{
		if (e->Attribute(attribute) != NULL)
		{
			std::string s = "__asdfghjkl__ = " + std::string(e->Attribute(attribute));
			PyService::GetInstance().GetInterpreter(PySecurityLevel::Admin).Evaluate(s);
			PyService::GetInstance().GetInterpreter(PySecurityLevel::Admin).Evaluate("__asdfghjkl__", PyInterpreter::PyEvaluationTypes::Float, &out);
		}
	}
	return out;
}
