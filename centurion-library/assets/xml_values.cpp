#include "xml_values.h"
#include "xml_value.h"

#include <services/pyservice.h>
#include <zipservice.h>


namespace centurion
{
	namespace assets
	{
		#pragma region Constructor and destructor:
		xml_values::xml_values(void) :
			python_s(PyService::GetInstance()),
			zip_s(rattlesmake::services::zip_service::get_instance())
		{
			std::string xml_text = zip_s.get_text_file("?data.zip", "object_values/values.xml");
			// TODO exception: sostituire assert con eccezioni
			tinyxml2::XMLDocument xml_file;
			tinyxml2::XMLError result = xml_file.Parse(xml_text.c_str());
			assert(result == tinyxml2::XML_SUCCESS);
			tinyxml2::XMLElement* values_xml = xml_file.FirstChildElement("values");
			assert(values_xml != nullptr);

			for (tinyxml2::XMLElement* value_xml = values_xml->FirstChildElement("value"); value_xml != nullptr; value_xml = value_xml->NextSiblingElement())
			{
				std::shared_ptr<const xml_value> vSP(xml_value::create(value_xml));
				this->values_map.insert({ vSP->name, std::move(vSP) });
			}

			//Python values functions
			auto fun = zip_s.get_text_file("?data.zip", "object_values/values.py");
			python_s.GetInterpreter(PySecurityLevel::Game).Evaluate(fun);
		}

		xml_values::~xml_values(void)
		{
		}
		#pragma endregion


		#pragma region Static public members:
		xmlValuesPtr_t xml_values::create(void)
		{
			return xmlValuesPtr_t{ new xml_values{} };
		}
		#pragma endregion


		const std::shared_ptr<const xml_value> xml_values::get_value_ptr(const std::string& name) const noexcept
		{
			return (this->values_map.contains(name) == true) ? this->values_map.at(name) : std::shared_ptr<const xml_value>();
		}
	};
};
