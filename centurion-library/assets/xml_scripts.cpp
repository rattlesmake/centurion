#include "xml_scripts.h"
#include "xml_script.h"
#include "xml_class_command.h"

#include <services/pyservice.h>
#include <zipservice.h>

#include <header.h>  // TMP

namespace centurion
{
	namespace assets
	{
		#pragma region Constructor and destructor:
		xml_scripts::xml_scripts(void)
		{
			auto& zipService = rattlesmake::services::zip_service::get_instance();

			// Read py files
			{
				auto scriptsPY = zipService.get_all_files_within_folder("?data.zip", "scripts/", "py");
				auto& pyInterpreter = PyService::GetInstance().GetInterpreter(PySecurityLevel::Game);
				// Evaluate all py scripts
				const size_t nScriptSize = scriptsPY.size();
				for (size_t i = 0; i < nScriptSize; i++)
				{
					const std::string pyText = zipService.get_text_file("?data.zip", scriptsPY[i]);
					pyInterpreter.Evaluate(pyText);
				}
			}

			// Read XML files
			{
				auto scriptsXML = zipService.get_all_files_within_folder("?data.zip", "scripts/", "xml");
				const size_t nScriptsXML = scriptsXML.size();
				for (size_t i = 0; i < nScriptsXML; i++)
				{
					auto xmlText = zipService.get_text_file("?data.zip", scriptsXML[i]);

					tinyxml2::XMLDocument xmlFile;
					auto error = xmlFile.Parse(xmlText.c_str());
					if (error != tinyxml2::XML_SUCCESS)
						throw std::runtime_error(std::string(scriptsXML[i] + " XML file is corrupted.").c_str());  // TODO - exception sostituire con apposita eccezione

					auto root = xmlFile.FirstChildElement("gObjectScript");
					assert(root != nullptr);

					std::string id = tinyxml2::TryParseStrAttribute(root, "id");
					const bool bAutoExecute = tinyxml2::TryParseIntAttribute(root, "autoExecute");
					const bool bAssociableToCmd = (bAutoExecute == false) ? tinyxml2::TryParseIntAttribute(root, "associableToCmd") : false;
					const bool bNeedTarget = (bAssociableToCmd == true) ? tinyxml2::TryParseIntAttribute(root, "needTarget") : false;
					requiredCmdParams_t set{};
					if (bAssociableToCmd == true)
					{
						// Required CMD params
						tinyxml2::XMLElement* params = root->FirstChildElement("requiredCommandParameters");
						assert(params != nullptr);
						for (tinyxml2::XMLElement* _par = params->FirstChildElement(); _par != nullptr; _par = _par->NextSiblingElement())
						{
							std::string name = tinyxml2::TryParseStrAttribute(_par, "name");
							std::string type = tinyxml2::TryParseStrAttribute(_par, "type");
							assert(name.empty() == false && type.empty() == false);
							set.insert(xml_class_command_params{ std::move(name), std::move(type), "" });
						}
					}

					// Create and store a new script 
					auto scriptSP = xml_script::create(id, bAutoExecute, bAssociableToCmd, bNeedTarget, std::move(set));
					assert(this->scripts.contains(id) == false);
					this->scripts.insert({ std::move(id), std::move(scriptSP) });
				}
			}
		}

		xml_scripts::~xml_scripts(void)
		{
			#if CENTURION_DEBUG_MODE
			auto scriptIt = this->scripts.begin();
			while (scriptIt != this->scripts.end())
			{
				assert((*scriptIt).second.use_count() == 1);
				scriptIt++;
			}
			#endif // CENTURION_DEBUG_MODE
		}
		#pragma endregion


		#pragma region Public static members:
		xmlScriptsPtr_t xml_scripts::create(void)
		{
			return xmlScriptsPtr_t{ new xml_scripts{} };
		}
		#pragma endregion


		std::shared_ptr<xml_script> xml_scripts::get_script_by_id(const std::string& id) const
		{
			assert(this->scripts.contains(id) == true);
			return this->scripts.at(id);
		}
	};
};
