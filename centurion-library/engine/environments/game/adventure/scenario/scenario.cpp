#include "scenario.h"

#include <engine.h>
#include <math_utils.h>
#include <players/player.h>
#include <fileservice.h>
#include <services/logservice.h>
#include <services/sqlservice.h>
#include <settings.h>
#include <environments/game/classes/building.h>
#include <environments/game/adventure/scenario/minimap.h>
#include <environments/game/adventure/scenario/surface/surface.h>
#include <environments/game/adventure/scenario/surface/surfaceSettings.h>
#include <environments/game/adventure/scenario/surface/terrain/terrain.h>
#include <environments/game/adventure/scenario/surface/textures/terrainTextures.h>
#include <environments/game/adventure/scenario/surface/sea/sea.h>
#include <environments/game/adventure/scenario/surface/terrain/terrainTextureChunks.h>
#include <environments/game/adventure/scenario/surface/randommap/randomMapGenerator.h>
#include <environments/game/editor/editor.h>

#include <dialogWindows.h>
#include <environments/game/races/races_interface.h>

#include <mouse.h>
#include <viewport.h>
#include <camera.h>
#include <zipservice.h>
#include <png.h>


// Assets
#include <xml_classes.h>

#pragma region Init methods and constructor
void Scenario::InitEmpty(std::shared_ptr<Scenario>& scenarioToInitSP, std::string zipKey, std::shared_ptr<PlayersArray>&& playersArray, const std::string& mapSize)
{
	/// N.B.: Every time you set a static varable in this method (or in any other method called by this) give a look to the catch statement in the method Match::LoadMatch.
	/// If I'm here it means that I'm initializing a new EMPTY scenario
	
	assert(scenarioToInitSP);
	Scenario& scenarioToInit = (*scenarioToInitSP);
	scenarioToInit.playersArraySP = std::move(playersArray);

	GObject::ResetNextId();
	// Set a weak reference to the players array of the current scenario. The reference is common to all the ObjsCollection and GObjects.
	ObjsCollection::SetPlayersArrayWeakRef(scenarioToInit.playersArraySP);
	GObject::SetPlayersArrayWRef(scenarioToInit.playersArraySP);

	// Map will have default size (it comes from terrainInfo.xml) if mapSize="".
	scenarioToInit.zipKey = std::move(zipKey);

	scenarioToInit.SetMapSize(mapSize);  // N.B.: This method sets also Surface
	assert(scenarioToInit.surface);
	Area::SetSurface(scenarioToInit.surface);
	scenarioToInit.SetCreationDate();
	scenarioToInit.SetLastEditDate();
	scenarioToInit.scenarioName = "scenario01";
	scenarioToInit.properties.fullName = SqlService::GetInstance().GetTranslation("e_text_untitled_scenario", false);

	// Finally, create minimap (minimap requires a valid scenario)
	scenarioToInit.minimap = Minimap::Create(scenarioToInitSP);
}

void Scenario::InitRandom(std::shared_ptr<Scenario>& scenarioToInitSP, std::string zipKey, const uint32_t seed, std::shared_ptr<PlayersArray>&& playersArray)
{
	///N.B.: Every time you set a static varable in this method (or in any other method called by this) give a look to the catch statement in the method Match::LoadMatch.
	///If I'm here it means that I'm initializing a new random scenario
	
	assert(scenarioToInitSP);
	Scenario& scenarioToInit = (*scenarioToInitSP);
	scenarioToInit.playersArraySP = std::move(playersArray);

	GObject::ResetNextId();
	// Set a weak reference to the players array of the current scenario. The reference is common to all the ObjsCollection and GObjects.
	ObjsCollection::SetPlayersArrayWeakRef(scenarioToInit.playersArraySP);
	GObject::SetPlayersArrayWRef(scenarioToInit.playersArraySP);

	// Map will have default size (it comes from terrainInfo.xml) if generateRandom=false
	scenarioToInit.zipKey = std::move(zipKey);
	scenarioToInit.surface = std::shared_ptr<Surface>(new Surface(seed));
	scenarioToInit.surface->Initialize();
	Area::SetSurface(scenarioToInit.surface);
	Surface::GenerateRandom(scenarioToInit.playersArraySP, scenarioToInit.surface);
	scenarioToInit.terrain = scenarioToInit.surface->GetTerrainW();

	scenarioToInit.SetCreationDate();
	scenarioToInit.SetLastEditDate();
	scenarioToInit.scenarioName = "scenario01";
	scenarioToInit.properties.fullName = "Untitled Scenario"; // todo
	scenarioToInit.properties.mapSize = scenarioToInit.terrain.lock()->GetMapSize();

	//Finally, create minimap (minimap requires a valid scenario)
	scenarioToInit.minimap = Minimap::Create(scenarioToInitSP);
}

