#include "colors_array.h"

ColorsArray::ColorsArray(tinyxml2::XMLElement* el)
{
	uint8_t i = 0;
	for (tinyxml2::XMLElement* _color = el->FirstChildElement(); _color != NULL; _color = _color->NextSiblingElement())
	{
		const int r = tinyxml2::TryParseIntAttribute(_color, "r");
		const int g = tinyxml2::TryParseIntAttribute(_color, "g");
		const int b = tinyxml2::TryParseIntAttribute(_color, "b");
		const glm::vec3 color{ r, g, b };
		this->defaultColors[i] = Color(color);
		i++;
	}
}

Color& ColorsArray::GetDefaultColor(const uint8_t idx)
{
	return (idx >= COLORS_ARRAY_SIZE) ? this->BLACK : this->defaultColors[idx];
}
