#include "terrainBrush.h"
#include "terrainConstants.h"

#include <environments/game/adventure/scenario/surface/surface.h>
#include <environments/game/adventure/scenario/surface/textures/terrainTextures.h>
#include <environments/game/adventure/scenario/surface/surfaceSettings.h>
#include <fileservice.h>
#include <services/logservice.h>
#include <math_utils.h>
#include <engine.h>
#include <environments/game/igame.h>

#include <mouse.h>
#include <viewport.h>
#include <camera.h>

#include <algorithm>
#include <execution>

TerrainBrush::TerrainBrush() : brushAreaForCalculations(glm::ivec2(0), 0, AreaOrigin::Center), brushAreaToDraw(glm::ivec2(0), 0, AreaOrigin::Center)
{
}

void TerrainBrush::Render()
{
	if (this->IsCreated() == false || this->IsEnabled() == false)
		return;

	this->UpdateDiameterScroll();

	// this function is run at every frame
	// so we need to follow the mouse position doing an update of the area center	
	this->brushAreaToDraw.SetPosition(glm::ivec2(rattlesmake::peripherals::mouse::get_instance().GetXMapCoordinate(), rattlesmake::peripherals::mouse::get_instance().GetYMapCoordinate()));
	this->brushAreaForCalculations.SetPosition(glm::ivec2(rattlesmake::peripherals::mouse::get_instance().GetXMapCoordinate(), rattlesmake::peripherals::mouse::get_instance().GetY2DMapCoordinate()));

	// and we finally render the area
	this->brushAreaToDraw.Render();
}

void TerrainBrush::ApplyTexture(const float value, const bool passable)
{
	auto vertices = this->brushAreaForCalculations.GetVerticesInside();
	auto terrain_ptr = this->terrain.lock();

	std::for_each(
		std::execution::par_unseq,
		vertices.begin(),
		vertices.end(),
		[value, passable, terrain_ptr](auto&& vertex) //Lambda
		{
			terrain_ptr->SetVertexTextureByIndex(vertex, (int)value, passable);
		});
}

void TerrainBrush::ApplyHeight(const float value)
{
	float h = value;
	glm::vec2 center = this->brushAreaForCalculations.GetCenter();
	float r = (float)this->brushAreaForCalculations.GetRadius();

	auto vertices = this->brushAreaForCalculations.GetVerticesInside();
	auto terrain_ptr = this->terrain.lock();

	std::for_each(
		std::execution::par_unseq,
		vertices.begin(),
		vertices.end(),
		[value, r, h, center, terrain_ptr](auto&& vertex) //Lambda
		{
			glm::vec2 vec = terrain_ptr->GetVertexCoordinatesByIndex(vertex);
			double d = Math::euclidean_distance(center.x, center.y, vec.x, vec.y);
			int z = int(float(std::exp(-1.0 * std::pow(3.0 * d / r, 2.0))) * h);

			terrain_ptr->IncreaseVertexZNoiseByIndex(vertex, z, (int)MAX_REACHABLE_HEIGHT_BRUSH);
		});
}

void TerrainBrush::ApplySea(const bool deepSea)
{
	int h = deepSea ? 50 : 20;
	glm::vec2 center = this->brushAreaForCalculations.GetCenter();
	float r = (float)this->brushAreaForCalculations.GetRadius();

	auto vertices = this->brushAreaForCalculations.GetVerticesInside();
	auto terrain_ptr = this->terrain.lock();

	std::for_each(
		std::execution::par_unseq,
		vertices.begin(),
		vertices.end(),
		[deepSea, r, h, center, terrain_ptr](auto&& vertex) //Lambda
		{
			glm::vec2 vec = terrain_ptr->GetVertexCoordinatesByIndex(vertex);
			double d = Math::euclidean_distance(center.x, center.y, vec.x, vec.y);
			int z = int(std::exp(-1.0 * std::pow(3.0 * d / r, 2.0)) * h);

			terrain_ptr->DecreaseVertexZNoiseByIndex(vertex, z, deepSea ? MIN_REACHABLE_HEIGHT : PASSABLE_SEA_HEIGHT);
		});
}

void TerrainBrush::UpdateDiameterScroll(void)
{
	auto& mouse = rattlesmake::peripherals::mouse::get_instance();
	if (mouse.ScrollBool)
	{
		this->UpdateDiameter(this->currentDiameter + (int)mouse.ScrollValue);
	}
}

