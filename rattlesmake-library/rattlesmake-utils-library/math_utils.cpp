#include "math_utils.h"

#include <random>
#include <climits>

float Math::GetViewAngle(const double x, const double y, const double xViewPos, const double yViewPos)
{
	const glm::vec2 viewPos = glm::vec2(xViewPos, yViewPos);
	float angle = (float)atan2(viewPos.y - y, viewPos.x - x) * Math::Radiant;
	if (angle < 0)
		angle += 360.0f;
	return angle;
}

inline double Math::euclidean_distance(const double x1, const double y1, const double x2, const double y2)
{
	return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

float Math::ellipse_distance(float x, float y, float cx, float cy, float r)
{
	return ((x - cx) * (x - cx) / (r * r) + 1.5f * (y - cy) * (y - cy) / (r * r));
}

std::array<float, 8> Math::get_rectangle_coords(float x0, float y0, float w, float h)
{
	return { x0, y0, x0, y0 - h, x0 + w, y0 - h, x0 + w, y0 };
}

float Math::random2D(const glm::vec2 st)
{
	const float value = glm::fract(sin(glm::dot(st, glm::vec2(12.9898f, 78.233f))) * 43758.5453123f);
	return value;
}

float Math::interpolate(const float a, const float b, const float x)
{
	float ft = x * 3.1415927f;
	float f = (1.0f - cos(ft)) * 0.5f;
	return  a * (1 - f) + b * f;
}

float Math::interpolated_noise(const int ind, const float x, const float y)
{
	int integer_X = int(floor(x));
	float fractional_X = glm::fract(x);
	int integer_Y = int(floor(y));
	float fractional_Y = glm::fract(y);
	glm::vec2 randomInput = glm::vec2(integer_X, integer_Y);
	float v1 = random2D(randomInput);
	float v2 = random2D(randomInput + glm::vec2(1.0, 0.0));
	float v3 = random2D(randomInput + glm::vec2(0.0, 1.0));
	float v4 = random2D(randomInput + glm::vec2(1.0, 1.0));
	float i1 = interpolate(v1, v2, fractional_X);
	float i2 = interpolate(v3, v4, fractional_X);
	return interpolate(i1, i2, fractional_Y);
}

float Math::perlin_noise(float x, float y, float xy_scale, float z_scale, float x_seed, float y_seed, const uint32_t map_width, const uint32_t map_height)
{
	z_scale *= map_width;
	xy_scale /= map_height;

	x += x_seed;
	y += y_seed;
	x *= xy_scale;
	y *= xy_scale;
	int numOctaves = 1;
	const float persistence = 0.5f;
	float total = 0.f;
	float frequency = (float)pow(2.0f, numOctaves);
	float amplitude = 1.f;
	for (int i = 0; i < numOctaves; ++i) {
		frequency /= 2.f;
		amplitude *= persistence;
		total += interpolated_noise(int(i % 10), x / frequency, y / frequency) * amplitude;
	}
	const float z = total / frequency * z_scale;

	return z;
}

float Math::smooth_noise(const float y, const float z, const float map_height, const float alpha, const float beta)
{
	float zSmoothed;
	zSmoothed = z * beta * tanh(y * alpha) * tanh((map_height - y) * alpha);
	return zSmoothed;
}

std::vector<glm::ivec2> Math::get_adjacent_vertices(glm::ivec2 pos, int triangle_width)
{
	int gap = triangle_width;
	std::vector<glm::ivec2> output;
	output = {
		glm::ivec2(pos.x - gap, pos.y), // left
		glm::ivec2(pos.x - gap, pos.y + gap), // up-left
		glm::ivec2(pos.x, pos.y + gap), // up
		glm::ivec2(pos.x + gap, pos.y), // right 
		glm::ivec2(pos.x + gap, pos.y - gap), // down-right
		glm::ivec2(pos.x, pos.y - gap), // down
	};
	return output;
}

glm::vec3 Math::calculate_triangle_normal(Triangle T)
{
	glm::vec3 N = glm::cross(T.c - T.a, T.b - T.a);
	return N;
}

int Math::runif_discrete(int min, int max, int seed)
{
	std::random_device rd;
	std::mt19937 mt_rand(rd());
	std::mt19937 mt_seed(seed);

	std::uniform_int_distribution<int> distribution(min, max);

	int result;
	if (seed == -1) result = distribution(mt_rand);
	else result = distribution(mt_seed);
	return result;
}

glm::vec2 Math::runif2D_discrete(int minX, int maxX, int minY, int maxY, int seed)
{
	const int x = runif_discrete(minX, maxX, seed);
	const int y = runif_discrete(minY, maxY, seed);
	return glm::vec2(x, y);
}

uint32_t Math::ConvertColorToId(glm::vec3 color)
{
	return (uint32_t)(color.r + color.g * 256 + color.b * 256 * 256);
}

glm::vec3 Math::ConvertPickingIdToVec3(const uint32_t pickingID)
{
	const uint32_t r = (pickingID & 0x000000FF) >> 0;
	const uint32_t g = (pickingID & 0x0000FF00) >> 8;
	const uint32_t b = (pickingID & 0x00FF0000) >> 16;
	return glm::vec3(r, g, b);
}
