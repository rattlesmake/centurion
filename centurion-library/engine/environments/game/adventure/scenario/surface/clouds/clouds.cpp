#include "clouds.h"
#include "clouds_shader.h"

Clouds::Clouds(const uint32_t _mapWidth, const uint32_t _mapHeight) : mapWidth(_mapWidth), mapHeight(_mapHeight)  {}
Clouds::~Clouds() {}

void Clouds::Render()
{
	clouds_shader::get_instance().draw();
}
