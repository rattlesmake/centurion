#include "objectPoint.h"
#include <mouse.h>

#include <bin_data_interpreter.h>

#pragma region Constructors:
Point::Point(void) 
{
}

Point::Point(const uint32_t _x, const uint32_t _y)
	: x(_x), y(_y)
{
}
#pragma endregion

#pragma region Operators:
Point Point::operator+(const Point& p) const noexcept
{
	return Point(this->x + p.x, this->y + p.y);
}

Point Point::operator-(const Point& p) const noexcept
{
	return Point(this->x - p.x, this->y - p.y);
}

bool Point::operator==(const Point& p) const noexcept
{
	return (this->x == p.x && this->y == p.y);
}
#pragma endregion

glm::vec3 Point::ToVec3(void) const
{
	return glm::vec3(this->x, this->y, this->z);
}

void Point::GetBinRepresentation(std::vector<uint8_t>& data) const
{
	BinaryDataInterpreter::PushUInt32(data, this->x);
	BinaryDataInterpreter::PushUInt32(data, this->y);
	BinaryDataInterpreter::PushUInt32(data, this->z);
}

void Point::InitByBinData(std::vector<uint8_t>& data, uint32_t& offset)
{
	this->x = BinaryDataInterpreter::ExtractUInt32(data, offset);
	this->y = BinaryDataInterpreter::ExtractUInt32(data, offset);
	this->z = BinaryDataInterpreter::ExtractUInt32(data, offset);
}

#pragma region Static members:
std::shared_ptr<Point> Point::GetClickedPoint(void)
{
	rattlesmake::peripherals::mouse& instance = rattlesmake::peripherals::mouse::get_instance();
	if (instance.LeftClick == true || instance.RightClick == true)
	{
		const uint32_t x = static_cast<uint32_t>(instance.GetXMapCoordinate());
		const uint32_t y = static_cast<uint32_t>(instance.GetY2DMapCoordinate());
		
		return std::shared_ptr<Point>(new Point(x, y));
	}
	return std::shared_ptr<Point>();
}
#pragma endregion

#pragma region To scripts members:
uint32_t Point::GetX(void) const
{
	return this->x;
}

void Point::SetX(const uint32_t _x)
{
	this->x = _x;
}

uint32_t Point::GetY(void) const
{
	return this->y;
}

void Point::SetY(const uint32_t _y)
{
	this->y = _y;
}

void Point::SetCoordinates(const uint32_t _x, const uint32_t _y)
{
	this->x = _x;
	this->y = _y;
}
#pragma endregion
