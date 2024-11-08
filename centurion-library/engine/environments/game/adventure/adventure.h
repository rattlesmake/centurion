/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/
#pragma once

#ifndef MAX_NUMBER_OF_SCENARIOS
#define MAX_NUMBER_OF_SCENARIOS 20 
#endif

#ifndef MAX_NUMBER_OF_ADV_OBJECTIVES
#define MAX_NUMBER_OF_ADV_OBJECTIVES 100 
#endif

#include <header.h>
#include <centurion_typedef.hpp>
#include <environments/game/adventure/scenario/scenario.h>
#include <binary_file_reader.h>
#include <binary_file_writer.h>

namespace rattlesmake { namespace services { class zip_service; }; };
class Settings;
class SurfaceSettings;

// questa classe contiene al massimo 20 scenari.
// deve contenere alcune proprieta tipiche delle avventure, partiamo giusto con il nome;
// l'array di scenari puo contenere Scenario oppure Scenario* --> da decidere quale è meglio
// deve contenere alcune funzioni:
// -- Initialize, che crea uno scenario vuoto e lo posiziona nella posizione 0 dell array
// -- Render, che renderizza lo scenario corrente
// -- Save e Load, che salvano e caricano tutti gli scenari e le varie proprietà dell'adventure (un xml)
// -- GetCurrentScenario, che restituisce un puntatore all'elemento dell'array --> per questo forse conviene che sia un array di ptr
// -- IsEdited, per stabilire se la mappa presenta modifiche e quindi va richiesto il salvataggio 
//    una qualunque modifica a qualunque scenario porta a rendere l'intera avventura "editata" e quindi 
//    con modifiche da salvare


//TODO - fare ordine 
class Adventure
{
	//In this way IGame and Editor can access to constructor and init methods
	friend class IGame;
	friend class Editor;
	friend class Match;
public:
	class Objective
	{
	public:
		Objective() {}
		explicit Objective(tinyxml2::XMLElement* el);

		std::string id, title, description, scenario, icon;
		bool minimap = false;
		int x = 0, y = 0;
	};

	class ObjectiveList
	{
	public:
		ObjectiveList() {}
		explicit ObjectiveList(tinyxml2::XMLElement* el);
		Objective* GetObjectiveById(std::string id);
		std::vector<std::string> GetListOfObjectivesId(void);
		bool IsObjectiveIdAvailable(std::string id, Objective* o = nullptr);
		void Clear();
		void RemoveObjectiveById(std::string id);
		void NewObjective();
		[[nodiscard]] std::stringstream Serialize(void) const;
		std::vector<Objective*> objectives;
	};

	class Properties
	{
	public:
		Properties() {}
		explicit Properties(tinyxml2::XMLElement* el);
		[[nodiscard]] std::stringstream Serialize(void) const;
		std::string adventureName;
		std::string author;
		std::string creationDate;
		std::string lastEditDate;
		std::string description;
		std::string startingScenario = "scenario01";
		uint8_t gameMode = 0;
		uint8_t playersNumber = 1;
	};

	Adventure(const Adventure& other) = delete;
	Adventure& operator=(const Adventure & other) = delete;
	~Adventure(void);

	static bool CheckIntegrity(const std::string& zipName);

	void NewScenario(std::string& scenarioName);
	void OpenScenario(std::string& scenarioName);
	void DeleteScenario(std::string& scenarioName);
	void GenerateRandomScenario(const uint32_t seed = 0);
	[[nodiscard]] std::vector<std::string> GetListOfValidScenariosWithDetails(void) const;
	[[nodiscard]] std::vector<std::string> GetListOfValidScenarios(void) const;

	/// <summary>
	/// This function loads the selected adventure's properties from a XML file.
	/// </summary>
	void LoadProperties(void);

	/// <summary>
	/// This function loads the selected adventure's objectives list from a XML file.
	/// </summary>
	void LoadObjectives(void);

	/// <summary>
	/// This function saves the current adventure and the respective content into a zip file.
	/// </summary>
	void SaveAll(void);

	void SaveAdventure(const bool closeZip = true);

	void SaveAsBinaryFile(BinaryFileWriter& bfw);

	/// <summary>
	/// This function sets the adventure's zip file name, ready to be saved. Special characters, symbols and spaces are not allowed.
	/// </summary>
	/// <param name="newName">Adventure's new name. It supports only strings.</param>
	void SetZipName(std::string newName);

	/// <summary>
	/// This function returns the adventure's zip file name.
	/// </summary>
	/// <returns>Adventure's zip file name. Extension is excluded from the string. STD::STRING</returns>
	[[nodiscard]] std::string GetZipName(void) const;

	/// <summary>
	/// This function returns the complete adventure's zip file path.
	/// </summary>
	/// <returns>Adventure's zip file path. Extension is included into the string. STD::STRING</returns>
	[[nodiscard]] std::string GetZipPath(void) const;

	/// <summary>
	/// This function sets the adventure's name between properties. All characters allowed.
	/// </summary>
	/// <param name="newName">Adventure's new name. It supports only strings.</param>
	void SetAdventureName(std::string newName);

	/// <summary>
	/// This function returns the adventure name from properties.
	/// </summary>
	/// <returns>Adventure's name from properties. STD::STRING</returns>
	[[nodiscard]] std::string GetAdventureName(void) const;

	/// <summary>
	/// This function sets the adventure's author between properties.
	/// </summary>
	/// <param name="newName">Adventure's author. It supports only strings.</param>
	void SetAuthor(std::string author);

