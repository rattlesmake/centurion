/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <players/player.h>

namespace Diplomacy
{
	bool SetCeaseFire(const uint16_t player1, const uint16_t player2, const bool status, const bool both = false);
	[[nodiscard]] bool GetCeaseFire(const uint16_t player1, const uint16_t player2);
	bool SetShareSupport(const uint16_t player1, const uint16_t player2, const bool status, const bool both = false);
	[[nodiscard]] bool GetShareSupport(const uint16_t player1, const uint16_t player2);
	bool SetShareView(const uint16_t player1, const uint16_t player2, const bool status, const bool both = false);
	[[nodiscard]] bool GetShareView(const uint16_t player1, const uint16_t player2);
	bool SetShareControl(const uint16_t player1, const uint16_t player2, const bool status, const bool both = false);
	[[nodiscard]] bool GetShareControl(const uint16_t player1, const uint16_t player2);
	void ResetStatuses(void);
	namespace
	{
		struct Type
		{
			bool ceaseFire = false;
			bool shareSupport = false;
			bool shareView = false;
			bool shareControl = false;
		}	diplomacyType;

		typedef std::array<Type, 17> playerDiplomacy;
		std::array<playerDiplomacy, 17> playersList;
	};
};