void Scenario::InitFromZip(std::shared_ptr<Scenario>& scenarioToInitSP, std::string zipKey, std::string scenarioName, std::shared_ptr<PlayersArray>&& playersArray)
{
	/// N.B.: Every time you set a static varable in this method (or in any other method called by this) give a look to the catch statement in the method Match::LoadMatch.
	/// If I'm here it means that I'm loading a scenario from a .zip file

	assert(scenarioToInitSP);
	Scenario& scenarioToInit = (*scenarioToInitSP);
	scenarioToInit.playersArraySP = std::move(playersArray);
	//if (scenarioName.empty() == false && Scenario::CheckIntegrity(zipPath, "scenarios/" + scenarioName))
		//scenarioSP = std::shared_ptr<Scenario>(new Scenario(zipPath, scenarioName, playersArray));
	//else
		//scenarioSP = std::shared_ptr<Scenario>(new Scenario(zipPath, false, playersArray)); // perche' false?? controllare sta riga (todo)
	
	GObject::ResetNextId();
	// Set a weak reference to the players array of the current scenario. The reference is common to all the ObjsCollection and GObjects.
	ObjsCollection::SetPlayersArrayWeakRef(scenarioToInit.playersArraySP);
	GObject::SetPlayersArrayWRef(scenarioToInit.playersArraySP);

	scenarioToInit.scenarioName = std::move(scenarioName);
	scenarioToInit.zipKey = std::move(zipKey);
	scenarioToInit.internalPath = "scenarios/" + scenarioToInit.scenarioName + "/";
	scenarioToInit.LoadProperties();  // N.B.: This method sets also Surface!
	assert(scenarioToInit.surface);
	Area::SetSurface(scenarioToInit.surface);

	// We can load heights and textures now
	scenarioToInit.LoadScenarioHeights();
	scenarioToInit.LoadScenarioTextures();

	// After heigth and textures, we can load objects and area arrays (THEY REQUIRES A CREATED SURFACE!)
	scenarioToInit.LoadObjectsFromXml();
	scenarioToInit.LoadAreaArrayFromXml();

	// Make loaded textures visible on the terrain
	scenarioToInit.surface->GetTerrain()->UpdateAllChunks(UpdateTerrainChunkFlags_Everything);

	// Finally, create minimap (minimap requires a valid scenario)
	scenarioToInit.minimap = Minimap::Create(scenarioToInitSP);
}

