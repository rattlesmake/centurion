#include <environments/game/classes/playable.h>
#include <dialogWindows.h>
#include <services/sqlservice.h>
#include <services/pyservice.h>
#include <bin_data_interpreter.h>

#include <xml_class.h>
#include <xml_class_method.h>
#include <xml_script.h>
#include <xml_value.h>


#pragma region Static attributes initialization:
PyInterpreter& Playable::pyInterpreter = PyService::GetInstance().GetInterpreter(PySecurityLevel::Game);
#pragma endregion


#pragma region Constructor and destructor
Playable::Playable(const uint8_t _player, glm::vec3&& _pos, const bool _bTemporary, const classData_t& _objData, const std::shared_ptr<Surface>& surface, gobjData_t* dataSource) :
	GObject(std::move(_pos), _bTemporary, _objData, surface, dataSource), playerID(_player),
	methodsMap(_objData->get_unrepeated_methods_map_cref()), repeatedMethodsMap(_objData->get_repeated_methods_map_cref())
{
	// Create an empty target 
	this->target = Target::CreateTarget();
}

Playable::~Playable(void)
{
}
#pragma endregion


#pragma region Static public members:
std::string Playable::ExecuteValueScript(const assets::xmlValueSP_t& valueSP)
{
	assert(valueSP);

	static const std::string valuesArgs{ "({'this':" + OBJECT_BINDING_PYTHON + ",})" };

	auto environment = Engine::GetInstance().GetEnvironment()->AsMatch();

	// Bind selected objects
	if (environment->GetNumberOfSelectedObjects() > 1)
	{
		std::shared_ptr<SelectedObjects> multiSel = environment->Selection();
		assert(multiSel);
		Playable::pyInterpreter.BindSharedPtr(OBJECT_BINDING_PYTHON, PyInterpreter::PyBindedSPTypes::MultiSelection, multiSel);
	}
	else  // == 1
	{
		std::shared_ptr<Playable> selp = environment->Selp();
		assert(selp);
		Playable::pyInterpreter.BindSharedPtr(OBJECT_BINDING_PYTHON, PyInterpreter::PyBindedSPTypes::GObject, selp);
	}
	std::string output;
	Playable::pyInterpreter.Evaluate(valueSP->get_function_cref() + valuesArgs, PyInterpreter::PyEvaluationTypes::String, &output);

	// Unbind
	Playable::pyInterpreter.BindSharedPtr(OBJECT_BINDING_PYTHON, PyInterpreter::PyBindedSPTypes::GObject, nullptr);

	return output;
}
#pragma endregion



void Playable::FinalizeCreation(const classData_t& _objData, gobjData_t* dataSource, const std::shared_ptr<Playable>& ref)
{
	// Attributes, commands, values and default commands
	// Methods reference are catch directly in constructor.
	if ((Engine::GetInstance().GetEnvironmentId() == IEnvironment::Environments::e_match) == true)
	{
		this->SetObjectCommands(_objData);
		this->SetPlayableGObjectDefaultCommands(_objData);
		this->SetPlayableGObjectValues(_objData);
	}

	// Read attributes always as last thing
	this->SetAttrs(_objData, dataSource);

	// Finally, create commands queue
	this->commandsQueue = CommandsQueue::Create(std::static_pointer_cast<Playable>(this->me.lock()), dataSource);
}

bool Playable::Deselect(const uint8_t _playerID)
{
	// Call parent method to deselect GObject
	const bool bDeselected = this->GObject::Deselect(_playerID);
	if (bDeselected == true)  // When a PlayableGObject is deselected, if it has some command waiting for a target, then it's important to reset it.
		this->commandsQueue->ResetCmdWaitingForTarget();
	return bDeselected;
}