void TerrainBrush::Create()
{
	auto surface = Engine::GetInstance().GetSurface().lock();
	this->terrain = surface->GetTerrainW();

	// update radius with default value
	this->UpdateDiameter(DEFAULT_BRUSH_DIAMETER);

	this->isCreated = true;
}

void TerrainBrush::UpdateDiameter(const uint16_t currentDiameter)
{
	float newDiameter = std::min(1.f * MAX_BRUSH_DIAMETER, std::max(1.f * MIN_BRUSH_DIAMETER, 1.f * currentDiameter));
	uint16_t newDiameterUint = (uint16_t)newDiameter;

	if (newDiameterUint == this->currentDiameter)
		return;

	this->currentDiameter = newDiameterUint;

	// currentDiameter = number of vertices involved
	// so we need the measure in pixels --> currentDiameter * Terrain::GetVerticesGapPixel()
	// radius --> / 2.f
	this->brushAreaForCalculations.SetRadius(newDiameter / 2.f * this->terrain.lock()->GetVerticesGapPixel());
	this->brushAreaToDraw.SetRadius(newDiameter / 2.f * this->terrain.lock()->GetVerticesGapPixel());
}

void TerrainBrush::Apply()
{
	if (this->IsEnabled() == false) return;

	switch (data.GetType())
	{
	case CHANGE_TERRAIN_HEIGHT:
		this->ApplyHeight(data.GetValue());
		this->terrain.lock()->UpdateChunksAroundPoint(this->brushAreaForCalculations.GetCenter().x, this->brushAreaForCalculations.GetCenter().y, UpdateTerrainChunkFlags_Heights | UpdateTerrainChunkFlags_Normals);
		break;
	case CHANGE_TERRAIN_TEXTURE:
		this->ApplyTexture(data.GetValue(), data.GetPassable());
		this->terrain.lock()->UpdateChunksAroundPoint(this->brushAreaForCalculations.GetCenter().x, this->brushAreaForCalculations.GetCenter().y, UpdateTerrainChunkFlags_Textures);
		break;
	case CHANGE_TERRAIN_SEA:
		this->ApplySea(data.GetPassable());
		this->terrain.lock()->UpdateChunksAroundPoint(this->brushAreaForCalculations.GetCenter().x, this->brushAreaForCalculations.GetCenter().y, UpdateTerrainChunkFlags_Heights | UpdateTerrainChunkFlags_Normals);
		break;
	case CHANGE_TERRAIN_DISABLED:
		break;
	default:
		break;
	}
}

TerrainBrush::Data::Data(TerrainBrush* myCreator, std::string type, std::string textureName, float heightValue)
{
	if (type == "height") this->type = CHANGE_TERRAIN_HEIGHT;
	else if (type == "texture") this->type = CHANGE_TERRAIN_TEXTURE;
	else if (type == "sea") this->type = CHANGE_TERRAIN_SEA;
	else myCreator->Disable();

	auto textureArray = Engine::GetInstance().GetSurface().lock()->GetTerrainTextureArray();
	assert(textureArray.expired() == false);

	auto tt = std::weak_ptr<TerrainTexture>();

	switch (this->type)
	{
	case CHANGE_TERRAIN_HEIGHT:
		this->value = heightValue;
		this->passable = true;
		myCreator->isEnabled = true;
		break;
	case CHANGE_TERRAIN_TEXTURE:
		tt = textureArray.lock()->GetTerrainTextureByName(textureName);
		if (tt.expired())
		{
			myCreator->Disable(); break;
		}
		this->value = (float)tt.lock()->GetTextureId();
		this->passable = tt.lock()->IsPassable();
		myCreator->isEnabled = true;
		break;
	case CHANGE_TERRAIN_SEA:
		this->passable = (heightValue == 1.f);
		myCreator->isEnabled = true;
		break;
	case CHANGE_TERRAIN_DISABLED:
		myCreator->Disable();
		break;
	default:
		break;
	}
}

void TerrainBrush::Initialize(std::string type, std::string textureName, float heightValue)
{
	this->Create();
	this->data = Data(this, type, textureName, heightValue);
}

void TerrainBrush::Disable()
{
	this->data.value = 0.f;
	this->data.passable = true;
	this->isEnabled = false;
	this->data.type = CHANGE_TERRAIN_DISABLED;
}
