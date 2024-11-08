/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/
#pragma once

#include <list>
#include <memory>


class EntityShader;
class VFX;

//TODO - maybe can be deleted. Type seems not used. Evaluate what to do based on future VFX. 
enum class VFXTypes
{
	e_unknown = 0,
	e_parabola
};

class VFXCollection
{
public:
	VFXCollection(void);
	VFXCollection(const VFXCollection& other) = delete;
	VFXCollection& operator=(const VFXCollection& other) = delete;
	~VFXCollection(void);

	void RenderVFXs(void);
	void AddVFX(std::unique_ptr<VFX>&& vfx);
private:
	std::list<std::unique_ptr<VFX>> vfxList;

	[[nodiscard]] static bool CheckRenderingConditions(const VFX& vfx);
};
