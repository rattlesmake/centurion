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

class BinaryFileWriter 
{
public:
	BinaryFileWriter(void);
	~BinaryFileWriter(void);
	BinaryFileWriter(const BinaryFileWriter& other) = delete;
	BinaryFileWriter& operator=(BinaryFileWriter const& other) = delete;

	void SetFile(std::string filePath);

	void PushString(const std::string& str);
	void PushImageData(std::vector<uint8_t>&& imageData, const uint32_t width, const uint32_t height, const uint32_t nChannels);
	void PushNumber(const size_t number);
	void PushBinaryData(std::vector<uint8_t>&& binData);

	void WaitForEmptyQueue(void);
	void CloseFile(void);
private:
	struct ImageInfo
	{
		uint32_t width = 0;
		uint32_t height = 0;
		uint32_t nChannels = 0;
	};

	struct OutputBinaryFileWriterInfo
	{
		std::optional<size_t> number;
		std::vector<uint8_t> data;
		std::optional<ImageInfo> imageInfo;
	};
	std::list<OutputBinaryFileWriterInfo> queue;

	bool bExceptionOccurs = false;
	std::ofstream targetFile;

	void bfwThreadRun(void);
	void Stop(void);

	void WriteUInt(const size_t number);
	void WriteBinData(std::vector<uint8_t>&& bytes);
	void WriteStbiImage(std::vector<uint8_t>&& imageData, const ImageInfo& imgInfo);

	bool bStop = false;

	//Multithreading
	std::thread bfwThread;
	std::mutex bfwMutex;
	std::condition_variable bfwCV;
};