#pragma region To scripts members:
std::string Playable::GetDisplayedName(void) const
{
	const std::string& singularName = this->GetSingularNameCRef();
	if (singularName.empty() == true && this->displayedName.empty() == true)
		return SqlService::GetInstance().GetTranslation("object", true);
	if (this->displayedName != "" && this->displayedName != singularName)
		return SqlService::GetInstance().GetTranslation(this->displayedName, true);
	return SqlService::GetInstance().GetTranslation(singularName, true);
}

void Playable::SetDisplayedName(std::string _displayedName)
{
	std::trim(_displayedName);
	this->displayedName = std::move(_displayedName);
}

scriptIdName_t Playable::GetIDName(void) const noexcept
{
	return this->idName;
}

bool Playable::SetIDName(scriptIdName_t _idName)
{
	std::trim(_idName);
	if (this->idName == _idName)
		return true;

	const std::shared_ptr<PlayersArray> playersArray = GObject::playersWeakRef.lock();
	assert(playersArray);
	assert(this->me.lock());

	// Try to assign a new script ID name and, in case, reset the previous one 
	const bool bOk = playersArray->UpdateGObjectScriptId(std::static_pointer_cast<Playable>(this->me.lock()), _idName);

	// Checks if a name belongs to an other existing GObject.
	if (bOk == false)
	{
		if (this->bLoading == true)
			throw BinaryDeserializerException("Playable::SetIDName -> duplicated script ID name " + std::move(_idName)); //Playable gobject script id name READ FROM FILE is not valid!
		gui::NewInfoWindow("e_text_id_name_already_used", IEnvironment::Environments::e_editor);
		return false;
	}

	// Assigning name 
	this->idName = std::move(_idName);

	// ID name was assigned
	return true;
}

const std::shared_ptr<Target>& Playable::GetMyTarget(void) const
{
	return this->target;
}

uint8_t Playable::GetPlayer(void) const
{
	return this->playerID;
}

bool Playable::SetPlayer(const uint8_t _playerID)
{
	if (this->playerID == _playerID || _playerID == PlayersArray::UNDEFINED_PLAYER_INDEX || _playerID > PlayersArray::NUMBER_OF_PLAYERS)
		return false;
	this->commandsQueue->ResetCmdWaitingForTarget();
	this->playerID = _playerID;
	return true;
}

int Playable::GetDefaultIntAttributesValue(const std::string attributeName)
{
	
	if (!this->xmlData)
		return -2;  // Class doesn't exist
	std::optional<int> iAttribute = assets::xml_class::try_parse_integer(this->xmlData->get_attributes(), attributeName);
	return (iAttribute.has_value() ? iAttribute.value() : -1);  // -1 means that attribute doesn't exist
}

uint8_t Playable::GetNumberOfCommands(void) const
{
	return static_cast<uint8_t>(commandsVec.size());
}

std::shared_ptr<Command> Playable::GetCommandById(const std::string& id) const
{
	return this->commandsMapId.contains(id) == true ? this->commandsMapId.at(id) : Command::GetInvalidCommand();
}

std::shared_ptr<Target> Playable::AsTarget(void) const
{
	return Target::CreateTarget(std::static_pointer_cast<Playable>(this->me.lock()), this->GetPosition());
}
#pragma endregion

void Playable::SetTarget(std::shared_ptr<Target> _target)
{
	this->target = std::move(_target);
}

void Playable::SetMyTarget(const std::weak_ptr<Playable> tg, const std::shared_ptr<Point> pt)
{
	this->target->Set(tg, pt);
}

std::shared_ptr<Player> Playable::GetPlayerRef(void) const
{
	const auto players = GObject::playersWeakRef.lock();
	assert(players);
	const auto player = players->GetPlayerRef(this->playerID).lock();
	assert(player);
	return player;
}

const valuesList_t& Playable::GetValues(void) const
{
	assert(this->values != nullptr);
	return (*this->values);
}

std::shared_ptr<CommandsQueue> Playable::GetCommandsQueue(void) const
{
	return this->commandsQueue;
}