void Scenario::InitFromBinFile(std::shared_ptr<Scenario>& scenarioToInitSP, BinaryFileReader& bfr, std::shared_ptr<PlayersArray>&& playersArray)
{
	/// N.B.: Every time you set a static varable in this method (or in any other method called by this) give a look to the catch statement in the method Match::LoadMatch.
	/// If I'm here it means that I'm loading a scenario from a binary file (i.e. a save file)
	/// If you change loading order here, go to Scenario::SaveAsBinaryFile and impose the same saving order

	assert(scenarioToInitSP);
	Scenario& scenarioToInit = (*scenarioToInitSP);
	scenarioToInit.playersArraySP = std::move(playersArray);

	GObject::ResetNextId();
	// Set a weak reference to the players array of the current scenario. The reference is common to all the ObjsCollection and GObjects.
	ObjsCollection::SetPlayersArrayWeakRef(scenarioToInit.playersArraySP);
	GObject::SetPlayersArrayWRef(scenarioToInit.playersArraySP);

	{
		// I begin to read everything there is to read on the file regarding the scenario (reading will be done on another thread)
		bfr.ReadString();  // Map size
		bfr.ReadBinaryData();  // Heights map
		bfr.ReadBinaryData();  // Textures map
	}

	{
		const std::string mapSize = bfr.GetString();
		// Reading of scenario map size and creation of a surface via method Scenario::SetMapSize
		scenarioToInit.SetMapSize(mapSize);  // N.B.: This method sets also Surface!
		assert(scenarioToInit.surface);
		Area::SetSurface(scenarioToInit.surface);
	}

	{
		// Scenario heigths
		std::vector<uint8_t> heightmapImgBytes = bfr.GetBinData();
		scenarioToInit.LoadScenarioHeights(std::move(heightmapImgBytes));
	}

	{
		// Scenario textures:
		std::vector<uint8_t> textureImgBytes = bfr.GetBinData();
		scenarioToInit.LoadScenarioTextures(std::move(textureImgBytes));
	}

	{
		// After heigth and textures, we can load objects and area arrays (THEY REQUIRES A CREATED SURFACE!)
		scenarioToInit.playersArraySP->DeserializeFromBinFile(bfr, scenarioToInit.surface);
	}

	// Make loaded textures visible on the terrain
	scenarioToInit.surface->GetTerrain()->UpdateAllChunks(UpdateTerrainChunkFlags_Everything);

	// Finally, create minimap (minimap requires a valid scenario)
	scenarioToInit.minimap = Minimap::Create(scenarioToInitSP);
}

Scenario::Scenario(classesData_t _classesDataSP) :
	zipService(rattlesmake::services::zip_service::get_instance()),
	areaArray(std::shared_ptr<AreaArray>(new AreaArray())),
	selectionArea(std::shared_ptr<SelectionArea>(new SelectionArea())),
	classesDataSP(std::move(_classesDataSP))
{	
}
#pragma endregion


#pragma region Scenario methods
std::weak_ptr<Minimap> Scenario::GetMinimap(void) const
{
	return this->minimap;
}

std::weak_ptr<PlayersArray> Scenario::GetPlayersArrayW(void) const
{
	return this->playersArraySP;
}

std::shared_ptr<PlayersArray> Scenario::GetPlayersArray(void) const
{
	return this->playersArraySP;
}

std::weak_ptr<Surface> Scenario::GetSurfaceW(void) const
{
	return this->surface;
}

std::shared_ptr<Surface> Scenario::GetSurface(void) const
{
	return this->surface;
}

std::shared_ptr<AreaArray> Scenario::GetAreaArrayPtr(void) const
{
	return this->areaArray;
}

std::weak_ptr<SelectionArea> Scenario::GetSelectionAreaPtr(void) const
{
	return this->selectionArea;
}

void Scenario::SetCurrentAdv(const std::weak_ptr<Adventure>& curAdv)
{
	this->currentAdv = curAdv;
}

Scenario::~Scenario(void)
{
	assert(this->playersArraySP.use_count() == 1);
	#if CENTURION_DEBUG_MODE
	std::cout << "Scenario destructor" << std::endl;
	#endif
}

