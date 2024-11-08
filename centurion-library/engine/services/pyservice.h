/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <environments/ienvironment.h>
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>


const std::string OBJECT_BINDING_PYTHON{ "__o__" };
const std::string COMMAND_BINDING_PYTHON{ "__cmd__" };


class PyService;
class Settings;

namespace rattlesmake 
{ 
	namespace peripherals 
	{ 
		class viewport; 
		class keyboard;
	}; 
};

namespace pybind11 { class scoped_interpreter; };

enum class PySecurityLevel
{
	Admin = 0,
	Game = 1
};

class PyInterpreter
{
	friend class PyService;

public:
	enum class PyEvaluationTypes
	{
		Undefined = -1,

		Void = 0,
		String = 1,
		Boolean = 2,
		Integer = 3,
		Float = 4,
		FloatVector = 5,
	};

	enum class PyBindedTypes
	{
		Undefined = -1,

		String,
		Boolean,
		Integer,
		Float,

		Iframe,
		Engine,
	};

	enum class PyBindedSPTypes
	{
		GObject, //We don't need subclasses since pybind provides inheritance.
		Settlement,
		Target,
		ObjsList, 
		MultiSelection, //Used for multiple selection (selos).
		Command,
	};

	PyInterpreter(const PyInterpreter& other) = delete;
	PyInterpreter& operator=(const PyInterpreter& other) = delete;
	~PyInterpreter(void);

	void Evaluate(const std::string& command, PyEvaluationTypes type = PyEvaluationTypes::Void, void* data = 0);
	void Bind(std::string&& name, PyBindedTypes type, void* data);
	void BindSharedPtr(const std::string& name, PyBindedSPTypes type, const std::shared_ptr<void>& data);
private:
	PyInterpreter(void) = delete;
	explicit PyInterpreter(PySecurityLevel _securityLevel);
	PySecurityLevel securityLevel;
	void SetSecurityLevel(void);
};



// PyFunction (for Object Methods)
class PyFunction
{
public:

	struct PyParameter
	{
		enum class Source
		{
			Undefined = -1,
			Command = 0,
			Engine = 1,
		};
		enum class Type
		{
			Undefined = -1,
			GObject = 0, //We don't need subclasses since pybind provides inheritance.
			Target = 1,
			Command = 2,
			String = 3,
		};

		std::string name;
		std::string value;
		Type type = Type::Undefined;
		Source source = Source::Undefined;
	};
	[[nodiscard]] static PyParameter::Type GetParamType(std::string typeStr, std::string name);
	[[nodiscard]] static std::string GetParamValueAsString(PyParameter::Type type, std::string& commandName);

	PyFunction(PyInterpreter::PyEvaluationTypes _type, std::string _name, std::vector<PyParameter>& paramsWithoutValues);

	void Execute(const std::string& functionExecutionString, void* data = 0);
	[[nodiscard]] std::string GetFunctionSignature(std::vector<PyParameter>& paramsWithValues);
	[[nodiscard]] bool IsUndefined(void) const;
	[[nodiscard]] std::string GetName(void) const;

private:
	PyInterpreter::PyEvaluationTypes type = PyInterpreter::PyEvaluationTypes::Undefined;
	std::string functionSignature;
	std::string name;

	PyInterpreter& py;
};
typedef PyFunction::PyParameter PyParam;
typedef std::vector<PyParam> PyParams;





// Service that wraps PyBind11 library for our purposes
class PyService
{
public:
	enum class GObjectScriptsArgsTypes
	{
		e_values = 1,
	};
	~PyService(void);

	#pragma region Singleton
	///Copy constructor and operator = are not allowed in order to avoid accidentally copies of the singleton appearing.
	PyService(const PyService& other) = delete;
	PyService& operator=(const PyService& other) = delete;
	static PyService& GetInstance(void);
	#pragma endregion

	[[nodiscard]] PyInterpreter& GetInterpreter(PySecurityLevel _securityLevel);
	[[nodiscard]] bool CurrentInterpreterIsAdmin(void) const;
	[[nodiscard]] bool CurrentInterpreterIsGame(void) const;
	[[nodiscard]] std::string GetArgs(IEnvironment::Environments env) const;
private:
	PyService(void);

	void SetSecurityLevel(PySecurityLevel _securityLevel);

	Settings& settings;
	PyInterpreter adminInterpreter;
	PyInterpreter gameInterpreter;

	PySecurityLevel currentSecurityLevel = PySecurityLevel::Admin;
	pybind11::scoped_interpreter* guard;

	std::unordered_map<IEnvironment::Environments, std::string> argsMap; // for ui functions

	static PyService instance;

	friend class PyInterpreter;
};

class PyConsole
{
public:
	#pragma region Singleton
	///Copy constructor and operator = are not allowed in order to avoid accidentally copies of the singleton appearing.
	PyConsole(const PyConsole& other) = delete;
	PyConsole& operator=(const PyConsole& other) = delete;
	static PyConsole& GetInstance(void);
	#pragma endregion

	void Render(void);
	void ExecuteCommand(void);
private:
	PyConsole(void);
	~PyConsole(void);

	PyInterpreter& py;
	rattlesmake::peripherals::keyboard& keyboard;
	rattlesmake::peripherals::viewport& viewport;

	bool bIsActive = false;
	std::string commandToExecute;
	const char* inputLabel = "##";
	char inputBuf[256]{ '\0' };

	static PyConsole instance;
};
