/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/
#pragma once

#include <string>

#include <binary_file_reader.h>

#ifndef CENTURION_SAVE_EXTENSION
#define CENTURION_SAVE_EXTENSION    std::string("csave")
#endif

#ifndef CENTURION_QUICKSAVE
#define CENTURION_QUICKSAVE    std::string("quicksave")
#endif

class FileSaveInfo
{
public:
	[[nodiscard]] std::string GetTime(void) const;
	[[nodiscard]] std::string GetDifficulty(void) const;
	[[nodiscard]] uint8_t GetPlayersNumber(void) const;

	[[nodiscard]] static FileSaveInfo GetFileSaveInfo(BinaryFileReader& bfr);
private:
	FileSaveInfo(std::string&& _time, std::string&& _difficulty, uint8_t _nPlayers);

	std::string time;
	std::string difficulty;
	uint8_t nPlayers;
};
