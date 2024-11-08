/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/
#pragma once

#include <header.h>
#include <environments/game/adventure/scenario/surface/surface_grid/surface_grid.h>
#include <environments/game/adventure/scenario/surface/randommap/randomMapGenerator.h>
#include <environments/game/adventure/scenario/area.h>
#include <environments/game/adventure/scenario/area_array.h>

#include <binary_file_reader.h>
#include <binary_file_writer.h>

#include <players/players_array.h>

namespace rattlesmake { namespace services { class zip_service; }; };
class Surface;
class Terrain;

class Adventure;
class Minimap;

//TODO - fare ordine 
class Scenario
{
	friend class Adventure;
public:
	void SetCurrentAdv(const std::weak_ptr<Adventure>& curAdv);
	~Scenario(void);

	///Copy constructor and operator = are not allowed in order to avoid accidentally copies of the singleton appearing.
	Scenario(const Scenario& other) = delete;
	Scenario& operator=(Scenario const& other) = delete;

	[[nodiscard]] static bool CheckIntegrity(const std::string& zipKey, const std::string& scenarioFolderName);

	struct Properties
	{
		std::string fullName;
		std::string creationDate;
		std::string lastEditDate;
		std::string maxPitch;
		std::string mapSize;
	};

	[[nodiscard]] std::weak_ptr<Minimap> GetMinimap(void) const;
	[[nodiscard]] std::weak_ptr<PlayersArray> GetPlayersArrayW(void) const;
	[[nodiscard]] std::shared_ptr<PlayersArray> GetPlayersArray(void) const;
	[[nodiscard]] std::weak_ptr<Surface> GetSurfaceW(void) const;
	[[nodiscard]] std::shared_ptr<Surface> GetSurface(void) const;

	[[nodiscard]] std::shared_ptr<AreaArray> GetAreaArrayPtr(void) const;
	[[nodiscard]] std::weak_ptr<SelectionArea> GetSelectionAreaPtr(void) const;

	void EnableWireframe(void);
	void DisableWireframe(void);
	[[nodiscard]] bool IsWireframeActive(void) const;
	[[nodiscard]] bool IsSeaRenderingActive(void) const;
	
	void ToggleGrid(void);
	void ToggleHitboxRendering(void);
	void ToggleWireframe(void);
	void ToggleSea(void);
	void ToggleClouds(void);
	void ToggleTracingDebugging(void);
	void ToggleDrawWithoutNoise(void);
	[[nodiscard]] bool IsHitboxRenderingActive(void) const;
	[[nodiscard]] bool IsCloudRenderingActive(void) const;
	[[nodiscard]] bool IsTracingDebuggingActive(void) const;
	[[nodiscard]] bool IsDrawWithoutNoiseActive(void) const;
	float GetNoiseFromRGBColor(const float color);

	/// <summary>
	/// This function saves a scenario. 
	/// </summary>
	void SaveFromEditor(const bool saveZip = true);

	void SaveAsBinaryFile(BinaryFileWriter& bfw) const;

	/// <summary>
	/// This method exports the current scenario.
	/// </summary>
	/// <param name="path">The path of the file.</param>
	void Export(const std::string& path);

	/// <summary>
	/// This function sets the folder name of the scenario.
	/// </summary>
	/// <param name="name">The name of the folder</param>
	void SetFolderName(std::string name);

	/// <summary>
	/// This function returns the folder name of the scenario.
	/// </summary>
	/// <returns>The name of the folder</returns>
	[[nodiscard]] std::string GetFolderName(void) const;

	/// <summary>
	/// This function sets the name of the scenario between its property.
	/// </summary>
	/// <param name="name">The name of the scenario</param>
	void SetName(std::string name);

	/// <summary>
	/// This function returns the name of the scenario.
	/// </summary>
	/// <returns>The name of the scenario</returns>
	[[nodiscard]] std::string GetName(void) const;

	/// <summary>
	/// This function sets the creation date of the scenario between its property.
	/// </summary>
	/// <param name="name">The creation date</param>
	void SetCreationDate(void);

