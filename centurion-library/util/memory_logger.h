/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <string>
#include <vector>

#ifndef MAX_LOG_FILES
#define MAX_LOG_FILES 8
#endif // !MAX_LOG_FILES


namespace MemoryLogger
{
	typedef std::vector<std::pair<const std::string, const std::string>> memoryLoggerParams;
	void AddMemoryConstructionLog(const std::string& _className, const unsigned int* const _address, const std::string& _creator, const memoryLoggerParams& _otherParams = {});
	void AddMemoryDestructionLog(const std::string& _className, const unsigned int* const _address, const memoryLoggerParams& _otherParams = {});
};
