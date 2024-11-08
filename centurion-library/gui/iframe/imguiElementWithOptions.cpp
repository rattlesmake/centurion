#include <iframe/imguiElementWithOptions.h>
#include <services/sqlservice.h>

namespace gui
{
	ImGuiElementWithOptions::ImGuiElementWithOptions(ClassesTypes type, std::shared_ptr<Iframe>&& iframeSP) : 
		ImGuiElement(type, std::move(iframeSP))
	{
	}
	
	#pragma region To scripts members:
	ImGuiElementWithOptions::~ImGuiElementWithOptions(void)
	{
	}
	uint32_t ImGuiElementWithOptions::GetSelectedOptionIndex(void) const
	{
		return static_cast<uint32_t>(this->currentIndex);
	}

	std::vector<std::string> ImGuiElementWithOptions::GetListOfStrings(void) const
	{
		return this->listOfStrings;
	}

	std::string ImGuiElementWithOptions::GetSelectedOption(void) const
	{
		return this->selectedOption;
	}

	void ImGuiElementWithOptions::ResetOptions(void)
	{
		this->listOfTranslatedStrings.clear();
		this->listOfBooleans.clear();
		this->listOfStrings.clear();
		this->bIsActive = false;
	}

	void ImGuiElementWithOptions::ResetSelectedOption(void)
	{
		this->selectedOption.clear();
	}

	void ImGuiElementWithOptions::UpdateOptions(std::vector<std::string> listOfStrings, const std::string prefix, const std::string current)
	{
		this->listOfTranslatedStrings.clear();
		this->listOfStrings = std::move(listOfStrings);
		this->listOfBooleans = std::vector<bool>(this->listOfStrings.size(), false);
		this->bIsActive = false;
		if (prefix.empty() == false)
		{
			for (auto const& s : this->listOfStrings)
			{
				std::string option;
				if (prefix == "True")
					option = SqlService::GetInstance().GetTranslation(s, false);
				else
					option = SqlService::GetInstance().GetTranslation(prefix + s, false);
				this->listOfTranslatedStrings.push_back(std::move(option));
			}
			if (current != "")
			{
				this->currentIndex = 0;
				for (auto const& i : this->listOfStrings)
				{
					if (current != i)
					{
						this->currentIndex++;
					}
					else
					{
						this->selectedOption = this->listOfStrings[this->currentIndex];
						break;
					}
						
				}
			}
		}
		else
		{
			this->listOfTranslatedStrings = this->listOfStrings;
		}
	}
	#pragma endregion
}
