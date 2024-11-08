#include "commandsQueue.h"

#include <environments/game/classes/playable.h>
#include <environments/game/classes/building.h>

#if CENTURION_DEBUG_MODE
#include <environments/game/classes/unit.h>
#endif

#include <environments/game/classes/objectsSet/settlement.h>
#include <GLFW/glfw3.h>

#include <bin_data_interpreter.h>

#include <custom_exceptions.hpp>

// Assets
#include <xml_classes.h>
#include <xml_class.h>
#include <xml_class_method.h>
#include <xml_script.h>

// Peripherals
#include <keyboard.h>


#pragma region Static attributes:
classesData_t CommandsQueue::classesDataSP;
#pragma endregion


#pragma region Constructor and destructor:
CommandsQueue::CommandsQueue(const std::weak_ptr<Playable>& _owner) :
	owner(_owner)
{
}

CommandsQueue::~CommandsQueue(void)
{
}
#pragma endregion


#pragma region Static members:
std::shared_ptr<CommandsQueue> CommandsQueue::Create(const std::shared_ptr<Playable>& owner, gobjData_t* dataSource)
{
	assert(owner);
	assert(CommandsQueue::classesDataSP);
	auto cQueue = std::shared_ptr<CommandsQueue>(new CommandsQueue{ owner });
	if (dataSource != nullptr && std::holds_alternative<gobjBinData_t>(*dataSource) == true)
	{
		// Load queue by binary data
		gobjBinData_t& binData = std::get<gobjBinData_t>(*dataSource);
		cQueue->InitByBinData(binData);
	}
	return cQueue;
}

void CommandsQueue::SetClassesDataRef(classesData_t _classesDataSP)
{
	CommandsQueue::classesDataSP = std::move(_classesDataSP);
}

void CommandsQueue::HandleCommandResources(const Command& cmd, Playable& obj, const bool bPush, const uint8_t costToApplyPerc)
{
	const int8_t factor = (bPush == false) ? -1 : 1;

	if (obj.IsBuilding() == true)
	{
		const std::shared_ptr<Settlement>set = static_cast<Building*>(&obj)->GetSettlement();
		if (cmd.GetGoldCost() != 0)
		{
			assert(set);
			auto a = cmd.GetGoldCost() * costToApplyPerc / 100;
			set->SetGold(set->GetGold() - factor * static_cast<int>(cmd.GetGoldCost() * costToApplyPerc / 100));
		}
		if (cmd.GetFoodCost() != 0)
		{
			const std::shared_ptr<Settlement>set = static_cast<Building*>(&obj)->GetSettlement();
			assert(set);
			set->SetFood(set->GetFood() - factor * static_cast<int>(cmd.GetFoodCost() * costToApplyPerc / 100));
		}
		if (cmd.GetPopulationCost() != 0)
		{
			if (static_cast<Building*>(&obj)->IsBroken() == false || bPush == true) 
			{
				// I can be here if:
				// 1) the building is not broken;
				// 2) I want to pay for adding a new command
				// so, if the building is broken, I can't retrieve population previously payed.
				const std::shared_ptr<Settlement>set = static_cast<Building*>(&obj)->GetSettlement();
				assert(set);
				set->SetPopulation(set->GetPopulation() - factor * static_cast<int>(cmd.GetPopulationCost() * costToApplyPerc / 100));
			}
		}
	}
}
#pragma endregion


#pragma region Public members:
void CommandsQueue::Push(const std::shared_ptr<Command>& cmd)
{
	std::shared_ptr<Playable> objSP = this->owner.lock();

	const auto& methodsMap = CommandsQueue::classesDataSP->get_xml_class(objSP->GetClassNameCRef())->get_unrepeated_methods_map_cref();
	auto scriptSP = methodsMap.at(cmd->GetMethodCRef())->get_associated_script();

	if (scriptSP->is_target_required() == true)
	{
		// Set command as waiting for a target provided by the player. 
		// When a target will be catched, command will be appended.
		if (this->cmdWaitingForTarget.expired() == false)
			this->SetCommandWaitingForTarget((*objSP), std::weak_ptr<Command>());  // Before set a new command waiting for a target, reset old one.
		this->SetCommandWaitingForTarget((*objSP), cmd);
	}
	else  // Command doesn't require a target: so append it in the queue.
	{
		this->InsertCmd(cmd);
	}
}