	/// <summary>
	/// This function returns the adventure author from properties.
	/// </summary>
	/// <returns>Adventure's author from properties. STD::STRING</returns>
	[[nodiscard]] std::string GetAuthor(void) const;

	/// <summary>
	/// This function sets the adventure's creation date between properties.
	/// </summary>
	void SetCreationDate(void);

	/// <summary>
	/// This function returns the adventure creation date from properties.
	/// </summary>
	/// <returns>Adventure's creation date from properties. STD::STRING</returns>
	[[nodiscard]] std::string GetCreationDate(void) const;

	/// <summary>
	/// This function sets the adventure's last edit date between properties.
	/// </summary>
	void SetLastEditDate(void);

	/// <summary>
	/// This function returns the adventure last edit date from properties.
	/// </summary>
	/// <returns>Adventure's last edit date from properties. STD::STRING</returns>
	[[nodiscard]] std::string GetLastEditDate(void) const;

	/// <summary>
	/// This function sets the adventure's description between properties.
	/// </summary>
	/// <param name="newName">Adventure's description. It supports only strings.</param>
	void SetDescription(std::string description);

	/// <summary>
	/// This function returns the adventure description from properties.
	/// </summary>
	/// <returns>Adventure's description from properties. STD::STRING</returns>
	[[nodiscard]] std::string GetDescription(void) const;

	/// <summary>
	/// This function sets the adventure's default scenario between properties.
	/// </summary>
	/// <param name="newName">Adventure's default scenario. It supports only strings.</param>
	void SetStartingScenario(std::string scenario);

	/// <summary>
	/// This function returns the adventure default scenario from properties.
	/// </summary>
	/// <returns>Adventure's default scenario from properties. STD::STRING</returns>
	[[nodiscard]] std::string GetStartingScenario(void) const;

	/// <summary>
	/// This function sets the adventure's game mode between properties.
	/// </summary>
	/// <param name="newName">Adventure's game mode. It supports only uint8_t values.</param>
	void SetGameMode(uint8_t mode);

	/// <summary>
	/// This function returns the adventure game mode from properties.
	/// </summary>
	/// <returns>Adventure's game mode from properties. UINT8_T</returns>
	[[nodiscard]] uint8_t GetGameMode(void) const;

	/// <summary>
	/// This function sets the adventure's maximum allowed players number between properties.
	/// </summary>
	/// <param name="number">Adventure's players number. It supports only uint8_t values.</param>
	void SetPlayersNumber(uint8_t number);

	/// <summary>
	/// This function returns the adventure maximum allowed players number from properties.
	/// </summary>
	/// <returns>Adventure's players number from properties. UINT8_T</returns>
	[[nodiscard]] uint8_t GetPlayersNumber(void) const;

	/// <summary>
	/// This function marks the current adventure as "edited". 
	/// </summary>
	void MarkAsEdited(void);

	/// <summary>
	/// This function marks the current adventure as "non-edited". 
	/// </summary>
	void MarkAsNonEdited(void);

	/// <summary>
	/// This boolean function returns true if the current adventure has been edited, false otherwise.
	/// </summary>
	[[nodiscard]] bool IsEdited(void) const;

	[[nodiscard]] bool IsOpened(void) const;

	/// <summary>
	/// This method warns the currently used adventure cannot be deleted, opening an Infobox Window.
	/// </summary>
	void UnableToDelete(void);

	/// <summary>
	/// This function returns the adventure current scenario instance pointer.
	/// </summary>
	/// <returns>Adventure's current scenario instance pointer.</returns>
	[[nodiscard]] std::shared_ptr<Scenario> GetScenario(void) const;

	/// <summary>
	/// This function returns the adventure objectives list instance pointer.
	/// </summary>
	/// <returns>Adventure's objectives list instance pointer.</returns>
	ObjectiveList* GetObjectives(void);

	void ExecuteInternalMethods(void);

	[[nodiscard]] classesData_t GetClassesDataSp(void) const noexcept;
private:
	#pragma region Init methods and constructor:
	static void InitEmpty(std::shared_ptr<Adventure>& adventureToInitSP, std::string advName);
	static void InitRandom(std::shared_ptr<Adventure>& adventureToInitSP);
	static void InitFromZip(std::shared_ptr<Adventure>& adventureToInitSP, std::string zipName);
	static void InitFromBinFile(std::shared_ptr<Adventure>& adventureToInitSP, BinaryFileReader& bfr);

	// The only constructor. INIT METHODS HAVE TO PROPER FILL THE ADVENTURE!
	explicit Adventure(classesData_t classesDataSP);
	#pragma endregion

	static void NewScenario_Internal(const std::string& scenarioName, const bool saveBefore, const bool increaseScenarios);

	static void OpenScenario_Internal(const std::string& scenarioName, const bool saveBefore);
	struct OpenScenario_Internal_Data
	{
		std::string scenarioName;
		bool bSaveBefore = false;
		bool bActive = false;
	};
	static OpenScenario_Internal_Data sOpenScenario_Internal;

	static void DeleteScenario_Internal(const std::string& scenarioName, const bool openAnother);

	static void GenerateRandomScenario_Internal(const uint32_t seed);

	void DecreaseNumberOfScenarios(void);
	void IncreaseNumberOfScenarios(void);

	classesData_t classesDataSP;

	// reference lookups
	rattlesmake::services::zip_service& zipService;
	Settings& settings;
	SurfaceSettings& surfaceSettings;

	//Scenario pointer (even scenario has a weak_ptr to the adventure)
	std::shared_ptr<Scenario> scenario;

	Properties properties;
	std::string zipName;
	bool edited = false;
	ObjectiveList objectives;

	uint32_t numberOfScenarios = 1;
};