bool Scenario::CheckIntegrity(const std::string& zipKey, const std::string& scenarioFolderName)
{
	try
	{
		if (rattlesmake::services::zip_service::get_instance().check_if_folder_exists(zipKey, scenarioFolderName) == false) throw std::exception("Missing folder");
		if (rattlesmake::services::zip_service::get_instance().check_if_file_exists(zipKey, scenarioFolderName + "/properties.xml") == false) throw std::exception("Missing properties.xml");
		if (rattlesmake::services::zip_service::get_instance().check_if_file_exists(zipKey, scenarioFolderName + "/mapObjects.xml") == false) throw std::exception("Missing mapObjects.xml");
		if (rattlesmake::services::zip_service::get_instance().check_if_file_exists(zipKey, scenarioFolderName + "/areaArray.xml") == false) throw std::exception("Missing areaArray.xml");
		if (rattlesmake::services::zip_service::get_instance().check_if_file_exists(zipKey, scenarioFolderName + "/textures.png") == false) throw std::exception("Missing textures.png");
		if (rattlesmake::services::zip_service::get_instance().check_if_file_exists(zipKey, scenarioFolderName + "/heightmap.png") == false) throw std::exception("Missing heightmap.png");
		return true;
	}
	catch (const std::exception& ex)
	{
		Logger::LogMessage msg = Logger::LogMessage("Scenario \"" + scenarioFolderName + "\"" + " files integrity has failed. Error: " + ex.what(), "Warn", "", "Scenario", __FUNCTION__);
		Logger::Warn(msg);
		return false;
	}
}

void Scenario::EnableWireframe(void)
{
	this->wireframe = true;
}

void Scenario::DisableWireframe(void)
{
	this->wireframe = false;
}

bool Scenario::IsWireframeActive(void) const
{
	return this->wireframe;
}
bool Scenario::IsSeaRenderingActive(void) const
{
	return this->bSeaRenderingActive;
}

void Scenario::ToggleGrid(void)
{
	this->surface->ToggleGrid();
}

void Scenario::ToggleHitboxRendering(void)
{
	this->bHitboxRenderingActive = !this->bHitboxRenderingActive;
}

void Scenario::ToggleWireframe(void)
{
	this->wireframe = !this->wireframe;
}

void Scenario::ToggleSea(void)
{
	this->bSeaRenderingActive = !this->bSeaRenderingActive;
}

void Scenario::ToggleClouds(void)
{
	this->bCloudsRenderingActive = !this->bCloudsRenderingActive;
}

void Scenario::ToggleTracingDebugging(void)
{
	this->bDebugTracingActive = !this->bDebugTracingActive;
}

void Scenario::ToggleDrawWithoutNoise(void)
{
	this->bDrawObjectsWithoutNoise = !this->bDrawObjectsWithoutNoise;
}

bool Scenario::IsHitboxRenderingActive(void) const
{
	return this->bHitboxRenderingActive;
}

bool Scenario::IsTracingDebuggingActive(void) const
{
	return this->bDebugTracingActive;
}
bool Scenario::IsDrawWithoutNoiseActive(void) const
{
	return this->bDrawObjectsWithoutNoise;
}
bool Scenario::IsCloudRenderingActive(void) const
{
	return this->bCloudsRenderingActive;
}

float Scenario::GetNoiseFromRGBColor(const float color)
{
	return this->terrain.lock()->GetNoiseFromRGBColor(color);
}

void Scenario::UnableToDelete(void)
{
	gui::NewInfoWindow("e_text_unable_to_delete_scenario", IEnvironment::Environments::e_editor);
}
#pragma endregion


#pragma region Loading functions
void Scenario::LoadObjectsFromXml(void)
{
	try
	{
		const std::string path{ this->internalPath + "mapObjects.xml" };
		const std::string xmlText{ zipService.get_text_file(this->zipKey, path) };
		tinyxml2::XMLDocument mapobjs;
		// Assertion: file MUST exist.
		auto error = mapobjs.Parse(xmlText.c_str());
		if (error != tinyxml2::XML_SUCCESS)
			throw std::exception("Objects XML file is corrupted.");

		auto _mapobjsTag = mapobjs.FirstChildElement("mapObjects");
		// Assertion: tag must exist.
		assert(_mapobjsTag != nullptr);
		auto playersTag = _mapobjsTag->FirstChildElement("players");
		// Assertion: tag must exist.
		assert(playersTag != nullptr);
		// Assertion: surface must be valid
		assert(this->surface);
		this->playersArraySP->DeserializeFromXML((*playersTag), this->surface);
	}
	catch (CenturionException* e)
	{
		e->AddFuncSignature(__FUNCSIG__);
		throw e;
	}
	catch (std::exception e)
	{
		EngineException* ex = new EngineException(e, __FUNCSIG__);
		throw ex->AsCenturionException();
	}
}

