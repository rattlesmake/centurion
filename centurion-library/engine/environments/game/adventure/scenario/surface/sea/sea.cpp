#include "sea.h"
#include "sea_shader.h"

Sea::Sea(const uint32_t _mapWidth, const uint32_t _mapHeight) : mapWidth(_mapWidth), mapHeight(_mapHeight)  {}
Sea::~Sea() {}

void Sea::Render()
{
	sea_shader::get_instance().draw();
}
