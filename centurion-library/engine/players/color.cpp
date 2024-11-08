#include "color.h"
#include "player.h"

Color::Color(void)
{
}

Color::Color(const ImVec4& vec)
{
	this->r = vec.x;
	this->g = vec.y;
	this->b = vec.z;
	this->FixValues();
}

Color::Color(const glm::vec3& vec)
{
	this->r = vec.r;
	this->g = vec.g;
	this->b = vec.b;
	this->FixValues();
}

Color::Color(const glm::vec4& vec)
{
	this->r = vec.r;
	this->g = vec.g;
	this->b = vec.b;
	this->FixValues();
}

ImVec4 Color::ToImVec4()
{
	return ImVec4(this->r, this->g, this->b, 1.f);
}

glm::vec4 Color::ToGlmVec4()
{
	return glm::vec4(this->r, this->g, this->b, 1.f);
}

glm::vec3 Color::ToGlmVec3()
{
	return glm::vec3(this->r, this->g, this->b);
}

void Color::FixValues()
{
	if (this->r > 1 || this->g > 1 || this->b > 1)
	{
		this->r = this->r / 255.f;
		this->g = this->g / 255.f;
		this->b = this->b / 255.f;
	}
}
