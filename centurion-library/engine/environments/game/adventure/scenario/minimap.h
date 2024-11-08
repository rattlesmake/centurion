/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/
#pragma once

#include <header.h>
#include <png.h>

namespace gui { class Image; }
class Scenario;

class Minimap : public rattlesmake::image::png
{
public:

	static std::shared_ptr<Minimap> Create(const std::weak_ptr<Scenario>& creator);
	Minimap& operator=(const Minimap& other) = delete;
	~Minimap(void);

	void Update(void);
	bool IsUpdating(void) const;

	[[nodiscard]] bool IsOpen(void) const;
	void Open(void);
	void Close(void);
	void Render();

	void go_to_point(void);

protected:
	// inherited by png
	void update_texture(const uint8_t* image_data) override;
	uint32_t bind_texture(const uint8_t* image_data, const uint32_t width, const uint32_t height) override;


	explicit Minimap(const std::weak_ptr<Scenario>& creator);
	explicit Minimap(const Minimap& other) = delete;

private:
	void CreateMinimapTexture(void);
	void UpdateMinimapTexture(void);

	float cameraToX = 0.f, cameraToY = 0.f;
	
	bool bIsOpen = false;
	bool bIsBlocked = false;
	bool bToUpdate = true;
	
	//A pointer to the scenario that generated the minimap (N.B.: it's a weak_ptr since myCreator has a shared_ptr to this minimap object!)
	std::weak_ptr<Scenario> myCreator;
};