void Playable::AddCommandToExecute(const std::string& commandID)
{
	// Check if the object has the command.
	if (this->HasCommand(commandID) == false)
		return;

	const std::shared_ptr<Command>& commandSP = this->commandsMapId.at(commandID);
	this->commandsQueue->Push(commandSP);
}

bool Playable::HasCommand(const string& id) const noexcept
{
	return this->commandsMapId.contains(id);
}

const std::vector<std::shared_ptr<Command>>& Playable::GetCommandsVecCRef(void) const
{
	return this->commandsVec;
}

std::shared_ptr<Command> Playable::GetCommand(const uint8_t index) const
{
	return (index < this->commandsVec.size()) ? this->commandsVec.at(index) : Command::GetInvalidCommand();
}

bool Playable::HasMethod(const std::string& method) const noexcept
{
	return this->methodsMap.contains(method);
}

void Playable::ExecuteMethod(const std::string& method)
{
	if (this->HasMethod(method) == false)
		return;
	const std::shared_ptr<centurion::assets::xml_script> scriptToRunSP = this->methodsMap.at(method)->get_associated_script();
	assert(scriptToRunSP && scriptToRunSP->is_autoexecutable() == false);
	this->ExecuteScript(true, nullptr, scriptToRunSP);
}

bool Playable::CheckCommandCondition(const std::string& commandID) const
{
	assert(this->commandsMapId.contains(commandID));
	const std::string& associatedMethodID = this->commandsMapId.at(commandID)->GetMethodCRef();
	assert(this->methodsMap.contains(associatedMethodID));
	const std::shared_ptr<centurion::assets::xml_script> scriptToCheckSP = this->methodsMap.at(associatedMethodID)->get_associated_script();
	assert(scriptToCheckSP && scriptToCheckSP->is_autoexecutable() == false);
	return this->ExecuteScriptCondition(false, this->commandsMapId.at(commandID), scriptToCheckSP);
}

bool Playable::ExecuteScript(const bool bCheckCondition, const std::shared_ptr<Command>& cmdSP, const std::shared_ptr<assets::xml_script>& scriptToRunSP) const
{
	assert(this->me.lock() && scriptToRunSP);
	assert(cmdSP && cmdSP->IsValid() == true || !cmdSP);
	bool bResult = true;
	// Bind variables
	Playable::Binding(std::static_pointer_cast<Playable>(this->me.lock()), cmdSP);

	bool bExecuteBody = true;
	// Test condition
	if (bCheckCondition == true)
		bResult = bExecuteBody = this->ExecuteScriptCondition(true, cmdSP, scriptToRunSP);

	// Evaluate body if condition script was ok
	if (bExecuteBody == true)
	{
		if (cmdSP)
			Playable::pyInterpreter.Evaluate(scriptToRunSP->get_id_cref() + "_body" + scriptToRunSP->get_arguments(cmdSP->GetParamsList()));
		else
			Playable::pyInterpreter.Evaluate(scriptToRunSP->get_id_cref() + "_body" + scriptToRunSP->get_arguments(assets::providedCmdParams_t()));
	}

	// Unbind variables
	Playable::Binding(nullptr, nullptr);

	return bResult;
}

bool Playable::ExecuteScriptCondition(const bool bSkipBinding, const std::shared_ptr<Command>& cmdSP, const std::shared_ptr<assets::xml_script>& scriptToRunSP) const
{
	assert(this->me.lock() && scriptToRunSP);
	assert(cmdSP && cmdSP->IsValid() == true || !cmdSP);

	// Bind variables
	if (bSkipBinding != true)
		Playable::Binding(std::static_pointer_cast<Playable>(this->me.lock()), cmdSP);

	// Evaluate condition
	bool bConditionResult = false;

	if (cmdSP)
		Playable::pyInterpreter.Evaluate(scriptToRunSP->get_id_cref() + "_condition" + scriptToRunSP->get_arguments(cmdSP->GetParamsList()), PyInterpreter::PyEvaluationTypes::Boolean, &bConditionResult);
	else
		Playable::pyInterpreter.Evaluate(scriptToRunSP->get_id_cref() + "_condition" + scriptToRunSP->get_arguments(assets::providedCmdParams_t()), PyInterpreter::PyEvaluationTypes::Boolean, &bConditionResult);

	// Unbind variables
	if (bSkipBinding != true)
		Playable::Binding(nullptr, nullptr);

	return bConditionResult;
}