std::stringstream Scenario::SerializeObjects(void) const
{
	// call this function always BEFORE area array serialization

	std::stringstream mapobjs{ "" };
	mapobjs << "<mapObjects>\n";
	PlayersArray::SetTabs(1);
	mapobjs << (*this->playersArraySP);
	mapobjs << "\n</mapObjects>" << endl;
	return mapobjs;
}

std::stringstream Scenario::SerializeProperties(void) const
{
	std::stringstream properties;
	properties << "<properties>" << endl <<
		"\t<name>" << this->properties.fullName << "</name>" << endl <<
		"\t<creationDate>" << this->properties.creationDate << "</creationDate>" << endl <<
		"\t<lastEditDate>" << this->properties.lastEditDate << "</lastEditDate>" << endl <<
		"\t<mapSize>" << this->properties.mapSize << "</mapSize>" << endl <<
		"\t<maxPitch>" << this->properties.maxPitch << "</maxPitch>" << endl <<
		"</properties>" << endl;
	return properties;
}

std::stringstream Scenario::SerializeAreaArray(void) const
{
	// call this function always AFTER objects serialization

	std::stringstream areaArray{ "" };
	AreaArray::SetTabs(0);
	areaArray << (*this->areaArray);
	return areaArray;
}

void Scenario::LoadAreaArrayFromXml(void)
{
	try
	{
		auto path = this->internalPath + "areaArray.xml";
		auto xmlText = zipService.get_text_file(this->zipKey, path);

		tinyxml2::XMLDocument _areasXML;
		if (_areasXML.Parse(xmlText.c_str()) != tinyxml2::XML_SUCCESS)
			return;

		auto _areaArray = _areasXML.FirstChildElement("areas");
		if (_areaArray != nullptr)
		{
			this->areaArray->Deserialize(*_areaArray);
		}
	}
	catch (CenturionException* e)
	{
		e->AddFuncSignature(__FUNCSIG__);
		throw e;
	}
	catch (std::exception e)
	{
		EngineException* ex = new EngineException(e, __FUNCSIG__);
		throw ex->AsCenturionException();
	}
}

void Scenario::LoadScenarioHeights(std::vector<uint8_t>&& heightmapImgBytes)
{
	const std::string path = this->internalPath + "heightmap.png";

	// Read image and prepare data
	rattlesmake::image::stb::flip_vertically_on_load(1);
	int imageXSize, imageYSize, nChannels;
	byte_t* heightsImage = nullptr;
	if (heightmapImgBytes.empty() == true)  // If true, load heightmap from zip
		heightsImage = zipService.get_image_data(this->zipKey, path, &imageXSize, &imageYSize, &nChannels, 0);
	else  // Else load heighthmap directly from provided bytes
		heightsImage = rattlesmake::image::stb::load_from_memory(heightmapImgBytes.data(), static_cast<int>((heightmapImgBytes.size() * sizeof(byte_t))), &imageXSize, &imageYSize, &nChannels, 0);

	auto terrain_ptr = this->terrain.lock();

	auto TERRAIN_ZNOISE_ARRAY = terrain_ptr->GetTerrainHeightsArrayPtr();
	TERRAIN_ZNOISE_ARRAY->clear();
	terrain_ptr->SetMinZ(MAX_REACHABLE_HEIGHT);
	terrain_ptr->SetMaxZ(MIN_GRASS_HEIGHT);
	for (int i = 0; i < imageXSize * imageYSize * 3; i += 3)
	{
		glm::vec3 color{};
		color.r = static_cast<float>(heightsImage[i]);
		color.g = static_cast<float>(heightsImage[i + 1]);
		color.b = static_cast<float>(heightsImage[i + 2]);
		auto height = (int)(color.r + color.g * 256 + color.b * 256 * 256);
		TERRAIN_ZNOISE_ARRAY->push_back(height);
		terrain_ptr->SetMinZ(std::min(terrain_ptr->GetMinZ(), (float)height));
		terrain_ptr->SetMaxZ(std::max(terrain_ptr->GetMaxZ(), (float)height));
	}
	rattlesmake::image::stb::free(heightsImage);
	rattlesmake::image::stb::flip_vertically_on_load(0);
}

