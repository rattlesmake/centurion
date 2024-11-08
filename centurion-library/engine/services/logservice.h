/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#ifndef MAX_LOGGER_SIZE
#define MAX_LOGGER_SIZE 50
#endif

#include <string>
#include <vector>
#include <custom_exceptions.hpp>

#include <duration_logger.h>

/// <summary>
/// This namespace handles logging system.
/// </summary>
namespace Logger
{
	/// <summary>
	/// This class instances a complete log message of a specified function or method.
	/// </summary>
	class LogMessage
	{
	public:
		LogMessage(void);
		LogMessage(std::string txt, std::string typ, std::string nms, std::string clss, std::string mtd);
		LogMessage(CenturionException* e);
		~LogMessage(void);
		std::string date, type, cpp_stack, text, exception_type;
	};

	/// <summary>
	/// This function creates an instance of LogMessage about game informations.
	/// </summary>
	/// <param name="msg">This parameter can only accept an instance of LogMessage.</param>
	void Info(LogMessage msg);

	/// <summary>
	/// This function creates an instance of LogMessage about game informations, using a std::string.
	/// </summary>
	/// <param name="msg">This parameter sets a LogMessage information message from a std::string.</param>
	void Info(std::string msg);

	/// <summary>
	/// This function creates an instance of LogMessage about game warnings.
	/// </summary>
	/// <param name="msg">This parameter can only accept an instance of LogMessage.</param>
	void Warn(LogMessage msg);

	/// <summary>
	/// This function creates an instance of LogMessage about game warnings, using a std::string.
	/// </summary>
	/// <param name="msg">This parameter sets a LogMessage warning message from a std::string.</param>
	void Warn(std::string msg);

	/// <summary>
	/// This function creates an instance of LogMessage about game errors.
	/// </summary>
	/// <param name="msg">This parameter can only accept an instance of LogMessage.</param>
	void Error(LogMessage msg);

	/// <summary>
	/// This function creates an instance of LogMessage about game errors, using a std::string.
	/// </summary>
	/// <param name="msg">This parameter sets a LogMessage error message from a std::string.</param>
	void Error(std::string msg);

	void Error(CenturionException* ex);

	/// <summary>
	/// This function clear the entire Log file.
	/// </summary>
	void CleanLogs(void);

	/// <summary>
	/// This function saves game parameters into a xml when the application is closed or interrupted.
	/// </summary>
	void SaveParamsXML(void);

	/// <summary>
	/// This function saves last debug information into a xml. It should be used each time a LogMessage is instanced.
	/// </summary>
	void SaveDebugXML(void);

};
