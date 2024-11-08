#include <xml_class_command.h>

namespace centurion
{
	namespace assets
	{
		xml_class_default_command::xml_class_default_command(tinyxml2::XMLElement* el)
		{
			this->target = tinyxml2::TryParseStrAttribute(el, "target");
			for (tinyxml2::XMLElement* cmd = el->FirstChildElement("cmd"); cmd != nullptr; cmd = cmd->NextSiblingElement())
			{
				std::string cmdName = tinyxml2::TryParseStrAttribute(cmd, "name");
				assert(cmdName.empty() == false);
				this->commands.push_back(std::move(cmdName));
			}
		}

		const std::string& xml_class_default_command::GetTargetCRef(void) const noexcept
		{
			return this->target;
		}

		const std::list<std::string> xml_class_default_command::GetCommandListCRef(void) const noexcept
		{
			return this->commands;
		}

		std::list<std::string> const* xml_class_default_command::GetCommandListCPtr(void) const noexcept
		{
			return (&this->commands);
		}

		xml_class_command::xml_class_command(tinyxml2::XMLElement* el, const uint16_t _readOrder, const bool opensIframe) : cmd(std::shared_ptr<cmd_s>(new cmd_s()))
		{
			this->cmd->readOrder = _readOrder;
			this->cmd->id = tinyxml2::TryParseStrAttribute(el, "id");
			// Assertion: ID can't be empty.
			assert(this->cmd->id.empty() == false);

			/// Reading of command method and provided params
			this->cmd->method = tinyxml2::TryParseStrAttribute(el, "associatedMethod");
			tinyxml2::XMLElement* params = el->FirstChildElement("params");
			assert(params != nullptr);
			for (tinyxml2::XMLElement* _par = params->FirstChildElement(); _par != nullptr; _par = _par->NextSiblingElement())
			{
				std::string name = tinyxml2::TryParseStrAttribute(_par, "name");
				std::string type = tinyxml2::TryParseStrAttribute(_par, "type");
				std::string value = tinyxml2::TryParseStrAttribute(_par, "value");
				assert(name.empty() == false && type.empty() == false && value.empty() == false);
				this->cmd->params.push_back(xml_class_command_params{ std::move(name), std::move(type), std::move(value) });
			}
			this->cmd->bottombarPriority = tinyxml2::TryParseIntAttribute(el, "bottombarPriority");

			this->cmd->displayedName = tinyxml2::TryParseFirstChildStrContent(el, "displayedName");
			this->cmd->description = tinyxml2::TryParseFirstChildStrContent(el, "description");
			this->cmd->hotKey = tinyxml2::TryParseFirstChildStrContent(el, "hotKey");

			this->cmd->staminaCost = tinyxml2::TryParseFirstChildIntContent(el, "staminaCost");
			this->cmd->goldCost = tinyxml2::TryParseFirstChildIntContent(el, "goldCost");
			this->cmd->foodCost = tinyxml2::TryParseFirstChildIntContent(el, "foodCost");
			this->cmd->populationCost = tinyxml2::TryParseFirstChildIntContent(el, "populationCost");
			this->cmd->delay = tinyxml2::TryParseFirstChildIntContent(el, "delay");

			if (opensIframe == true && el->FirstChildElement("iframeToOpen") != nullptr)
			{
				this->cmd->iframeToOpen = tinyxml2::TryParseFirstChildStrContent(el, "iframeToOpen");
			}

			this->cmd->bottomBarIcon = Encode::FixImageName(tinyxml2::TryParseFirstChildStrContent(el, "bottomBarIcon"));
			this->cmd->topBarIcon = Encode::FixImageName(tinyxml2::TryParseFirstChildStrContent(el, "topBarIcon"));

			//Get cmd type.
			{
				const std::string typeCommand = tinyxml2::TryParseFirstChildStrContent(el, "type");
				if (typeCommand == CMD_TYPE_1)
					this->cmd->type = xml_command_type::Training;
				else if (typeCommand == CMD_TYPE_2)
					this->cmd->type = xml_command_type::Technology;
				else
					this->cmd->type = xml_command_type::Other;
			}
		}

		bool xml_class_command::operator<(const xml_class_command& other) const noexcept
		{
			// Try to order by priority bar
			if (this->cmd->bottombarPriority < other.cmd->bottombarPriority)
				return true;
			if (this->cmd->bottombarPriority > other.cmd->bottombarPriority)
				return false;

			// Priority bar is the same: try to order by inheritance (inherited commands have have higher priority than a self command with the same priority)
			if (this->inheritedCmdLevel > other.inheritedCmdLevel)
				return true;
			if (this->inheritedCmdLevel < other.inheritedCmdLevel)
				return false;

			// Inheritance level is the same: order by read order (a cmd write before in the XML will have an higher priority).
			return (this->cmd->readOrder < other.cmd->readOrder);
		}

		const std::string& xml_class_command::GetIdCRef(void) const noexcept
		{
			return this->cmd->id;
		}

		const std::string& xml_class_command::GetDisplayedNameCRef(void) const noexcept
		{
			return this->cmd->displayedName;
		}

		const std::string& xml_class_command::GetDescriptionCRef(void) const noexcept
		{
			return this->cmd->description;
		}

		const std::string& xml_class_command::GetMethodCRef(void) const noexcept
		{
			return this->cmd->method;
		}

		const std::string& xml_class_command::GetHotKeyCRef(void) const noexcept
		{
			return this->cmd->hotKey;
		}

		const std::string& xml_class_command::GetBottombarIconCRef(void) const noexcept
		{
			return this->cmd->bottomBarIcon;
		}

		const std::string& xml_class_command::GetTopbarIconCRef(void) const noexcept
		{
			return this->cmd->topBarIcon;
		}

		const std::string& xml_class_command::GetIframeToOpen(void) const noexcept
		{
			return this->cmd->iframeToOpen;
		}

		xml_command_type xml_class_command::GetType(void) const noexcept
		{
			return this->cmd->type;
		}

		uint8_t xml_class_command::GetStaminaCost(void) const noexcept
		{
			return this->cmd->staminaCost;
		}

		const uint16_t xml_class_command::GetBottombarPriority(void) const noexcept
		{
			return this->cmd->bottombarPriority;
		}

		const uint16_t xml_class_command::GetInheritedLevel(void) const noexcept
		{
			return this->inheritedCmdLevel;
		}

		const uint16_t xml_class_command::GetXMLReadOrder(void) const noexcept
		{
			return this->cmd->readOrder;
		}

		uint32_t xml_class_command::GetGoldCost(void) const noexcept
		{
			return this->cmd->goldCost;
		}

		uint32_t xml_class_command::GetFoodCost(void) const noexcept
		{
			return this->cmd->foodCost;
		}

		uint32_t xml_class_command::GetPopulationCost(void) const noexcept
		{
			return this->cmd->populationCost;
		}

		uint32_t xml_class_command::GetDelay(void) const noexcept
		{
			return this->cmd->delay;
		}

		const providedCmdParams_t& xml_class_command::GetParamsListCRef(void) const noexcept
		{
			return this->cmd->params;
		}

		void xml_class_command::IncrementBarPriority(const uint32_t i)
		{
			this->cmd->bottombarPriority += i;
		}

		void xml_class_command::SetInheritedCmdLevel(const uint16_t _inheritedCmdLevel)
		{
			this->inheritedCmdLevel = _inheritedCmdLevel;
		}
	};
};