void Scenario::LoadScenarioTextures(std::vector<uint8_t>&& textureImgBytes)
{
	const std::string path{ this->internalPath + "textures.png" };

	// Read image and prepare data
	rattlesmake::image::stb::flip_vertically_on_load(1);
	int imageXSize, imageYSize, nChannels;
	byte_t* texturesImage = nullptr;
	if (textureImgBytes.empty() == true)  // If true, load textures from zip
		texturesImage = zipService.get_image_data(this->zipKey, path, &imageXSize, &imageYSize, &nChannels, 0);
	else  // Else load textures directly from provided bytes
		texturesImage = rattlesmake::image::stb::load_from_memory(textureImgBytes.data(), static_cast<int>((textureImgBytes.size() * sizeof(byte_t))), &imageXSize, &imageYSize, &nChannels, 0);

	auto TERRAIN_TEXTURE_ARRAY = this->terrain.lock()->GetTerrainTexturesArrayPtr();  // array that contains texture id for each vertex of the map
	TERRAIN_TEXTURE_ARRAY->clear();

	auto textureArray = this->surface->GetTerrainTextureArray();  // array of terrain textures
	assert(textureArray.expired() == false);

	for (int i = 0; i < imageXSize * imageYSize * nChannels; i += nChannels)
	{
		glm::vec3 color{};
		color.r = static_cast<float>(texturesImage[i]);
		color.g = static_cast<float>(texturesImage[i + 1]);
		color.b = static_cast<float>(texturesImage[i + 2]);

		uint32_t texID = 0;
		auto tex = textureArray.lock()->GetTerrainTextureByColor(color);
		if (tex.expired() == false)
		{
			texID = tex.lock()->GetTextureId();
		}
		TERRAIN_TEXTURE_ARRAY->push_back(texID);
	}
	rattlesmake::image::stb::free(texturesImage);
	rattlesmake::image::stb::flip_vertically_on_load(0);
}
#pragma endregion


#pragma region Saving functions
void Scenario::SaveFromEditor(const bool saveZip)
{
	try
	{
		auto internalPath = "scenarios/" + this->scenarioName + "/";

		{
			auto objects = this->SerializeObjects();  // call this function always BEFORE area array serialization
			auto properties = this->SerializeProperties();
			auto area_array = this->SerializeAreaArray();
			zipService.add_file(this->zipKey, internalPath + "mapobjects.xml", objects);
			zipService.add_file(this->zipKey, internalPath + "properties.xml", properties);
			zipService.add_file(this->zipKey, internalPath + "areaarray.xml", area_array);
		}

		const int n = 3;
		int w = 0, h = 0;
		auto im1 = GenerateTerrainHeightsImage(w, h);
		zipService.add_image(this->zipKey, internalPath + "heightmap.png", im1.data(), w, h, n);

		auto im2 = GenerateTerrainTexturesImage(w, h);
		zipService.add_image(this->zipKey, internalPath + "textures.png", im2.data(), w, h, n);

		auto im3 = GenerateGridPngImage(w, h);
		zipService.add_image(this->zipKey, internalPath + "debug/grid_matrix.png", im3.data(), w, h, n);

		if (saveZip) 
			zipService.save_and_close(this->zipKey);

		Logger::LogMessage msg = Logger::LogMessage("Scenario has been successfully saved with the following name: \"" + GetFolderName() + "\"", "Info", "Editor", "Scenario", __FUNCTION__);
		Logger::Info(msg);

		auto adv = this->currentAdv.lock();
		if (adv)
			this->currentAdv.lock()->MarkAsNonEdited();
	}
	catch (CenturionException* e)
	{
		e->AddFuncSignature(__FUNCSIG__);
		throw e;
	}
	catch (std::exception e)
	{
		EngineException* ex = new EngineException(e, __FUNCSIG__);
		throw ex->AsCenturionException();
	}
}

