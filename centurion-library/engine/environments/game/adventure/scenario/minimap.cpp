#include "minimap.h"

#include <engine.h>

#include <png_shader.h>
#include <mouse.h>
#include <viewport.h>
#include <camera.h>
#include <keyboard.h>

#include <environments/game/adventure/scenario/surface/surface.h>
#include <environments/game/adventure/scenario/scenario.h>
#include <environments/game/igame.h>
#include <environments/game/igameUi.h>
#include <ui.h>

#define IMAGE_NAME_MINIMAP "##minimap"

#pragma region Constructor and destructor:
Minimap::Minimap(const std::weak_ptr<Scenario>& creator) : myCreator(creator)
{
	//this->minimapRectangle.shader = Shader::GetInstance(Shader_Rectangle)->AsRectangleShader();

	// cil_image properties
	this->bVerticallyFlipped = true;  
}

std::shared_ptr<Minimap> Minimap::Create(const std::weak_ptr<Scenario>& creator)
{
	std::shared_ptr<Minimap> new_minimap = std::shared_ptr<Minimap>(new Minimap(creator));
	new_minimap->me = new_minimap;
	return new_minimap;
}

Minimap::~Minimap(void)
{
}
#pragma endregion

void Minimap::Update(void)
{
	// we activate a boolean to notify the need of update
	// the minimap will be updated the first time it will be active
	this->bToUpdate = true;
}

bool Minimap::IsUpdating(void) const
{
	return this->bToUpdate;
}

bool Minimap::IsOpen(void) const
{
	return this->bIsOpen;
}

void Minimap::Open(void)
{
	auto surface = this->myCreator.lock()->GetSurface();
	auto visibleMapSize = surface->GetVisibleMapSize();

	auto ui = Engine::GetInstance().GetEnvironment()->AsIGame()->GetUIRef();
	uint32_t bottomBarHeight = ui->BottomBarHeight;
	uint32_t topBarHeight = ui->TopBarHeight;

	this->bIsOpen = true;
	this->bToUpdate = true;
}

void Minimap::Close(void)
{
	this->bIsOpen = false;
}

void Minimap::Render()
{
	if (this->bToUpdate == true)
		this->UpdateMinimapTexture();

	this->rattlesmake::image::png::render(0, 0, rattlesmake::image::png_flags_::png_flags_NoRepeat, RIL_IMAGE_DEFAULT_VAL, RIL_IMAGE_DEFAULT_VAL);

	//TODO - objects rendering
}

void Minimap::update_texture(const uint8_t* image_data)
{
	auto texData = this->textureData.lock();
	if (texData)
	{
		glBindTexture(GL_TEXTURE_2D, texData->get_opengl_texture_id());
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texData->get_image_width(), texData->get_image_height(), GL_RGB, GL_UNSIGNED_BYTE, image_data);
	}
}

uint32_t Minimap::bind_texture(const uint8_t* image_data, const uint32_t width, const uint32_t height)
{
	uint32_t textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// create texture and generate mipmaps
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	return textureID;
}

#pragma region Private members:
void Minimap::CreateMinimapTexture(void)
{
	//Texture minimapImageTexture = Texture(0, (int)rattlesmake::peripherals::viewport::get_instance().GetWidth(), (int)rattlesmake::peripherals::viewport::get_instance().GetHeight(), 3);
	std::vector<byte_t> _imageData(static_cast<uint64_t>(rattlesmake::peripherals::viewport::get_instance().GetWidth()) * rattlesmake::peripherals::viewport::get_instance().GetHeight() * 3);

	// this function has to be invoked "una tantum"
	// i.e. 1 time per environment life cycle.

	// the goal is create the texture (using glTexImage2D)
	// with a black temporary image 

	this->stringID = IMAGE_NAME_MINIMAP;
	auto& imageShader = rattlesmake::image::png_shader::get_instance();
	uint32_t width = (uint32_t)rattlesmake::peripherals::viewport::get_instance().GetWidth();
	uint32_t height = (uint32_t)rattlesmake::peripherals::viewport::get_instance().GetHeight();
	const uint8_t* data = _imageData.data();

	if (imageShader.check_if_png_in_cache(this->stringID) == false)
	{
		int w = 0, h = 0, n = 0;
		this->textureData = imageShader.add_image_data(this->bind_texture(data, width, height), this->stringID, width, height, 3);
	}
	else
	{
		this->textureData = imageShader.get_image_data(this->stringID);
	}
}

