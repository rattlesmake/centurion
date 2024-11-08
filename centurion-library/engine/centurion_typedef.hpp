/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <cctype>
#include <string>
#include <utility>  // Includes std::pair<T1, T2>
#include <variant>
#include <vector>

#include <tinyxml2.h>

#include <xml_typedef.h>


#ifndef CENTURION_DEBUG_MODE
#define CENTURION_DEBUG_MODE 1
#endif // !CENTURION_DEBUG_MODE


namespace centurion
{
	#pragma region bytes:
	typedef uint8_t byte_t;
	typedef uint32_t lastByteRead_t;
	#pragma endregion


	// Represents where to read the information from to create a gobject. Alternatively it can be either an XML file or a byte vector plus an offset.
	// Offset is an uint32_t indicating which is the last byte read
	typedef std::pair<std::vector<byte_t>*, lastByteRead_t> gobjBinData_t;
	typedef std::variant<tinyxml2::XMLElement*, gobjBinData_t> gobjData_t;


	#pragma region Assets:
	typedef assets::xmlClassSP_t classData_t;
	typedef assets::xmlClassesSP_t classesData_t;
	typedef assets::defaultClassCommandsList_t defaultCommandsList_t;
	typedef assets::classValuesList_t valuesList_t;
	#pragma endregion


	#pragma region Database:
	typedef std::string dbWord_t;
	typedef std::string dbTranslation_t;
	#pragma endregion


	#pragma region Editor:
	typedef uint8_t placementError_t;
	#pragma endregion


	#pragma region GObject:
	typedef uint32_t pickingID_t;
	typedef uint32_t uniqueID_t;
	#pragma endregion


	#pragma region PlaceableGObject:
	typedef std::string scriptIdName_t;
	#pragma endregion
}

using namespace centurion;