void Scenario::SaveAsBinaryFile(BinaryFileWriter& bfw) const
{
	/// If you change saving order here, go to Scenario::InitFromBinFile and impose the same loading order

	bfw.PushString(this->properties.mapSize);

	const int n = 3;
	int w = 0, h = 0;
	{
		// Save heigthsmap
		std::vector<byte_t> heightmapImgBytes = this->GenerateTerrainHeightsImage(w, h);
		bfw.PushImageData(std::move(heightmapImgBytes), w, h, n);
	}

	{
		// Save textures map
		std::vector<byte_t> textureImgBytes = this->GenerateTerrainTexturesImage(w, h);
		bfw.PushImageData(std::move(textureImgBytes), w, h, n);
	}

	this->playersArraySP->SaveAsBinaryFile(bfw);
}

std::vector<byte_t> Scenario::GenerateTerrainHeightsImage(int& width, int& height) const
{
	auto wideness = surface->GetTerrainChunkWideness();  // 10
	auto nChunks = surface->GetNumberOfTerrainChunks();  // [48, 32]

	width = wideness * nChunks.x; // 480
	height = wideness * nChunks.y;  // 320

	std::vector<byte_t> image_data;
	for (auto znoise : *this->terrain.lock()->GetTerrainHeightsArrayPtr())
	{
		const uint32_t r = (znoise & 0x000000FF) >> 0;
		const uint32_t g = (znoise & 0x0000FF00) >> 8;
		const uint32_t b = (znoise & 0x00FF0000) >> 16;
		image_data.push_back(r);
		image_data.push_back(g);
		image_data.push_back(b);
	}
	return image_data;
}

std::vector<byte_t> Scenario::GenerateTerrainTexturesImage(int& width, int& height) const
{
	auto wideness = surface->GetTerrainChunkWideness(); // 10
	auto nChunks = surface->GetNumberOfTerrainChunks(); // [48, 32]

	width = wideness * nChunks.x;  // 480
	height = wideness * nChunks.y; // 320

	auto textureArray = this->surface->GetTerrainTextureArray();  // array of terrain textures
	assert(textureArray.expired() == false);

	std::vector<byte_t> image_data;
	for (auto texID : *this->terrain.lock()->GetTerrainTexturesArrayPtr())
	{
		glm::vec3 texColor{ 0.f };
		auto tex = textureArray.lock()->GetTerrainTextureById((uint16_t)texID);
		if (tex.expired() == false)
		{
			texColor = tex.lock()->GetColorVec3();
		}
		image_data.push_back(static_cast<int8_t>(texColor.r));
		image_data.push_back(static_cast<int8_t>(texColor.g));
		image_data.push_back(static_cast<int8_t>(texColor.b));
	}
	return image_data;
}

std::vector<byte_t> Scenario::GenerateGridPngImage(int& width, int& height) const
{
	auto surfaceGrid = this->surface->GetSurfaceGrid().lock();
	width = surfaceGrid->GetGridSizeX();
	height = surfaceGrid->GetGridSizeY();

	std::vector<byte_t> data;
	const uint32_t n = surfaceGrid->GetGridArraySize();
	for (uint32_t i = 0; i < n; i++)
	{
		const byte_t value = static_cast<byte_t>((surfaceGrid->GetGridCRef()[i])) * 255;
		data.push_back(value);
		data.push_back(value);
		data.push_back(value);
	}
	return data;
}

