/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/
#pragma once

#include <header.h>

class Color
{
public:
	Color(void);
	explicit Color(const ImVec4& vec);
	explicit Color(const glm::vec3& vec);
	explicit Color(const glm::vec4& vec);

	ImVec4 ToImVec4(void);
	glm::vec4 ToGlmVec4(void);
	glm::vec3 ToGlmVec3(void);

	float r = 0.f, g = 0.f, b = 0.f;
private:
	void FixValues();
};
