#include "diplomacy.h"
#include <services/logservice.h>
#include <players/players_array.h>

bool CheckEnteredValues(const uint16_t player1, const uint16_t player2, const bool both)
{
	// Entered values can't be zero
	if (player1 == 0 || player2 == 0)
	{
		if (both == false)
		{
			Logger::LogMessage msg = Logger::LogMessage("Player number can't be zero.", "Warn", "Diplomacy", "", __FUNCTION__);
			Logger::Warn(msg);
		}
		return false;
	}

	// Entered values can't be higher than MAX_NUMBER_OF_PLAYERS
	if (player1 > PlayersArray::GetArraySize() || player2 > PlayersArray::GetArraySize())
	{
		if (both == false)
		{
			Logger::LogMessage msg = Logger::LogMessage("One of the two players' entered value is greater than the maximum number of players.", "Warn", "Diplomacy", "", __FUNCTION__);
			Logger::Warn(msg);
		}
		return false;
	}

	// Entered values can't be the same
	if (player1 == player2)
	{
		if (both == false)
		{
			Logger::LogMessage msg = Logger::LogMessage("Entered values of both players are the same.", "Warn", "Diplomacy", "", __FUNCTION__);
			Logger::Warn(msg);
		}
		return false;
	}

	return true;
}

bool Diplomacy::SetCeaseFire(const uint16_t player1, const uint16_t player2, const bool status, const bool both)
{
	if (CheckEnteredValues(player1, player2, both) == false)
		return false;

	// It's necessary to reduce both entered values by 1 so that they are scaled for the array system properly
	uint16_t rPlayer1 = player1 - 1;
	uint16_t rPlayer2 = player2 - 1;

	// Storing all player1 diplomacy relationship statuses
	auto player1Diplomacy = playersList[rPlayer1];

	// Setting player1 "ceaseFire" diplomatic relationship to player 2
	auto player1Player2Status = player1Diplomacy[rPlayer2].ceaseFire = status;

	// Since these are temporary variables, it's necessary to apply the changes previously made to diplomatic relations
	playersList[rPlayer1] = player1Diplomacy;
	
	// If the diplomatic relationship is to be applied to both players, a semi-recursive call is made, reversing both players
	// in the parameter list.
	if (both == true)
		SetCeaseFire(player2, player1, status, false);
	
	return true;
}

bool Diplomacy::GetCeaseFire(const uint16_t player1, const uint16_t player2)
{
	if (CheckEnteredValues(player1, player2, false) == false)
		return false;

	uint16_t rPlayer1 = player1 - 1;
	uint16_t rPlayer2 = player2 - 1;

	return playersList[rPlayer1][rPlayer2].ceaseFire;
}

bool Diplomacy::SetShareSupport(const uint16_t player1, const uint16_t player2, const bool status, const bool both)
{
	if (CheckEnteredValues(player1, player2, both) == false)
		return false;

	// It's necessary to reduce both entered values by 1 so that they are scaled for the array system properly
	uint16_t rPlayer1 = player1 - 1;
	uint16_t rPlayer2 = player2 - 1;

	// Storing all player1 diplomacy relationship statuses
	auto player1Diplomacy = playersList[rPlayer1];

	// Setting player1 "shareSupport" diplomatic relationship to player 2
	auto player1Player2Status = player1Diplomacy[rPlayer2].shareSupport = status;

	// Since these are temporary variables, it's necessary to apply the changes previously made to diplomatic relations
	playersList[rPlayer1] = player1Diplomacy;

	// If the diplomatic relationship is to be applied to both players, a semi-recursive call is made, reversing both players
	// in the parameter list.
	if (both == true)
		SetShareSupport(player2, player1, status, false);

	return true;
}

bool Diplomacy::GetShareSupport(const uint16_t player1, const uint16_t player2)
{
	if (CheckEnteredValues(player1, player2, false) == false)
		return false;

	uint16_t rPlayer1 = player1 - 1;
	uint16_t rPlayer2 = player2 - 1;

	return playersList[rPlayer1][rPlayer2].shareSupport;
}

bool Diplomacy::SetShareView(const uint16_t player1, const uint16_t player2, const bool status, const bool both)
{
	if (CheckEnteredValues(player1, player2, both) == false)
		return false;

	// It's necessary to reduce both entered values by 1 so that they are scaled for the array system properly
	uint16_t rPlayer1 = player1 - 1;
	uint16_t rPlayer2 = player2 - 1;

	// Storing all player1 diplomacy relationship statuses
	auto player1Diplomacy = playersList[rPlayer1];

	// Setting player1 "shareView" diplomatic relationship to player 2
	auto player1Player2Status = player1Diplomacy[rPlayer2].shareView = status;

	// Since these are temporary variables, it's necessary to apply the changes previously made to diplomatic relations
	playersList[rPlayer1] = player1Diplomacy;

	// If the diplomatic relationship is to be applied to both players, a semi-recursive call is made, reversing both players
	// in the parameter list.
	if (both == true)
		SetShareView(player2, player1, status, false);

	return true;
}

bool Diplomacy::GetShareView(const uint16_t player1, const uint16_t player2)
{
	if (CheckEnteredValues(player1, player2, false) == false)
		return false;

	uint16_t rPlayer1 = player1 - 1;
	uint16_t rPlayer2 = player2 - 1;

	return playersList[rPlayer1][rPlayer2].shareView;
}

bool Diplomacy::SetShareControl(const uint16_t player1, const uint16_t player2, const bool status, const bool both)
{
	if (CheckEnteredValues(player1, player2, both) == false)
		return false;

	// It's necessary to reduce both entered values by 1 so that they are scaled for the array system properly
	uint16_t rPlayer1 = player1 - 1;
	uint16_t rPlayer2 = player2 - 1;

	// Storing all player1 diplomacy relationship statuses
	auto player1Diplomacy = playersList[rPlayer1];

	// Setting player1 "shareControl" diplomatic relationship to player 2
	auto player1Player2Status = player1Diplomacy[rPlayer2].shareControl = status;

	// Since these are temporary variables, it's necessary to apply the changes previously made to diplomatic relations
	playersList[rPlayer1] = player1Diplomacy;

	// If the diplomatic relationship is to be applied to both players, a semi-recursive call is made, reversing both players
	// in the parameter list.
	if (both == true)
		SetShareControl(player2, player1, status, false);

	return true;
}

bool Diplomacy::GetShareControl(const uint16_t player1, const uint16_t player2)
{
	if (CheckEnteredValues(player1, player2, false) == false)
		return false;

	uint16_t rPlayer1 = player1 - 1;
	uint16_t rPlayer2 = player2 - 1;

	return playersList[rPlayer1][rPlayer2].shareControl;
}

void Diplomacy::ResetStatuses(void)
{
	const uint8_t players = PlayersArray::GetArraySize();
	for (uint16_t i = 0; i < players; i++)
	{
		for (uint16_t j = 0; j < players; j++)
		{
			bool isSamePlayer = (i == j);
			playersList[i][j].ceaseFire = isSamePlayer;
			playersList[i][j].shareSupport = isSamePlayer;
			playersList[i][j].shareView = isSamePlayer;
			playersList[i][j].shareControl = isSamePlayer;
		}
	}
}