const CommandsQueue::cmdsQueue_t& CommandsQueue::GetQueueByType(const assets::xml_command_type wantedType) const
{
	if (wantedType == assets::xml_command_type::Technology)
		return this->GetTechnologiesCommands();
	else if (wantedType == assets::xml_command_type::Training)
		return this->GetTrainingsCommands();
	else
		return this->GetOtherCommands();
}

void CommandsQueue::RemoveElement(const assets::xml_command_type wantedType, uint32_t pos)
{
	assert(pos < this->pendingCommands[static_cast<assets::xmlCommandTypeInt>(wantedType)].size());

	uint32_t nDeletion = 1;
	if (rattlesmake::peripherals::keyboard::get_instance().IsKeyPressed(GLFW_KEY_LEFT_CONTROL) == true)
		nDeletion = 5;
	else if (rattlesmake::peripherals::keyboard::get_instance().IsKeyPressed(GLFW_KEY_LEFT_SHIFT) == true)
		nDeletion = this->pendingCommands[static_cast<assets::xmlCommandTypeInt>(wantedType)].at(pos).counter;
	else if (rattlesmake::peripherals::keyboard::get_instance().IsKeyPressed(GLFW_KEY_LEFT_ALT) == true)
	{
		// Remove all elements from the wanted queue.
		this->ClearAllByIndex(static_cast<uint8_t>(wantedType), 100);
		return;  // Deque already cleared entirely.
	}
	this->Remove(static_cast<uint8_t>(wantedType), pos, nDeletion, 100);
}

void CommandsQueue::ResetCmdWaitingForTarget(void)
{
	if (this->cmdWaitingForTarget.expired() == false)
	{
		this->SetCommandWaitingForTarget(*this->owner.lock(), std::weak_ptr<Command>());  // Before set a new command waiting for a target, reset old one.
	}
}

void CommandsQueue::Reset(uint8_t returnedResourcesPerc)
{
	if (returnedResourcesPerc > 100)
		returnedResourcesPerc = 100;

	this->ResetCmdWaitingForTarget();
	for (uint8_t index = 0; index < 3; ++index)
	{
		this->ClearAllByIndex(index, returnedResourcesPerc);
	}
}

void CommandsQueue::ExecuteReadyCommands(void)
{
	static Time& gameTimeRef = Engine::GetInstance().GetGameTime();

	this->TryToCatchTarget();

	std::shared_ptr<Playable> objSP = this->owner.lock();
	const auto& methodsMap = CommandsQueue::classesDataSP->get_xml_class(objSP->GetClassNameCRef())->get_unrepeated_methods_map_cref();

	for (assets::xmlCommandTypeInt index = 0; index < COMMANDS_QUEUE_NUMBER; ++index)
	{
		if (this->pendingCommands[index].empty() == false)
		{
			auto& firstCmd = this->pendingCommands[index].front();
			assert(firstCmd.cmd.expired() == false);
			const std::shared_ptr<Command> cmdToExecute = firstCmd.cmd.lock();

			const auto elapsedSeconds = gameTimeRef.GetTotalSeconds();

			// Store the start instant of the command that is being processed.
			// Obviously only when there is no command that is already being processed.
			if (this->cmdsBeingProcessed[index].empty() == true)
				this->cmdsBeingProcessed[index].startInstantOfProcessing = static_cast<float>(elapsedSeconds);

			// Calculates the current state of command processing
			this->cmdsBeingProcessed[index].currentInstantOfProcessing = static_cast<float>(elapsedSeconds);

			// Set the completion percentage (this info is used in topbar.cpp)
			firstCmd.completionPerc = (static_cast<float>(elapsedSeconds) - this->cmdsBeingProcessed[index].startInstantOfProcessing) / cmdToExecute->GetDelay();

			const bool cmdIsReadyForExecution = this->cmdsBeingProcessed[index].currentInstantOfProcessing >= this->cmdsBeingProcessed[index].startInstantOfProcessing + cmdToExecute->GetDelay();
			if (cmdIsReadyForExecution == true) 
			{
				// Get script to execute
				auto scriptSP = methodsMap.at(cmdToExecute->GetMethodCRef())->get_associated_script();

				// Update current queue
				if (firstCmd.counter >= 2)
					firstCmd.counter -= 1;  // Decrease counter
				else
					this->pendingCommands[index].pop_front(); //Remove element from the queue

				// Script execution...
				// We are indicating to not execute the condition (first argument equal to false) 
				// because it was already checked when the command was put in the queue
				objSP->ExecuteScript(false, cmdToExecute, scriptSP);

				// Command processing has been completed
				this->cmdsBeingProcessed[index].reset();
			}
		}
	}
}

