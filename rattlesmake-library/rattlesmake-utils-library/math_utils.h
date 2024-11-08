/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <array>
#include <vector>
#include <glm.hpp>

/// <summary>
/// This namespace contains some mathematical functions.
/// </summary>
namespace Math 
{
	const float ReciprocalRadiant = 3.14159265f / 180.0f;
	const float Radiant = 180.0f / 3.14159265f;

	[[nodiscard]] float GetViewAngle(const double x, const double y, const double xViewPos, const double yViewPos);

	/// <summary>
	/// This function calculates the euclidean distance between two points
	/// </summary>
	/// <param name="x1">X coordinate of the first point</param>
	/// <param name="y1">Y coordinate of the first point</param>
	/// <param name="x2">X coordinate of the second point</param>
	/// <param name="y2">Y coordinate of the second point</param>
	[[nodiscard]] extern inline double euclidean_distance(const double x1, const double y1, const double x2, const double y2);

	/// <summary>
	/// This function calculates the distance between two points within an ellipse
	/// </summary>
	/// <param name="x">X coordinate of the first point</param>
	/// <param name="y">Y coordinate of the first point</param>
	/// <param name="cx">X coordinate of the second point</param>
	/// <param name="cy">Y coordinate of the second point</param>
	/// <param name="r">Ellipse radius</param>
	float ellipse_distance(float x, float y, float cx, float cy, float r);
	
	/// <summary>
	/// This function creates a rectangle into a specified point
	/// </summary>
	/// <param name="x0">X coordinate of the rectangle</param>
	/// <param name="y0">Y coordinate of the rectangle</param>
	/// <param name="w">Rectangle width</param>
	/// <param name="h">Rectangle height</param>
	std::array<float, 8> get_rectangle_coords(float x0, float y0, float w, float h);

	// Map Noise Generation Functions

	struct Triangle
	{
		glm::vec3 a, b, c;
	};

	float random2D(const glm::vec2 st);

	// cosine interpolation
	float interpolate(const float a, const float b, const float x);

	float interpolated_noise(const int ind, const float x, const float y);

	float perlin_noise(float x, float y, float xy_scale, float z_scale, float x_seed, float y_seed, const uint32_t map_width, const uint32_t map_height);

	float smooth_noise(const float y, const float z, const float map_height, const float alpha = 0.005f, const float beta = 1.0f);

	std::vector<glm::ivec2> get_adjacent_vertices(glm::ivec2 pos, int triangle_width);

	glm::vec3 calculate_triangle_normal(Triangle T);

	int runif_discrete(int min, int max, int seed = -1);

	glm::vec2 runif2D_discrete(int minX, int maxX, int minY, int maxY, int seed = -1);

	uint32_t ConvertColorToId(glm::vec3 color);

	glm::vec3 ConvertPickingIdToVec3(const uint32_t pickingID);

	template <class T>
	struct Randomizer
	{
	public:
		T Extract(std::vector<T>& v, int seed = -1)
		{
			int index = runif_discrete(0, (int)v.size() - 1, seed);
			return v[index];
		}
	};
};