	/// <summary>
	/// This function returns the creation date of the scenario.
	/// </summary>
	/// <returns>The creation date</returns>
	[[nodiscard]] std::string GetCreationDate(void) const;

	/// <summary>
	/// This function sets the last edit date of the scenario between its property.
	/// </summary>
	/// <param name="name">The creation date</param>
	void SetLastEditDate(void);

	/// <summary>
	/// This function returns the last edit date of the scenario.
	/// </summary>
	/// <returns>The last edit date</returns>
	[[nodiscard]] std::string GetLastEditDate(void) const;

	/// <summary>
	/// This method shows that can't be delete the currently used scenario, opening an Infobox Window.
	/// </summary>
	void UnableToDelete(void);

	[[nodiscard]] std::string GetScenarioSize(void) const;

	[[nodiscard]] classesData_t GetClassesDataSp(void) const noexcept;
private:
	#pragma region Init methods and constructor (called by friend class Advdenture)
	static void InitEmpty(std::shared_ptr<Scenario>& scenarioToInitSP, std::string zipKey, std::shared_ptr<PlayersArray>&& playersArray, const std::string& mapSize = "");
	static void InitRandom(std::shared_ptr<Scenario>& scenarioToInitSP, std::string zipKey, const uint32_t seed, std::shared_ptr<PlayersArray>&& playersArray);
	static void InitFromZip(std::shared_ptr<Scenario>& scenarioToInitSP, std::string zipKey, std::string scenarioName, std::shared_ptr<PlayersArray>&& playersArray);
	static void InitFromBinFile(std::shared_ptr<Scenario>& scenarioToInitSP, BinaryFileReader& bfr, std::shared_ptr<PlayersArray>&& playersArray);

	// The only constructor. INIT METHODS HAVE TO PROPER FILL THE SCENARIO!
	explicit Scenario(classesData_t _classesDataSP);
	#pragma endregion

	void SetMapSize(std::string _mapSize);

	void LoadProperties(void);
	void LoadAreaArrayFromXml(void);
	void LoadScenarioHeights(std::vector<byte_t>&& heightmapImgBytes = std::vector<byte_t>());
	void LoadScenarioTextures(std::vector<byte_t>&& textureImgBytes = std::vector<byte_t>());
	void LoadObjectsFromXml(void);

	classesData_t classesDataSP;

	// reference lookups
	rattlesmake::services::zip_service& zipService;

	// surface and terrain
	std::shared_ptr<Surface> surface;
	std::weak_ptr<Terrain> terrain;

	std::shared_ptr<PlayersArray> playersArraySP;

	[[nodiscard]] std::stringstream SerializeObjects(void) const;
	[[nodiscard]] std::stringstream SerializeProperties(void) const;
	[[nodiscard]] std::stringstream SerializeAreaArray(void) const;
	[[nodiscard]] std::vector<uint8_t> GenerateTerrainHeightsImage(int& width, int& height) const;
	[[nodiscard]] std::vector<uint8_t> GenerateTerrainTexturesImage(int& width, int& height) const;
	[[nodiscard]] std::vector<uint8_t> GenerateGridPngImage(int& width, int& height) const;

	// selection area
	bool bSelectionAreaRendering = false;
	std::shared_ptr<SelectionArea> selectionArea;

	// area array
	std::shared_ptr<AreaArray> areaArray;

	//Minimap
	std::shared_ptr<Minimap> minimap;

	//A pointer to the adventure that generated the minimap (N.B.: it's a weak_ptr since currentAdv has a shared_ptr to 'this' scenario object!)
	std::weak_ptr<Adventure> currentAdv;

	Properties properties;
	std::string scenarioName = "scenario01";
	std::string zipKey;
	std::string internalPath;
	bool wireframe = false;
	bool bHitboxRenderingActive = false;
	bool bSeaRenderingActive = false;
	bool bCloudsRenderingActive = false;
	bool bDebugTracingActive = false;
	bool bDrawObjectsWithoutNoise = false;
};
