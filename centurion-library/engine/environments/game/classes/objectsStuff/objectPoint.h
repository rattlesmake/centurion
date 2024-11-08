/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <header.h>

/// <summary>
/// This class is a 2D Point
/// </summary>
class Point
{
public:
	uint32_t x = 0;
	uint32_t y = 0;
	uint32_t z = 0;

	Point(void);
	Point(const uint32_t _x, const uint32_t _y);
	[[nodiscard]] Point operator+(const Point& p) const noexcept;
	[[nodiscard]] Point operator-(const Point& p) const noexcept;
	[[nodiscard]] bool operator==(const Point& p) const noexcept;

	#pragma region To scripts members:
	[[nodiscard]] uint32_t GetX(void) const;
	void SetX(const uint32_t _x);
	[[nodiscard]] uint32_t GetY(void) const;
	void SetY(const uint32_t _y);
	void SetCoordinates(const uint32_t _x, const uint32_t _y);
	#pragma endregion

	[[nodiscard]] glm::vec3 ToVec3(void) const;

	void GetBinRepresentation(std::vector<uint8_t>& data) const;
	void InitByBinData(std::vector<uint8_t>& data, uint32_t& offset);

	[[nodiscard]] static std::shared_ptr<Point> GetClickedPoint(void);

	class PointHashFunct
	{
	public:

		// We use predfined hash functions of int and define our hash function as XOR of the hash values. 
		//It requires redefinition of Point::operator==.
		size_t operator()(const Point& p) const
		{
			return (std::hash<int>()(p.x)) ^ (std::hash<int>()(p.y));
		}
	};
private:
	bool bIsValid = false;
};
