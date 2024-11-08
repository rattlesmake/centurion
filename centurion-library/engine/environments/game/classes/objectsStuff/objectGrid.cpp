#include "objectGrid.h"
#include <cassert>

#include <encode_utils.h>
#include <tinyxml2_utils.h> 
#include <fileservice.h>
#include <zipservice.h>

#pragma region Static attributes:
std::unordered_map<std::string, std::weak_ptr<Grid>> Grid::gridCache;
#pragma endregion

#pragma region Constructor and destructor:
Grid::Grid(std::string _className, std::vector<CellValues>&& _gridData, const uint32_t _sizeX, const uint32_t _sizeY) :
	className(std::move(_className)), gridData(std::move(_gridData)), sizeX(_sizeX), sizeY(_sizeY)
{
}

Grid::~Grid(void)
{
	//When a GObject is deleted, also its shared_ptr will be deleted and associated counter is decreased.
	//In addition, when shared_ptr counter is 0, this destructor is invoked.
	//So we auto-remove the grid from the cache optimizing the memory :)
	//This operation done by the destructor is safe since operator= and copy constructor are marked as deleted
	Grid::RemoveGrid(this->className);
}
#pragma endregion

#pragma region Public members:
uint32_t Grid::GetSizeX(void) const
{
	return this->sizeX;
}

uint32_t Grid::GetSizeY(void) const
{
	return this->sizeY;
}

CellValues Grid::GetValueByCoordinates(const uint32_t X, const uint32_t Y)
{
	const uint32_t idx = (this->sizeX * Y + X);
	return this->gridData.at(idx);
}
#pragma endregion

#pragma region Public static members:
std::shared_ptr<Grid> Grid::GetOrReadGrid(const std::string& path, const std::string& className)
{
	assert(path.empty() == false && className.empty() == false);

	if (Grid::gridCache.contains(className) == false)
	{
		//Read grid from xml and then create it
		auto gridData = Grid::ReadGridFromXml(path);
		std::shared_ptr<Grid>grid{ new Grid{ className, std::move(std::get<2>(gridData)), std::get<0>(gridData), std::get<1>(gridData) } };

		//Add the GObject grid to the cache
		Grid::gridCache.insert({ className, grid });

		return grid;
	}

	return Grid::gridCache.at(className).lock();
}

std::shared_ptr<Grid> Grid::GetGrid(const std::string& className)
{
	assert(className.empty() == false);
	return (Grid::gridCache.contains(className) == true) ? Grid::gridCache.at(className).lock() : std::shared_ptr<Grid>();
}
#pragma endregion

#pragma region Private static members:
std::tuple<uint32_t, uint32_t, std::vector<CellValues>> Grid::ReadGridFromXml(const std::string& path)
{
	//TODO sostituire assert con opportune eccezioni

	auto xmlText = rattlesmake::services::zip_service::get_instance().get_text_file("?entities.zip", path);
	assert(xmlText.empty() == false);

	tinyxml2::XMLDocument xmlFile(true, tinyxml2::COLLAPSE_WHITESPACE);
	assert(xmlFile.Parse(xmlText.c_str()) == tinyxml2::XML_SUCCESS);

	auto grid = xmlFile.FirstChildElement("grid");
	assert(grid != nullptr);

	const uint32_t sizeX = tinyxml2::TryParseIntAttribute(grid, "x");
	const uint32_t sizeY = tinyxml2::TryParseIntAttribute(grid, "y");
	assert(sizeX > 0 && sizeY > 0);

	auto hexString = tinyxml2::TryParseFirstChildStrContent(grid, "grid");
	auto binString = Encode::HexStrToBinStr(hexString);
	assert(hexString.empty() == false && binString.empty() == false);

	std::vector<CellValues> gridData;
	//Store grid using string read via xml
	for (auto const& value : binString)
	{
		assert(std::isdigit(value));
		const uint8_t valueAsInt = value - '0';
		gridData.push_back(static_cast<CellValues>(valueAsInt));
	}
	xmlFile.Clear();

	return std::make_tuple(sizeX, sizeY, std::move(gridData));
}

bool Grid::RemoveGrid(const std::string& className)
{
	if (Grid::gridCache.contains(className) == false || Grid::gridCache[className].expired() == false)
		return false;
	Grid::gridCache.erase(className);
	return true;
}
#pragma endregion