void Scenario::SetMapSize(std::string _mapSize)
{
	this->properties.mapSize = std::move(_mapSize);
	//this->surface.reset();
	this->surface = std::shared_ptr<Surface>(new Surface(this->properties.mapSize));
	this->surface->Initialize();
	this->terrain = this->surface->GetTerrainW();
}

void Scenario::LoadProperties(void)
{
	auto path = this->internalPath + "properties.xml";
	auto xmlText = zipService.get_text_file(this->zipKey, path);

	tinyxml2::XMLDocument xmlFile;
	if (xmlFile.Parse(xmlText.c_str()) != tinyxml2::XML_SUCCESS)
		return;

	tinyxml2::XMLElement* _properties = xmlFile.FirstChildElement("properties");
	this->properties.fullName = tinyxml2::TryParseFirstChildStrContent(_properties, "name");
	this->properties.creationDate = tinyxml2::TryParseFirstChildStrContent(_properties, "creationDate");
	this->properties.lastEditDate = tinyxml2::TryParseFirstChildStrContent(_properties, "lastEditDate");
	//this->properties.maxPitch = tinyxml2::TryParseFirstChildStrContent(_properties, "maxPitch");
	//this->SetMapSize(tinyxml2::TryParseFirstChildStrContent(_properties, "mapSize"));
	//TODO - per Pongo. Sistemare set di mapSize e maxPitch in mappa creata da zip e (se serve) anche in mappa creata casualmeente
	this->properties.maxPitch = 255;
	this->SetMapSize("medium");
}

std::string Scenario::GetScenarioSize(void) const
{
	return this->properties.mapSize;
}

classesData_t Scenario::GetClassesDataSp(void) const noexcept
{
	return this->classesDataSP;
}

void Scenario::Export(const std::string& path)
{
	try
	{
		auto& fs = rattlesmake::services::file_service::get_instance();
		if (fs.check_if_folder_exists(path) == false)
			fs.create_folder(path);

		//Terrain::SaveScenarioHeights(path + "//heightmap.png");
		//Terrain::SaveScenarioTextures(path + "//textures.png");

		//this->SaveObjectsToXml(path + "//mapObjects.xml");
		//this->SavePropertiesToXml(path + "//properties.xml");
		//this->SaveGridToPng(path);
		this->currentAdv.lock()->MarkAsNonEdited();
		gui::NewInfoWindow("e_text_export_succesfully", IEnvironment::Environments::e_editor);
		Logger::LogMessage msg = Logger::LogMessage("Scenario has been successfully exported with the following name: \"" + GetFolderName() + "\"", "Info", "Editor", "Scenario", __FUNCTION__);
		Logger::Info(msg);
	}
	catch (CenturionException* e)
	{
		e->AddFuncSignature(__FUNCSIG__);
		throw e;
	}
	catch (std::exception e)
	{
		EngineException* ex = new EngineException(e, __FUNCSIG__);
		throw ex->AsCenturionException();
	}
}
#pragma endregion

void Scenario::SetFolderName(std::string name)
{
	this->scenarioName = std::move(name);
}

std::string Scenario::GetFolderName(void) const
{
	return this->scenarioName;
}

void Scenario::SetName(std::string name)
{
	this->properties.fullName = std::move(name);
}

std::string Scenario::GetName(void) const
{
	return this->properties.fullName;
}

void Scenario::SetCreationDate(void)
{
	this->properties.creationDate = rattlesmake::services::file_service::get_instance().get_current_datetime("%d/%m/%Y");
}

std::string Scenario::GetCreationDate(void) const
{
	return this->properties.creationDate;
}

void Scenario::SetLastEditDate(void)
{
	this->properties.lastEditDate = rattlesmake::services::file_service::get_instance().get_current_datetime("%d/%m/%Y - %H:%M:%S");
}

std::string Scenario::GetLastEditDate(void) const
{
	return this->properties.lastEditDate;
}
