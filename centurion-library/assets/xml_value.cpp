#include "xml_value.h"

// utils
#include <tinyxml2_utils.h>
#include <encode_utils.h>

#include <cassert>


using tinyxml2::TryParseStrAttribute;


namespace centurion
{
	namespace assets
	{
		#pragma region Constuctor and destructor:
		xml_value::xml_value(tinyxml2::XMLElement* xml_element)
		{
			this->name = TryParseStrAttribute(xml_element, "name");
			assert(this->name.empty() == false);

			this->icon = Encode::FixImageName(TryParseStrAttribute(xml_element, "icon"));
			this->function = TryParseStrAttribute(xml_element, "scriptFun");
		}

		xml_value::~xml_value(void)
		{
		}
		#pragma endregion


		#pragma region Operators:
		bool xml_value::operator==(const xml_value& other) const noexcept
		{
			return (this->name == other.name);
		}
		#pragma endregion


		#pragma region Public static members:
		const std::shared_ptr<xml_value> xml_value::create(tinyxml2::XMLElement* xml_element)
		{
			return std::shared_ptr<xml_value>{ new xml_value{ xml_element } };
		}
		#pragma endregion


		#pragma region Public members:
		const std::string& xml_value::get_icon_path_cref(void) const noexcept
		{
			return this->icon;
		}

		const std::string& xml_value::get_function_cref(void) const noexcept
		{
			return this->function;
		}

		const std::string& xml_value::get_name_cref(void) const noexcept
		{
			return this->name;
		}
		#pragma endregion
	};
};
