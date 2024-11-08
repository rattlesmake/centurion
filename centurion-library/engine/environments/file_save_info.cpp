#include "file_save_info.h"

FileSaveInfo::FileSaveInfo(std::string&& _time, std::string&& _difficulty, uint8_t _nPlayers) :
	time(std::move(_time)), difficulty(std::move(_difficulty)), nPlayers(_nPlayers)
{
}

std::string FileSaveInfo::GetTime(void) const
{
	return this->time;
}

std::string FileSaveInfo::GetDifficulty(void) const
{
	return this->difficulty;
}

uint8_t FileSaveInfo::GetPlayersNumber(void) const
{
	return this->nPlayers;
}

FileSaveInfo FileSaveInfo::GetFileSaveInfo(BinaryFileReader& bfr)
{
	bfr.ReadString();
	bfr.ReadString();
	bfr.ReadNumber();
	std::string time = bfr.GetString();
	std::string difficulty = bfr.GetString();
	uint8_t nPlayers = static_cast<uint8_t>(bfr.GetNumber());
	return FileSaveInfo(std::move(time), std::move(difficulty), nPlayers);
}
