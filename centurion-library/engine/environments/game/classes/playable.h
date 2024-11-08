/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#include <environments/game/classes/gobject.h>
#include <environments/game/classes/objectsAttributes/playableAttributes.h>

namespace centurion
{
	namespace assets
	{
		class xml_class_method;
		class xml_script;
	}
}

class PyInterpreter;

#pragma once
class Playable : public GObject, public PlayableAttributes
{
public:
	Playable(const Playable& other) = delete;
	virtual ~Playable(void);
	Playable& operator=(const Playable& other) = delete;
	void FinalizeCreation(const classData_t& _objData, gobjData_t* dataSource, const std::shared_ptr<Playable>& ref);


	#pragma region Static members:
	[[nodiscard]] static std::string ExecuteValueScript(const assets::xmlValueSP_t& valueSP);
	#pragma endregion


	#pragma region To scripts members:
	bool Deselect(const uint8_t _playerID) override;

	/// <summary>
	/// This function return the dislayed name of the current object.
	/// </summary>
	/// <returns>The displayed name of the current object; if the current object doesn't have a name, its singular name will be returned.</returns>
	[[nodiscard]] std::string GetDisplayedName(void) const;

	/// <summary>
	/// This function sets the displayed name of the current object. 
	/// </summary>
	/// <param name="_displayedName">The displayed name of the current object. </param>
	virtual void SetDisplayedName(std::string _displayedName);

	/// <summary>
	/// This function returns the univocal identification name of the current object.
	/// </summary>
	/// <returns>The identification name of the current object; if the current object doesn't have an ID name, an empty string will be returned.</returns>
	[[nodiscard]] scriptIdName_t GetIDName(void) const noexcept;

	/// <summary>
	/// This function sets the identification name of the current object. If the script name is already used, it will be setted using an empty string.
	/// </summary>
	/// <param name="_IDName">The identification name of the current object. It must be univocal. </param>
	/// <returns>True if the ID name has been assigned; false otherwise.</returns>
	bool SetIDName(scriptIdName_t _idName);

	[[nodiscard]] const std::shared_ptr<Target>& GetMyTarget(void) const;

	[[nodiscard]] uint8_t GetPlayer(void) const;

	virtual bool SetPlayer(const uint8_t _playerID);

	[[nodiscard]] int GetDefaultIntAttributesValue(const std::string attributeName);

	[[nodiscard]] uint8_t GetNumberOfCommands(void) const;

	[[nodiscard]] std::shared_ptr<Command> GetCommandById(const std::string& id) const;

	[[nodiscard]] std::shared_ptr<Target> AsTarget(void) const;
	#pragma endregion

	void SetTarget(std::shared_ptr<Target> _target);
	void SetMyTarget(const std::weak_ptr<Playable> tg, const std::shared_ptr<Point> pt = nullptr);  // DO NOT USE REFERENCES!

	[[nodiscard]] std::shared_ptr<Player> GetPlayerRef(void) const;
	[[nodiscard]] const valuesList_t& GetValues(void) const;

	[[nodiscard]] std::shared_ptr<CommandsQueue> GetCommandsQueue(void) const;
	virtual void AddCommandToExecute(const std::string& commandID);
	[[nodiscard]] bool HasMethod(const std::string& method) const noexcept;
	virtual void ExecuteMethod(const std::string& method);
	bool CheckCommandCondition(const std::string& commandID) const;
	[[nodiscard]] bool HasCommand(const string& id) const noexcept;
	[[nodiscard]] const std::vector<std::shared_ptr<Command>>& GetCommandsVecCRef(void) const;
	[[nodiscard]] std::shared_ptr<Command> GetCommand(const uint8_t index) const;
	virtual bool SetCommandWithTarget(const std::string& command, const std::shared_ptr<Target>& targetObj) = 0;
	bool ExecuteScript(const bool bCheckCondition, const std::shared_ptr<Command>& cmdSP, const std::shared_ptr<assets::xml_script>& scriptToRunSP) const;
	[[nodiscard]] bool ExecuteScriptCondition(const bool bSkipBinding, const std::shared_ptr<Command>& cmdSP, const std::shared_ptr<assets::xml_script>& scriptToRunSP) const;
protected:
	Playable(const uint8_t _player, glm::vec3&& _pos, const bool _bTemporary, const classData_t& _objData, const std::shared_ptr<Surface>& surface, gobjData_t* dataSource);

	virtual void GetBinRepresentation(std::vector<byte_t>& data, const bool calledByChild) const;
	virtual std::ostream& Serialize(std::ostream& out, const bool calledByChild) const;
	virtual void SetAttrs(const classData_t& objData, gobjData_t* dataSource);

	virtual void SetObjectCommands(const classData_t& objData) = 0;

	const defaultCommandsList_t* defaultCommands{ nullptr };
	std::unordered_map<std::string, std::shared_ptr<Command>> commandsMapId;
	std::vector<std::shared_ptr<Command>> commandsVec;
	std::shared_ptr<CommandsQueue> commandsQueue;
	typedef const std::unordered_map<std::string, std::shared_ptr<centurion::assets::xml_class_method>>& methods_t;
	methods_t methodsMap;
	methods_t repeatedMethodsMap;

	static PyInterpreter& pyInterpreter;
private:
	static void Binding(const std::shared_ptr<Playable>& objSP, const std::shared_ptr<Command>& cmdSP);

	/// <summary>
	/// This functions sets the default commands of the current object when it is being created. 
	/// The values of these attributes were previously read 
	/// from an XML file and they are now stored into a ClassesData::XMLClassData object.
	/// </summary>
	/// <param name="objData">The object in which are stored the values of the current object.</param>
	void SetPlayableGObjectDefaultCommands(const classData_t& objData);

	/// <summary>
	/// This functions sets the values of the current object when it is being created. 
	/// The values of these attributes were previously read 
	/// from an XML file and they are now stored into a ClassesData::XMLClassData object.
	/// </summary>
	/// <param name="objData">The object in which are stored the values of the current object.</param>
	void SetPlayableGObjectValues(const classData_t& objData);

	std::string displayedName;
	std::string idName;
	uint8_t playerID = 0;
	std::shared_ptr<Target> target;

	const valuesList_t* values{ nullptr };

	// This flag indicates which attributes are being set during a loading (both from XML and from binary file).
	// If true, an exception will be thrown, when a set method is invoked, if the value to be set is not valid.
	// This indicates an error in the file.
	bool bLoading = false;
};
