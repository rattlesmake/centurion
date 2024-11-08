/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <memory>
#include <string>

#include <centurion_typedef.hpp>
#include <xml_class_enums.h>

#include <iframe/imguiImageButton.h>

/// <summary>
/// Command class, useful to update buttons on bottom bar
/// </summary>
class Command
{
public:
	#pragma region Constructors, destructor and operators:
	Command(const Command& other) = delete;
	~Command(void);

	Command& operator=(const Command& other) = delete;
	[[nodiscard]] bool operator==(const Command& other) noexcept;
	friend bool operator<(const Command& lhs, const Command& rhs) noexcept;

	struct CommandSPComparator
	{
		bool operator() (const std::shared_ptr<Command>& lhs, const std::shared_ptr<Command>& rhs) const
		{
			const auto& a = (*lhs);
			const auto& b = (*rhs);
			return a < b;
		}
	};
	#pragma endregion

	#pragma region Static members:
	[[nodiscard]] static std::shared_ptr<Command> CreateCommand(const assets::xmlClassCommand_t& xmlCommand, const bool bIsUICommand);
	[[nodiscard]] static std::shared_ptr<Command> GetInvalidCommand(void);
	#pragma endregion

	#pragma region To external scripts methods:
	[[nodiscard]] bool IsValid(void) const noexcept;

	void SetRollover(const std::string rollover);
	[[nodiscard]] std::string GetRollover(void) const noexcept;
	void ResetRollover(void) noexcept;

	void SetStaminaCost(const uint8_t value);
	[[nodiscard]] uint8_t GetStaminaCost(void) const noexcept;

	void SetGoldCost(const uint32_t value);
	[[nodiscard]] uint32_t GetGoldCost(void) const noexcept;

	void SetFoodCost(const uint32_t value);
	[[nodiscard]] uint32_t GetFoodCost(void) const noexcept;

	void SetPopulationCost(const uint32_t value);
	[[nodiscard]] uint32_t GetPopulationCost(void) const noexcept;
	#pragma endregion

	[[nodiscard]] const string& GetIdCRef(void) const noexcept;
	[[nodiscard]] const string& GetDisplayedNameCRef(void) const noexcept;
	[[nodiscard]] const string& GetDescriptionCRef(void) const noexcept;
	[[nodiscard]] assets::xml_command_type GetType(void) const noexcept;
	[[nodiscard]] uint16_t GetPriority(void) const noexcept;
	[[nodiscard]] uint32_t GetDelay(void) const noexcept;
	[[nodiscard]] const string& GetTopbarImageNameCRef(void) const noexcept;
	[[nodiscard]] const string& GetBottombarImageNameCRef(void) const noexcept;
	[[nodiscard]] const string& GetMethodCRef(void) const noexcept;
	[[nodiscard]] const assets::providedCmdParams_t& GetParamsList(void) const noexcept;
	void SetEnableness(const bool _bIsEnabled);
	[[nodiscard]] bool IsDisabled(void) const noexcept;

	[[nodiscard]] gui::ImGuiTooltip::tooltip_t GetTooltipInfo(void);

	void CatchRolloverForBottomBar(void);
	void Execute(void);
private:
	Command(void);
	Command(const assets::xmlClassCommand_t& xmlCommand, const bool bIsUICommand);
	bool OpensIframeOnClick(void) const noexcept;
	bool ExecuteMethodOnClick(void) const noexcept;

	bool bEnabled = true;
	bool bIsUICommand = false;
	bool bValid = false;
	assets::xml_command_type type{ assets::xml_command_type::Other };
	uint8_t staminaCost = 0;
	const uint16_t priority;
	const uint16_t inheritedCmdLevel;
	const uint16_t xmlReadOrder;
	uint32_t goldCost = 0;
	uint32_t foodCost = 0;
	uint32_t populationCost = 0;
	uint32_t delay = 0;
	const std::string& id;
	const std::string& iframeToOpen;
	const std::string& displayedName;
	const std::string& description;
	const std::string& method;
	const std::string& bottomImageName;
	const std::string& topImageName;
	const assets::providedCmdParams_t& params;
	std::string rollover;
};
