#include "game_time.h"

#include <bin_data_interpreter.h>

double Time::frameDurationMs = 16.6666666;

Time::Time(void) :
	start(std::chrono::system_clock::now()), oldElapsedSeconds(0.0)
{
}

Time::Time(std::vector<uint8_t>&& timeBytes)
{
	///If you change loading order here, go to Time::GetBinRepresentation ad impose the same saving order
	uint32_t offset = 0;

	this->totalMs = BinaryDataInterpreter::ExtractDouble(timeBytes, offset);
	this->lastTime = BinaryDataInterpreter::ExtractDouble(timeBytes, offset);
	this->totalSeconds = BinaryDataInterpreter::ExtractDouble(timeBytes, offset);

	this->hours = BinaryDataInterpreter::ExtractUInt32(timeBytes, offset);
	this->minutes = BinaryDataInterpreter::ExtractUInt8(timeBytes, offset);
	this->seconds = BinaryDataInterpreter::ExtractUInt8(timeBytes, offset);

	//Extract elapsed seconds of previous match:
	this->oldElapsedSeconds = BinaryDataInterpreter::ExtractDouble(timeBytes, offset);

	assert(static_cast<uint64_t>(offset) == timeBytes.size());

	this->start = std::chrono::system_clock::now();
}

Time::~Time(void)
{
}

uint32_t Time::GetCurrentFrame(void) const
{
	return this->framesCounter;
}

double Time::GetElapsedSecs(void) const
{
	duration<double> oldElapsedSecondsDur{ this->oldElapsedSeconds };
	auto now = system_clock::now();
	duration<double> elapsed_seconds = (now - this->start) + oldElapsedSecondsDur;
	return static_cast<double>(elapsed_seconds.count());
}

void Time::Reset(void)
{
	this->framesCounter = 0;
	this->totalMs = 0.0;
	this->lastTime = 0.0;
	this->totalSeconds = 0.0;
	this->hours = 0;
	this->minutes = 0;
	this->seconds = 0;

	//Reset start time.
	this->start = std::chrono::system_clock::now();
}

void Time::Update(void)
{
	this->framesCounter++;
	this->totalMs += Time::frameDurationMs;
	this->totalSeconds = this->totalMs / 1000.0;

	if (this->totalSeconds - this->lastTime > 1.f)
	{
		if (this->seconds < 59)
		{
			this->seconds++;
		}
		else
		{
			this->seconds = 0;
				
			if (this->minutes < 59)
				this->minutes++;
			else
			{
				this->minutes = 0;
				this->hours++;
			}
		}
			
		// update lastTime with current time
		this->lastTime = this->totalSeconds;
	}
}

std::string Time::GetFullTimeString(void) const
{
	return (this->GetHoursString() + ":" + this->GetMinutesString() + ":" + this->GetSecondsString());
}

uint64_t Time::GetTotalMinutesInt(void) const
{
	return static_cast<uint64_t>(this->totalSeconds / 60.0);
}

uint64_t Time::GetTotalSecondsInt(void) const
{
	return static_cast<uint64_t>(this->totalMs * 1000);
}

double Time::GetTotalMinutes(void) const
{
	return (GetTotalSeconds() / 60.f);
}

double Time::GetTotalSeconds(void) const
{
	return this->totalSeconds;
}

std::string Time::GetTotalMinutesString(void) const
{
	return std::to_string(this->totalSeconds / 60.0);
}

std::string Time::GetTotalSecondsString(void) const
{
	return std::to_string(this->totalSeconds);
}

uint32_t Time::GetHours(void) const
{
	return this->hours;
}

uint8_t Time::GetMinutes(void) const
{
	return this->minutes;
}

uint8_t Time::GetSeconds(void) const
{
	return this->seconds;
}

float Time::GetHoursFloat(void) const
{
	return static_cast<float>(this->hours);
}

float Time::GetMinutesFloat(void) const
{
	return static_cast<float>(this->minutes);
}

float Time::GetSecondsFloat(void) const
{
	return static_cast<float>(this->seconds);
}

std::string Time::GetHoursString(void) const
{
	if (this->hours < 10)
		return "0" + std::to_string(this->hours);
	else
		return std::to_string(this->hours);
}

std::string Time::GetMinutesString(void) const
{
	if (this->minutes < 10)
		return "0" + std::to_string(this->minutes);
	else
		return std::to_string(this->minutes);
}

std::string Time::GetSecondsString(void) const
{
	if (this->seconds < 10)
		return "0" + std::to_string(this->seconds);
	else
		return std::to_string(this->seconds);
}

bool Time::IsFrameMultipleOf(const uint32_t value)const
{
	return ((this->framesCounter % value) == 0);
}

void Time::GetBinRepresentation(std::vector<uint8_t>& data)
{
	///If you change saving order here, go to secondary Time constructor ad impose the same loading order

	BinaryDataInterpreter::PushDouble(data, this->totalMs);
	BinaryDataInterpreter::PushDouble(data, this->lastTime);
	BinaryDataInterpreter::PushDouble(data, this->totalSeconds);

	BinaryDataInterpreter::PushUInt32(data, this->hours);
	BinaryDataInterpreter::PushUInt8(data, this->minutes);
	BinaryDataInterpreter::PushUInt8(data, this->seconds);

	//Save elapsed seconds until now
	BinaryDataInterpreter::PushDouble(data, this->GetElapsedSecs());
}