void CommandsQueue::GetBinRepresentation(std::vector<byte_t>& data)
{
	/// The saving order here must be equals to the loading order in CommandsQueue::InitByBinData

	// For each commands queue
	for (assets::xmlCommandTypeInt index = 0; index < COMMANDS_QUEUE_NUMBER; index++)
	{
		// Firstly save queue size
		BinaryDataInterpreter::PushUInt32(data, static_cast<uint32_t>(this->pendingCommands[index].size()));

		// Then, save each pending command of the queue
		for (auto const& pendingCmd : this->pendingCommands[index])
		{
			BinaryDataInterpreter::PushString(data, pendingCmd.cmd.lock()->GetIdCRef());
			BinaryDataInterpreter::PushUInt32(data, pendingCmd.counter);
		}

		// Finally save info about the command that is being processed
		BinaryDataInterpreter::PushFloat(data, this->cmdsBeingProcessed[index].startInstantOfProcessing);
		BinaryDataInterpreter::PushFloat(data, this->cmdsBeingProcessed[index].currentInstantOfProcessing);
	}
}
#pragma endregion

#pragma region Private members:
void CommandsQueue::SetCommandWaitingForTarget(const Playable& obj, const std::weak_ptr<Command>& cmd)
{
	const bool bMethodNotEmpty = (cmd.expired() == false);
	if (bMethodNotEmpty == true)
		obj.GetPlayerRef()->GetObjsCollection()->AddGObjWaitingForATarget(obj.GetUniqueID());
	else
		obj.GetPlayerRef()->GetObjsCollection()->RemoveGObjWaitingForATarget(obj.GetUniqueID());

	// Set a command that is waiting for a target
	this->cmdWaitingForTarget = cmd;
}

void CommandsQueue::InsertCmd(const std::shared_ptr<Command>& cmd)
{
	std::shared_ptr<Playable> objSP = this->owner.lock();
	if (objSP->CheckCommandCondition(cmd->GetIdCRef()) == false)
		return;  // Command can't be inserted since condition can be satisfied

	// Try to provide resources
	this->HandleCommandResources((*cmd), (*objSP), true, 100);

	// Get cmd type
	const assets::xmlCommandTypeInt cmdType = static_cast<assets::xmlCommandTypeInt>(cmd->GetType());

	if (this->pendingCommands[cmdType].empty() == false)
	{
		// Get the last command inserted
		auto& lastCmd = this->pendingCommands[cmdType].back();

		if (lastCmd.cmd.lock()->GetIdCRef() == cmd->GetIdCRef()) //If the command to insert is the same of the last inserted
			lastCmd.counter += 1; //Increment the counter
		else  // Else, add a new element
			this->pendingCommands[cmdType].push_back({ cmd, 1 });
	}
	else
		this->pendingCommands[cmdType].push_back({ cmd, 1 });
}

void CommandsQueue::TryToCatchTarget(void)
{
	// There is any command waiting for a target
	if (this->cmdWaitingForTarget.expired() == true)
		return;

	std::shared_ptr<Playable> objSP = this->owner.lock();
	auto targetSP = objSP->GetPlayerRef()->GetObjsCollection()->TryToGetCapturedTarget(false);
	if (targetSP)  // Founded a target.
	{
		// Set the target with the captured target.
		objSP->SetTarget(targetSP);

		this->InsertCmd(this->cmdWaitingForTarget.lock());

		// Reset command waiting for a target.
		this->SetCommandWaitingForTarget((*objSP), std::weak_ptr<Command>());
	}
}

const CommandsQueue::cmdsQueue_t& CommandsQueue::GetOtherCommands(void) const
{
	return this->pendingCommands[static_cast<assets::xmlCommandTypeInt>(assets::xml_command_type::Other)];
}

const CommandsQueue::cmdsQueue_t& CommandsQueue::GetTechnologiesCommands(void) const
{
	return this->pendingCommands[static_cast<assets::xmlCommandTypeInt>(assets::xml_command_type::Technology)];
}

const CommandsQueue::cmdsQueue_t& CommandsQueue::GetTrainingsCommands(void) const
{
	return this->pendingCommands[static_cast<assets::xmlCommandTypeInt>(assets::xml_command_type::Training)];
}

