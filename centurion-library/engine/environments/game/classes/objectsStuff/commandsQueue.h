/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <string>
#include <list>
#include <memory>
#include <array>

#include <centurion_typedef.hpp>

#include <environments/game/classes/objectsStuff/objectCommand.h>

#ifndef COMMANDS_QUEUE_NUMBER
#define COMMANDS_QUEUE_NUMBER   3
#endif // !COMMANDS_QUEUE_NUMBER


class Playable;

/// <summary>
/// This class contains three queue of pending commands: tranings, technologies, others.
/// </summary>
class CommandsQueue
{
public:
	struct CommandsDequeElement_s
	{
		std::weak_ptr<Command> cmd;
		uint32_t counter = 0; 
		float completionPerc = -1.0f;  // This info is used in topbar.cpp
	};
	typedef std::deque<CommandsDequeElement_s> cmdsQueue_t;

	CommandsQueue(const CommandsQueue& other) = delete;
	CommandsQueue& operator=(const CommandsQueue& other) = delete;
	~CommandsQueue(void);

	[[nodiscard]] static std::shared_ptr<CommandsQueue> Create(const std::shared_ptr<Playable>& owner, gobjData_t* dataSource);
	void static SetClassesDataRef(classesData_t _classesDataSP);

	void Push(const std::shared_ptr<Command>& cmd);
	[[nodiscard]] const cmdsQueue_t& GetQueueByType(const assets::xml_command_type wantedType) const;
	void RemoveElement(const assets::xml_command_type wantedType, uint32_t pos);
	void ResetCmdWaitingForTarget(void);
	void Reset(uint8_t returnedResourcesPerc);

	/// <summary>
	/// For each queue, execute the first command ready for the execution.
	/// Executed commands will be removed from their queue.
	/// </summary>
	void ExecuteReadyCommands(void);

	void GetBinRepresentation(std::vector<byte_t>& data);
private:
	explicit CommandsQueue(const std::weak_ptr<Playable>& _owner);

	/// This function applies the costs of the command if it is being pushed.
	/// Otherwise, this function recoveres the costs of the command if it is being popped.
	static void HandleCommandResources(const Command& cmd, Playable& obj, const bool bPush, const uint8_t costToApplyPerc);

	void SetCommandWaitingForTarget(const Playable& obj, const std::weak_ptr<Command>& cmd);

	void InsertCmd(const std::shared_ptr<Command>& cmd);

	void TryToCatchTarget(void);

	[[nodiscard]] const cmdsQueue_t& GetOtherCommands(void) const;
	[[nodiscard]] const cmdsQueue_t& GetTechnologiesCommands(void) const;
	[[nodiscard]] const cmdsQueue_t& GetTrainingsCommands(void) const;

	void Remove(const assets::xmlCommandTypeInt index, const size_t pos, uint32_t nRemoval, const uint8_t returnedResourcesPerc);

	void ClearAllByIndex(const assets::xmlCommandTypeInt index, const uint8_t returnedResourcesPerc);

	// Fill the command queue by binary data (read by binary file save)
	void InitByBinData(gobjBinData_t& binData);

	// The PlayableGObject who has a command queue.
	std::weak_ptr<Playable> owner;

	// A command that needs a command provided by the player before it can be executed.
	std::weak_ptr<Command> cmdWaitingForTarget;

	// For each type of command (technology, training, other) we keep a deque of pending commands.
	std::array<cmdsQueue_t, COMMANDS_QUEUE_NUMBER> pendingCommands;

	typedef float timeInstant_t;

	struct cmdBeingProcessed
	{
		timeInstant_t startInstantOfProcessing = -1.0f;
		timeInstant_t currentInstantOfProcessing = -1.0f;

		void reset(void) noexcept
		{
			this->startInstantOfProcessing = -1.0;
			this->currentInstantOfProcessing = -1.0;
		}

		[[nodiscard]] bool empty(void) const noexcept
		{
			return (this->startInstantOfProcessing == -1.0);
		}
	};

	// For each type of commands, we keep the processing status of the first command (it's the first pending command)
	std::array<cmdBeingProcessed, COMMANDS_QUEUE_NUMBER> cmdsBeingProcessed;

	// Contains info about classes. Must be setted externally before invoke CommandQueue::Create
	static classesData_t classesDataSP;
};
