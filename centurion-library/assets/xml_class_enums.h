/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <xml_typedef.h>

#include <string>
#include <unordered_map>

namespace centurion
{
	namespace assets
	{
		/*
			Define the type of a class using a specific parameter written into the xml files
			It allows to call the correct constructor
		*/
		enum class xml_class_type : xmlClassTypeInt_t
		{
			/// Every time a new element is added/updated here, update even GObject::GetType
			/// Moreover, give a look to PlayersArray::OpenObjPropsIframe, xml_class::xml_class constructor, GObject::Create, SelectedObjects.cpp,
			/// ObjsList::Insert, ObjsList::GetOut

			e_unknown = 0,
			e_noClass,
			e_abstractClass,
			e_buildingClass,
			e_unitClass,
			e_heroClass,
			e_wagonClass,
			e_decorationClass,
			e_druidClass,
			e_vfxClass,
			e_settlementClass,
		};

		const std::unordered_map<xml_class_type, std::string> xml_class_types_str{
			{ xml_class_type::e_abstractClass, "cpp_abstractclass"},
			{ xml_class_type::e_buildingClass, "cpp_buildingclass"},
			{ xml_class_type::e_unitClass, "cpp_unitclass"},
			{ xml_class_type::e_heroClass, "cpp_heroclass"},
			{ xml_class_type::e_wagonClass, "cpp_wagonclass"},
			{ xml_class_type::e_druidClass, "cpp_druidclass"},
			{ xml_class_type::e_decorationClass, "cpp_decorationclass"},
			{ xml_class_type::e_vfxClass, "cpp_vfxclass"},
		};

		enum class xml_command_type : xmlCommandTypeInt
		{
			Training = 0,
			Technology = 1,
			Other = 2
		};
	};
};
