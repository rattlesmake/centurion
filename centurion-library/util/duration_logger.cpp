#include <iostream>
#include <iomanip>
#include <duration_logger.h>
#include <fileservice.h>
#include <fstream>
#include <sstream>
#include <unordered_map>

namespace {
	std::deque<double> emptyDeque = std::deque<double>(10, 0.0);
	std::unordered_map<std::string, uint8_t> namesIdMap;
	std::vector<DurationLogger> data;
};

std::string SerializeData()
{
	std::string dFile = "[";
	int i = 0;
	for (auto const& el : data)
	{
		dFile = dFile + "{" + "'name':'" + el.Name + "','depth':" + std::to_string(el.Depth) + ",'ms':[";
		int ii = 0;
		for (auto const& ms : el.Durations)
		{
			dFile += std::to_string(ms);
			if (ii < el.Durations.size() - 1) dFile += ",";
			ii++;
		}
		dFile += "]}";
		if (i < data.size() - 1) dFile += ",";
		i++;
	}
	dFile += "]";
	return dFile;
}

#pragma region Constructors and destructor
DurationLogger::DurationLogger(const std::string& name) : Name(name)
{
	this->Initialize();
	this->Start = std::chrono::high_resolution_clock::now();
	this->SaveData();
}

DurationLogger::DurationLogger(const std::string& name, const DurationLogger* other) : Name(name)
{
	if (other != nullptr && other->Ready)
	{
		this->Depth = other->Depth + 1;
		this->File = other->File;
		this->Start = std::chrono::high_resolution_clock::now();
		this->Ready = true;
		this->SaveData();
	}
}

void DurationLogger::Initialize()
{
	std::string path = "logs/durationlogger/";
	if (rattlesmake::services::file_service::get_instance().check_if_folder_exists(path) == false)
		rattlesmake::services::file_service::get_instance().create_folder(path);

	this->File = path + "durationlogger.json";
	this->Ready = true;
}

void DurationLogger::EnableClearing()
{
	this->clear = true;
}

void DurationLogger::Clear()
{
	namesIdMap.clear();
	data.clear();
	this->clear = false;
}

void DurationLogger::SaveData()
{
	if (namesIdMap.contains(Name))
	{
		this->Id = namesIdMap[Name];
		return;
	}

	DurationLogger dl = (*this);
	dl.Durations = emptyDeque;
	dl.Ready = false;
	this->Id = (uint8_t)data.size();
	namesIdMap[Name] = this->Id;
	data.push_back(dl);
}

DurationLogger::~DurationLogger(void)
{
	if (this->Ready == false) return;

	auto End = std::chrono::high_resolution_clock::now();
	double executionTime = (double)std::chrono::duration_cast<std::chrono::nanoseconds>(End - this->Start).count();
	executionTime /= 1000000.0;
	data[this->Id].Durations.pop_front();
	data[this->Id].Durations.push_back(executionTime);

	if (this->Depth == 0)
	{
		std::ofstream dFile(this->File);
		
		if (dFile.is_open())
		{
			dFile << SerializeData();
		}
		dFile.close();

		if (this->clear) this->Clear();
	}
}
#pragma endregion