void CommandsQueue::Remove(const assets::xmlCommandTypeInt index, const size_t pos, uint32_t nRemoval, const uint8_t returnedResourcesPerc)
{
	cmdsQueue_t& cmdQueue = this->pendingCommands[index];
	auto& commandDequeEl = cmdQueue.at(pos);
	if (nRemoval > commandDequeEl.counter)
		nRemoval = commandDequeEl.counter;

	std::shared_ptr<Playable> objSP = this->owner.lock();
	std::shared_ptr<Command> cmdSP = cmdQueue.at(pos).cmd.lock();

	for (uint32_t i = 0; i < nRemoval; i++)
	{
		// Retrieve resources
		this->HandleCommandResources((*cmdSP), (*objSP), false, returnedResourcesPerc);
		if (commandDequeEl.counter >= 2)
			commandDequeEl.counter--;  // Decrease the counter of the element.
		else 
		{
			// Remove totally an element form the queue.
			cmdQueue.erase(cmdQueue.begin() + pos);
			// If the element removed wasn't the last in the queue and if the queue has already more than 1 element
			if (pos < cmdQueue.size() && cmdQueue.size() > 1)
			{
				// If the two elements adjacent to the one just removed are equals, then
				if ((*cmdQueue[pos].cmd.lock()) == (*cmdQueue[pos - 1].cmd.lock()))
				{
					//... merge them
					cmdQueue[pos - 1].counter += cmdQueue[pos].counter;
					cmdQueue.erase(cmdQueue.begin() + pos);
				}
			}
		}
	}

	// Reset command being processed if I'm revoing the first element of the queue identified by index.
	if (pos == 0)
		this->cmdsBeingProcessed[index].reset();
}

void CommandsQueue::ClearAllByIndex(const assets::xmlCommandTypeInt index, const uint8_t returnedResourcesPerc)
{
	// Clear the queue of pending commands given an index.
	// Until the queue is empty, remove the last element.
	// We are preferring the last element and not the first since, in a deque, remove the last element is less expensive.
	while (this->pendingCommands[index].empty() == false)
	{
		const uint32_t pos = static_cast<uint32_t>(this->pendingCommands[index].size() - 1);
		const uint32_t nDeletion = this->pendingCommands[index].at(pos).counter;
		this->Remove(index, pos, nDeletion, returnedResourcesPerc);
	}

	// Reset the progress of the command being processed.
	this->cmdsBeingProcessed[index].reset();
}

void CommandsQueue::InitByBinData(gobjBinData_t& binData)
{
	if (binData.first->size() == binData.second)
	{
		// ASSERTION: this must happen only if the loading unit is dead (i.e. has only to play its animation "death")
		assert(this->owner.lock()->IsUnit() && std::static_pointer_cast<Unit>(this->owner.lock())->IsDead() == true);
		return;  // CommandsQueue doesn't have data. 
	}

	/// The loading order here must be equals to the saving order in CommandsQueue::GetBinRepresentation

	// Get owner
	const std::shared_ptr<Playable> gobj = this->owner.lock();

	// For each commands queue
	for (assets::xmlCommandTypeInt index = 0; index < COMMANDS_QUEUE_NUMBER; index++)
	{
		// Firstly load queue size
		const uint32_t queueSize = BinaryDataInterpreter::ExtractUInt32((*binData.first), binData.second);

		// Then, load each pending command and insert it in the queue
		for (uint32_t j = 0; j < queueSize; j++)
		{
			std::string cmdId = BinaryDataInterpreter::ExtractString((*binData.first), binData.second);
			std::shared_ptr<Command> cmd = gobj->GetCommandById(cmdId);
			if (cmd->IsValid() == false)
				throw BinaryDeserializerException("CommandsQueue::InitByBinData --> GObject having ID " + std::to_string(gobj->GetUniqueID()) + "and class " + gobj->GetClassNameCRef() + " doesn't have command having id " + std::move(cmdId));
			const uint32_t counter = BinaryDataInterpreter::ExtractUInt32((*binData.first), binData.second);
			this->pendingCommands[index].push_back(CommandsDequeElement_s{ std::move(cmd), counter });
		}

		// Finally load info about the command that is being processed
		this->cmdsBeingProcessed[index].startInstantOfProcessing = BinaryDataInterpreter::ExtractFloat((*binData.first), binData.second);
		this->cmdsBeingProcessed[index].currentInstantOfProcessing = BinaryDataInterpreter::ExtractFloat((*binData.first), binData.second);
	}
}
#pragma endregion
