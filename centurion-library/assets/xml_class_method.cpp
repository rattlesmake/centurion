#include "xml_class_method.h"
#include "xml_script.h"
#include "xml_scripts.h"

namespace centurion
{
	namespace assets
	{
		xml_class_method::xml_class_method(tinyxml2::XMLElement* el, const std::shared_ptr<xml_scripts>& scripts_collection)
		{
			this->name = tinyxml2::TryParseStrAttribute(el, "name");
			this->associated_script = scripts_collection->get_script_by_id(tinyxml2::TryParseStrAttribute(el, "scriptID"));
		}

		const std::string& xml_class_method::get_name_cref(void) const noexcept
		{
			return this->name;
		}

		const std::string& xml_class_method::get_script_id_cref(void) const noexcept
		{
			return this->associated_script.lock()->get_id_cref();
		}

		std::shared_ptr<centurion::assets::xml_script> xml_class_method::get_associated_script(void) const noexcept
		{
			return this->associated_script.lock();
		}

		bool xml_class_method::is_valid(void) const noexcept
		{
			return this->name.empty() == false && this->associated_script.lock();
		}
	};
};
