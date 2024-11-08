/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <fstream>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <condition_variable>
#include <stdexcept>
#include <optional>

class BinaryFileReader 
{
public:
	BinaryFileReader(void);
	~BinaryFileReader(void);
	BinaryFileReader(const BinaryFileReader& other) = delete;
	BinaryFileReader& operator=(BinaryFileReader const& other) = delete;

	void SetFile(std::string filePath);

	void ReadNumber(void);
	void ReadString(void);
	void ReadBinaryData(const uint32_t nToRead = 1);

	[[nodiscard]] size_t GetNumber(void);
	[[nodiscard]] std::string GetString(void);
	[[nodiscard]] std::vector<uint8_t> GetBinData(void);

	void WaitForEmptyQueues(void);
	void CloseFile(void);
private:
	enum class ReadTask
	{
		E_READ_NUMBER,
		E_READ_STRING,
		E_READ_BIN_DATA,
	};

	std::list<ReadTask> tasksQueue;

	bool bExceptionOccurs = false;

	std::ifstream targetFile;

	struct Queue
	{
		std::list<size_t> numbers;
		std::list<std::string> strs;
		std::list<std::vector<uint8_t>> binData;

		[[nodiscard]] bool IsEmpty(void) const;
		void Clear(void);
	}	queue;
	

	bool bStop = false;

	//Multithreading
	std::thread bfrThread;
	std::mutex bfrMutex;
	std::condition_variable bfrCV;

	void bfrThreadRun(void);
	void Stop(void);

	[[nodiscard]] size_t ReadUInt(void);
	[[nodiscard]] std::vector<uint8_t> ReadBinData(void);
	[[nodiscard]] std::string ReadStr(void);
};
