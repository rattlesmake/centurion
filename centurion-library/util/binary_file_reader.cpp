#include <binary_file_reader.h>

#include <stl_utils.h>

#pragma region Constructor and destructor:
BinaryFileReader::BinaryFileReader(void)
{
	std::thread t{ [this]() {
		this->bfrThreadRun();
	} };
	this->bfrThread = std::move(t);
}

BinaryFileReader::~BinaryFileReader(void)
{
	this->Stop();
	this->CloseFile();
	if (this->bfrThread.joinable() == true)
		this->bfrThread.join();
}
#pragma endregion


#pragma region Public members:
void BinaryFileReader::SetFile(std::string filePath)
{
	std::unique_lock<std::mutex> ul{ this->bfrMutex };

	this->bfrCV.wait(ul, [this]() {
		return ((this->tasksQueue.empty() == true && this->queue.IsEmpty() == true) || this->bExceptionOccurs == true);
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

	this->targetFile.open(filePath, std::ios::in | std::ios::binary);
	if (this->targetFile.is_open() == false)
		throw std::runtime_error("Cannot open");
}

void BinaryFileReader::ReadNumber(void)
{
	const std::lock_guard lg{ this->bfrMutex };

	if (this->bExceptionOccurs == true)
	{
		this->bExceptionOccurs = false;
		throw std::exception("errror TODO");
	}

	this->tasksQueue.push_back(ReadTask::E_READ_NUMBER);
	this->bfrCV.notify_all();
}

void BinaryFileReader::ReadString(void)
{
	const std::lock_guard lg{ this->bfrMutex };

	if (this->bExceptionOccurs == true)
	{
		this->bExceptionOccurs = false;
		throw std::exception("errror TODO");
	}

	this->tasksQueue.push_back(ReadTask::E_READ_STRING);
	this->bfrCV.notify_all();
}

void BinaryFileReader::ReadBinaryData(const uint32_t nToRead)
{
	const std::lock_guard lg{ this->bfrMutex };

	if (nToRead == 0)
		return;

	if (this->bExceptionOccurs == true)
	{
		this->bExceptionOccurs = false;
		throw std::exception("errror TODO");
	}

	for (uint32_t n = 0; n < nToRead; n++)
		this->tasksQueue.push_back(ReadTask::E_READ_BIN_DATA);
	this->bfrCV.notify_all();
}

size_t BinaryFileReader::GetNumber(void)
{
	std::unique_lock<std::mutex> ul{ this->bfrMutex };

	this->bfrCV.wait(ul, [this]() {
		return (this->queue.numbers.empty() == false || this->bExceptionOccurs == true);
	});

	if (this->bExceptionOccurs == true)
	{
		this->bExceptionOccurs = false;
		throw std::exception("errror TODO");
	}

	const size_t number = this->queue.numbers.front();
	this->queue.numbers.pop_front();
	return number;
}

std::string BinaryFileReader::GetString(void)
{
	std::unique_lock<std::mutex> ul{ this->bfrMutex };

	this->bfrCV.wait(ul, [this]() {
		return (this->queue.strs.empty() == false || this->bExceptionOccurs == true);
	});

	if (this->bExceptionOccurs == true)
	{
		this->bExceptionOccurs = false;
		throw std::exception("errror TODO");
	}

	const std::string str = std::move(this->queue.strs.front());
	this->queue.strs.pop_front();
	return str;
}

std::vector<uint8_t> BinaryFileReader::GetBinData(void)
{
	std::unique_lock<std::mutex> ul{ this->bfrMutex };

	this->bfrCV.wait(ul, [this]() {
		return (this->queue.binData.empty() == false || this->bExceptionOccurs == true);
	});

	if (this->bExceptionOccurs == true)
	{
		this->bExceptionOccurs = false;
		throw std::exception("errror TODO");
	}

	const std::vector<uint8_t> data = std::move(this->queue.binData.front());
	this->queue.binData.pop_front();
	return data;
}

void BinaryFileReader::WaitForEmptyQueues(void)
{
	std::unique_lock<std::mutex> ul{ this->bfrMutex };

	this->bfrCV.wait(ul, [this]() {
		return ((this->tasksQueue.empty() == true && this->queue.IsEmpty() == true) || this->bExceptionOccurs == true);
	});

	if (this->bExceptionOccurs == true)
	{
		this->bExceptionOccurs = false;
		throw std::exception("errror TODO");
	}
}

void BinaryFileReader::CloseFile(void)
{
	std::unique_lock<std::mutex> ul{ this->bfrMutex };

	this->tasksQueue.clear();
	this->queue.binData.clear();
	this->queue.numbers.clear();
	this->queue.strs.clear();

	this->bExceptionOccurs = false;

	if (this->targetFile.is_open() == true)
	{
		this->targetFile.clear();
		this->targetFile.close();
	}
}
#pragma endregion


#pragma region Private members:
void BinaryFileReader::bfrThreadRun(void)
{
	while (true)
	{
		std::unique_lock<std::mutex> ul{ this->bfrMutex };
		this->bfrCV.wait(ul, [this]() {
			return (this->bStop == true || this->tasksQueue.empty() == false);
		});

		if (this->tasksQueue.empty() == false)
		{
			const auto taskInfo = this->tasksQueue.front();
			ul.unlock();
			try
			{
				std::optional<size_t> number;
				std::optional<std::vector<uint8_t>> binData;
				std::optional<std::string> str;
				if (taskInfo == ReadTask::E_READ_NUMBER)
					number = this->ReadUInt();
				else if (taskInfo == ReadTask::E_READ_STRING)
					str = this->ReadStr();
				else
					binData = this->ReadBinData();
				ul.lock();
				if (this->tasksQueue.empty() == false)
				{
					if (number.has_value() == true)
						this->queue.numbers.push_back(number.value());
					else if (str.has_value() == true)
						this->queue.strs.push_back(std::move(str.value()));
					else
						this->queue.binData.push_back(std::move(binData.value()));
					this->tasksQueue.pop_front();
				}
			}
			catch (...)
			{
				ul.lock();
				this->tasksQueue.clear();
				this->queue.Clear();
				this->targetFile.clear();
				this->bExceptionOccurs = true;
				this->bfrCV.notify_all();
			}
		}

		if (this->tasksQueue.empty() == true)
			this->bfrCV.notify_all();

		if (this->bStop == true && this->tasksQueue.empty() == true && this->queue.IsEmpty() == true)
		{
			break;
		}
	}
}

void BinaryFileReader::Stop(void)
{
	const std::lock_guard<std::mutex> lg{ this->bfrMutex };
	this->bStop = true;
	this->bfrCV.notify_all();
}

size_t BinaryFileReader::ReadUInt(void) 
{
	auto a = targetFile.tellg();
	size_t number = 0;
	this->targetFile.read(reinterpret_cast<char*>(&number), sizeof(number));
	if (this->targetFile.good() == false)
		throw std::ifstream::failure("Cannot read data");
	return number;
}

std::vector<uint8_t> BinaryFileReader::ReadBinData(void)
{
	try
	{
		//Read bytes to read
		const size_t bytesToRead = this->ReadUInt();
		//Read bytes
		std::vector<uint8_t> bytes(bytesToRead, 0);
		this->targetFile.read(reinterpret_cast<char*>(bytes.data()), sizeof(uint8_t) * bytes.size());
		if (this->targetFile.good() == false)
			throw std::ifstream::failure("Cannot read data");
		return bytes;
	}
	catch (...)
	{
		throw std::ifstream::failure("Cannot read data");
	}
}

std::string BinaryFileReader::ReadStr(void)
{
	std::vector<uint8_t> strAsBin = this->ReadBinData();
	return std::vectorToString(strAsBin);
}
#pragma endregion

bool BinaryFileReader::Queue::IsEmpty(void) const
{
	return ((this->numbers.empty() == true) && (this->binData.empty() == true) && (this->strs.empty() == true));
}

void BinaryFileReader::Queue::Clear(void)
{
	this->numbers.clear();
	this->strs.clear();
	this->binData.clear();
}