void Minimap::UpdateMinimapTexture(void)
{
	if (this->textureData.expired())
		this->CreateMinimapTexture();

	// this function has to be invoked when minimap is active
	// in order to render map and objects with the minimap projection matrix.

	// it should be called every time something changes in the surface
	// it can happen a lot of times in Editor environment
	// it should never happen in Match environment.

	// the goal is update the texture (using glTexSubImage2D)
	
	auto texData = this->textureData.lock();
	std::vector<uint8_t> imageData(static_cast<uint64_t>(texData->get_image_width()) * texData->get_image_height() * texData->get_image_channels());
	glReadPixels(0, 0, texData->get_image_width(), texData->get_image_height(), GL_RGB, GL_UNSIGNED_BYTE, imageData.data());
	this->update_texture(imageData.data());

	// turn off update
	this->bToUpdate = false;
}

void Minimap::go_to_point(void)
{
	if (rattlesmake::peripherals::mouse::get_instance().LeftClick == false || rattlesmake::peripherals::mouse::get_instance().IsCursorInGameScreen() == false) return;

	glm::vec2 visibleMapSize = Engine::GetInstance().GetEnvironment()->AsIGame()->GetCurrentScenario()->GetSurface()->GetVisibleMapSize();

	auto ui = Engine::GetInstance().GetEnvironment()->AsIGame()->GetUIRef();
	auto selectionArea = this->myCreator.lock()->GetSelectionAreaPtr().lock();

	uint32_t bottomBarHeight = ui->BottomBarHeight;
	uint32_t topBarHeight = ui->TopBarHeight;


	// go to point
	if (rattlesmake::peripherals::mouse::get_instance().GetYPosition() <= (int)bottomBarHeight || rattlesmake::peripherals::mouse::get_instance().GetYPosition() >= rattlesmake::peripherals::viewport::get_instance().GetHeight() - topBarHeight)
		return;

	cameraToX = rattlesmake::peripherals::mouse::get_instance().GetXLeftClick() / rattlesmake::peripherals::viewport::get_instance().GetWidth() * visibleMapSize.x - rattlesmake::peripherals::viewport::get_instance().GetWidthZoomed() / 2.f;
	cameraToY = rattlesmake::peripherals::camera::get_instance().GetYMinimapCoordinate(rattlesmake::peripherals::mouse::get_instance().GetYLeftClick(), bottomBarHeight, topBarHeight) / rattlesmake::peripherals::viewport::get_instance().GetHeight() * visibleMapSize.y - rattlesmake::peripherals::viewport::get_instance().GetHeightZoomed() / 2.f;

	//todo
	//todo
	//todo
	// if you are clicking on a townhall you have to double click 
	// to move the camera there and quit minimap
	/*if (Picking::Obj::GetLeftClickId() > 0 && Picking::HasDoubleClicked())
	{
		if (Game::GetSelectedObject() == nullptr) return;
		cameraToX = Game::GetSelectedObject()->AsBuilding()->GetPositionX() - myWindow::GetInstance().GetWidthZoomed() / 2.f;
		cameraToY = Game::GetSelectedObject()->AsBuilding()->GetPositionY() - myWindow::GetInstance().GetHeightZoomed() / 2.f;
		Minimap::Unblock();
	}*/
	//todo
	//todo
	//todo

	if (this->bIsBlocked == false)
	{
		rattlesmake::peripherals::camera::get_instance().GoToPoint(cameraToX, cameraToY);
		rattlesmake::peripherals::mouse::get_instance().LeftClick = false;
		rattlesmake::peripherals::mouse::get_instance().LeftHold = false;

		this->Close();
		ui->ShowMenubar();
		selectionArea->ResetDrawingPoints();
	}
}
#pragma endregion