#pragma region Protected members:
void Playable::GetBinRepresentation(std::vector<byte_t>& data, const bool calledByChild) const
{
	this->GObject::GetBinRepresentation(data, calledByChild);

	// If you change saving order here, please go to Playable::SetAttrs and impose the same loading order!
	BinaryDataInterpreter::PushString(data, this->idName);
	BinaryDataInterpreter::PushString(data, this->displayedName);

	// Save pending commands queue
	this->commandsQueue->GetBinRepresentation(data);
}

std::ostream& Playable::Serialize(std::ostream& out, const bool calledByChild) const
{
	const std::string tabs(GObject::nTabs, '\t');
	out << tabs << "\t<displayedName>" << this->displayedName << "</displayedName>\n";
	out << tabs << "\t<idName>" << this->idName << "</idName>\n";
	return out;
}

void Playable::SetAttrs(const classData_t& objData, gobjData_t* dataSource)
{
	// As first thing, calls GObject::SetAttrs
	this->GObject::SetAttrs(objData, dataSource);
	this->PlayableAttributes::SetAttributes(objData, dataSource, this->bIsTemporary);

	this->bLoading = (dataSource != nullptr);  // If true, PlayableGobject is being loaded from a file otherwise it's being created manually (e.g. from a barracks or placing it in the editor).

	if (dataSource == nullptr)
		this->displayedName = this->PlayableAttributes::GetSingularNameCRef();  // When a Playable GObject is created, displayed name is equal to its singular name
	else if (std::holds_alternative<tinyxml2::XMLElement*>(*dataSource) == true)
	{
		tinyxml2::XMLElement* xml = std::get<tinyxml2::XMLElement*>(*dataSource);
		this->SetIDName(tinyxml2::TryParseFirstChildStrContent(xml, "idName"));
		this->SetDisplayedName(tinyxml2::TryParseFirstChildStrContent(xml, "displayedName"));
	}
	else if (std::holds_alternative<gobjBinData_t>(*dataSource) == true)
	{
		gobjBinData_t& binData = std::get<gobjBinData_t>(*dataSource);
		// If you change loading order here, please go to Playable::GetBinRepresentation and impose the same saving order!
		this->SetIDName(BinaryDataInterpreter::ExtractString((*binData.first), binData.second));
		this->SetDisplayedName(BinaryDataInterpreter::ExtractString((*binData.first), binData.second));
	}

	// Always equals to false, at the end of this method
	this->bLoading = false;
}
#pragma endregion


#pragma region Private static members:
void Playable::Binding(const std::shared_ptr<Playable>& objSP, const std::shared_ptr<Command>& cmdSP)
{
	Playable::pyInterpreter.BindSharedPtr(OBJECT_BINDING_PYTHON, PyInterpreter::PyBindedSPTypes::GObject, objSP);
	Playable::pyInterpreter.BindSharedPtr(COMMAND_BINDING_PYTHON, PyInterpreter::PyBindedSPTypes::Command, cmdSP);
}
#pragma endregion



#pragma region Private members:
void Playable::SetPlayableGObjectDefaultCommands(const classData_t& objData)
{
	this->defaultCommands = &objData->get_default_commands_list_cref();
}

void Playable::SetPlayableGObjectValues(const classData_t& objData)
{
	this->values = &(objData->get_values_vec_cref());
}
#pragma endregion
