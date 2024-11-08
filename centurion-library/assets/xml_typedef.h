/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <cctype>
#include <list>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>


namespace centurion
{
	namespace assets
	{
		// Here you can put incomplete classes definitions
		class xml_class;
		class xml_classes;
		class xml_class_command;
		class xml_class_default_command;
		class xml_value;
		struct xml_class_command_params;

		class xml_entity;
		struct xml_entity_point;
	}


	namespace assets
	{
		typedef uint16_t xmlClassTypeInt_t;


		#pragma region Shared Pointers
		typedef std::shared_ptr<xml_class> xmlClassSP_t;
		typedef std::shared_ptr<xml_classes> xmlClassesSP_t;
		typedef std::shared_ptr<xml_class_command> xmlCmdSP_t;
		typedef std::shared_ptr<xml_class_default_command> xmlDefaultCmdSP_t;
		typedef std::shared_ptr<xml_entity> xmlEntitySP_t;
		typedef std::shared_ptr<const xml_value> xmlValueSP_t;
		#pragma endregion


		#pragma region Classes:
		typedef std::unordered_map<std::string, std::string> attributesMap_t;
		#pragma endregion



		#pragma region Commands:
		typedef uint8_t xmlCommandTypeInt;
		typedef xml_class_command xmlClassCommand_t;
		typedef std::unique_ptr<xml_class_command> xmlCmdUP_t;
		typedef std::list<xml_class_command_params> providedCmdParams_t;
		typedef std::list<xmlDefaultCmdSP_t> defaultClassCommandsList_t;
		#pragma endregion


		#pragma region Entity
		typedef std::vector<xml_entity_point> entityPointsVec_t;
		#pragma endregion


		#pragma region Values:
		typedef std::vector<xmlValueSP_t> classValuesList_t;
		#pragma endregion
	}
}
