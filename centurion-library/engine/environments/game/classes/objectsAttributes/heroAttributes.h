/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <cctype>
#include <memory>
#include <string>

#include <centurion_typedef.hpp>


#ifndef MAX_TROUP_EXP_PERC_GAIN_VALUE
#define MAX_TROUP_EXP_PERC_GAIN_VALUE 100
#endif 


class HeroAttributes
{
public:
	HeroAttributes(const HeroAttributes& other) = delete;
	HeroAttributes& operator=(const HeroAttributes& other) = delete;

	[[nodiscard]] uint8_t GetTroupExpPercGain(void) const noexcept;
	void SetTroupExpPercGain(const uint8_t _troupExpPercGain);
protected:
	HeroAttributes(void) = default;

	void SetAttributes(const classData_t& objData, const bool _temporary);
private:
	uint8_t troupExpPercGain = 0;
};
