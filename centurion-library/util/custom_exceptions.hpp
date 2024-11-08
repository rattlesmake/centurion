/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <exception>
#include <string>
#include <vector>

class CenturionException : public std::exception
{
public:
	CenturionException* AsCenturionException() { return (CenturionException*)this; }
	explicit CenturionException(std::string msg = "")
	{
		this->Message = msg;
		this->ExceptionType = __func__;
	}
	CenturionException(std::exception e, const char* functionSignature, const char* exceptionType) {
		this->Message = e.what();
		this->stack.push_back(std::string(functionSignature));
		this->ExceptionType = exceptionType;
	}
	CenturionException(std::string msg, const char* functionSignature, const char* exceptionType)
	{
		this->Message = msg;
		this->stack.push_back(std::string(functionSignature));
		this->ExceptionType = exceptionType;
	}
	void AddFuncSignature(const char* functionSignature)
	{
		this->stack.push_back(std::string(functionSignature));
	}
	std::string GetStack(int nTabs = 0)
	{
		std::string out = "";
		std::string tabs = std::string((std::size_t)nTabs, '\t');
		for (auto const& s : stack)
		{
			out += tabs + s;
			if (s != stack[stack.size() - 1]) out += "\n";
		}
		return out;
	}
	const char* what() const noexcept override { return Message.c_str(); }
	std::string Message;
	std::string ExceptionType;
protected:
	std::vector<std::string> stack;
};

class EngineException : public CenturionException
{
public:
	explicit EngineException(std::string msg = "") : CenturionException(msg) { }
	EngineException(std::exception e, const char* functionSignature) : CenturionException(e, functionSignature, "EngineException") { }
	EngineException(std::string msg, const char* functionSignature) : CenturionException(msg, functionSignature, "EngineException") { }
};

class SecurityLevelException : public CenturionException
{
public:
	explicit SecurityLevelException(std::string msg = "") : CenturionException(msg) { }
	SecurityLevelException(std::exception e, const char* functionSignature) : CenturionException(e, functionSignature, "PythonException") { }
	SecurityLevelException(std::string msg, const char* functionSignature) : CenturionException(msg, functionSignature, "PythonException") { }
};

class BinaryDeserializerException : public std::exception
{
public:
	explicit BinaryDeserializerException(std::string _whatWasBeingRead) : whatWasBeingRead(std::move(_whatWasBeingRead)) { }
	[[nodiscard]] std::string GetWhatWasBeingRead(void) const { return this->whatWasBeingRead;  }
private:
	std::string whatWasBeingRead;
};

class BinarySerializerException : public std::exception
{
public:
	explicit BinarySerializerException(std::string _whatWasBeingSaved) : whatWasBeingSaved(std::move(_whatWasBeingSaved)) { }
	[[nodiscard]] std::string GetWhatWasBeingSaved(void) const { return this->whatWasBeingSaved;  }
private:
	std::string whatWasBeingSaved;
};
