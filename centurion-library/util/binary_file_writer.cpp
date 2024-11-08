#include <binary_file_writer.h>
#include <png.h>
#include <stl_utils.h>

#pragma region Constructor and destructor:
BinaryFileWriter::BinaryFileWriter(void)
{
	std::thread t{ [this]() {
		this->bfwThreadRun();
	} };
	this->bfwThread = std::move(t);
}

BinaryFileWriter::~BinaryFileWriter(void)
{
	this->Stop();
	this->CloseFile();
	if (this->bfwThread.joinable() == true)
		this->bfwThread.join();
}
#pragma endregion


#pragma region Public members:
void BinaryFileWriter::SetFile(std::string filePath)
{
	std::unique_lock<std::mutex> ul{ this->bfwMutex };

	this->bfwCV.wait(ul, [this]() {
		return (this->queue.empty() == true || this->bExceptionOccurs == true);
	});

	if (this->bExceptionOccurs == true)
	{
		this->bExceptionOccurs = false;
		throw std::exception("errror TODO");
	}

	if (this->targetFile.is_open() == true)
	{
		this->targetFile.clear();
		this->targetFile.close();
	}

	this->targetFile.open(filePath, std::ios::out | std::ios::binary);
	if (this->targetFile.is_open() == false)
		throw std::runtime_error("Cannot open");
}

void BinaryFileWriter::PushString(const std::string& str)
{
	const std::lock_guard lg{ this->bfwMutex };

	if (this->bExceptionOccurs == true)
	{
		this->bExceptionOccurs = false;
		throw std::exception("errror TODO");
	}

	this->queue.push_back(OutputBinaryFileWriterInfo(std::nullopt, std::stringToVector(str), std::nullopt));
	this->bfwCV.notify_all();
}

void BinaryFileWriter::PushImageData(std::vector<uint8_t>&& imageData, const uint32_t width, const uint32_t height, const uint32_t nChannels)
{
	const std::lock_guard lg{ this->bfwMutex };

	if (this->bExceptionOccurs == true)
	{
		this->bExceptionOccurs = false;
		throw std::exception("errror TODO");
	}
	this->queue.push_back(OutputBinaryFileWriterInfo(std::nullopt, std::move(imageData), ImageInfo(width, height, nChannels)));
	this->bfwCV.notify_all();
}

void BinaryFileWriter::PushNumber(const size_t number)
{
	const std::lock_guard lg{ this->bfwMutex };

	if (this->bExceptionOccurs == true)
	{
		this->bExceptionOccurs = false;
		throw std::exception("errror TODO");
	}

	this->queue.push_back(OutputBinaryFileWriterInfo(number, std::vector<uint8_t>(), std::nullopt));
	this->bfwCV.notify_all();
}

void BinaryFileWriter::PushBinaryData(std::vector<uint8_t>&& binData)
{
	const std::lock_guard lg{ this->bfwMutex };

	if (binData.empty() == true)
		return;

	if (this->bExceptionOccurs == true)
	{
		this->bExceptionOccurs = false;
		throw std::exception("errror TODO");
	}

	this->queue.push_back(OutputBinaryFileWriterInfo(std::nullopt, std::move(binData), std::nullopt));
	this->bfwCV.notify_all();
}

void BinaryFileWriter::WaitForEmptyQueue(void)
{
	std::unique_lock<std::mutex> ul{ this->bfwMutex };

	this->bfwCV.wait(ul, [this]() {
		return (this->queue.empty() == true || this->bExceptionOccurs == true);
	});

	if (this->bExceptionOccurs == true)
	{
		this->bExceptionOccurs = false;
		throw std::exception("errror TODO");
	}
}

void BinaryFileWriter::CloseFile(void)
{
	std::unique_lock<std::mutex> ul{ this->bfwMutex };

	this->bfwCV.wait(ul, [this]() {
		return (this->queue.empty() == true || this->bExceptionOccurs == true);
	});

	if (this->bExceptionOccurs == true)
	{
		this->bExceptionOccurs = false;
		throw std::exception("errror TODO");
	}

	if (this->targetFile.is_open() == true)
	{
		this->targetFile.clear();
		this->targetFile.close();
	}
}
#pragma endregion


#pragma region Private members:
void BinaryFileWriter::bfwThreadRun(void)
{
	while (true)
	{
		std::unique_lock<std::mutex> ul{ this->bfwMutex };
		this->bfwCV.wait(ul, [this]() {
			return (this->bStop == true || this->queue.empty() == false);
		});

		if (this->queue.empty() == false)
		{
			auto taskInfo = this->queue.front();
			ul.unlock();
			try
			{
				if (taskInfo.number.has_value() == true)
					this->WriteUInt(taskInfo.number.value());
				else if (taskInfo.imageInfo.has_value() == true)
					this->WriteStbiImage(std::move(taskInfo.data), taskInfo.imageInfo.value());
				else
					this->WriteBinData(std::move(taskInfo.data));
				ul.lock();
				this->queue.pop_front();
			}
			catch (...)
			{
				ul.lock();
				this->queue.clear();
				this->targetFile.clear();
				this->bExceptionOccurs = true;
				this->bfwCV.notify_all();
			}
		}

		if (this->queue.empty() == true)
			this->bfwCV.notify_all();

		if (this->bStop == true && this->queue.empty() == true)
		{
			break;
		}
	}
}

void BinaryFileWriter::Stop(void)
{
	const std::lock_guard<std::mutex> lg{ this->bfwMutex };
	this->bStop = true;
	this->bfwCV.notify_all();
}

void BinaryFileWriter::WriteUInt(const size_t number)
{
	this->targetFile.write(reinterpret_cast<const char*>(&number), sizeof(number));
	if (this->targetFile.good() == false)
		throw std::ofstream::failure("Cannot writing file");
}

void BinaryFileWriter::WriteBinData(std::vector<uint8_t>&& bytes)
{
	try
	{
		//Write number of bytes
		const size_t size = sizeof(uint8_t) * bytes.size();
		this->WriteUInt(size);
		//Write bytes
		this->targetFile.write(reinterpret_cast<char*>(bytes.data()), sizeof(uint8_t) * bytes.size());
		if (this->targetFile.good() == false)
			throw std::ofstream::failure("Cannot writing file");
	}
	catch (const std::exception& ex)
	{
		throw ex;
	}
	catch (...)
	{
		throw std::exception("BinaryFileWriter::WriteBinData error");
	}
}

void BinaryFileWriter::WriteStbiImage(std::vector<uint8_t>&& imageData, const ImageInfo& imgInfo)
{
	static std::vector<uint8_t> imageBuffer{};
	try
	{
		//Prepare stbi image
		rattlesmake::image::stb::flip_vertically_on_write(1);
		rattlesmake::image::stb::write_png_to_func([](void* context, void* data, int len) {
			//Lambda body
			imageBuffer.resize(len);
			memcpy(imageBuffer.data(), data, len);
			},
			0, imgInfo.width, imgInfo.height, imgInfo.nChannels, imageData.data(), 0
		);
		this->WriteBinData(std::move(imageBuffer));
		rattlesmake::image::stb::flip_vertically_on_write(0);
	}
	catch (const std::exception& ex)
	{
		rattlesmake::image::stb::flip_vertically_on_write(0);
		throw ex;
	}
	catch (...)
	{
		rattlesmake::image::stb::flip_vertically_on_write(0);
		throw;
	}
}
#pragma endregion
