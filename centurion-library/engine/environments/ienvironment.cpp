#include "ienvironment.h"
#include "game/igame.h"
#include "menu/menu.h"

#include <assert.h>

#if CENTURION_DEBUG_MODE
#include <iostream> 
#endif

#pragma region Constructors and destructor:
IEnvironment::IEnvironment(const IEnvironment::Environments type) : type(type)
{
	auto envType = static_cast<IEnvironment::Environments>(this->type);
	#if SFML_ENABLED
	this->envMusic = std::shared_ptr<OggService::Music>(new OggService::Music(envType));
	#endif
}

IEnvironment::~IEnvironment(void)
{
	#if CENTURION_DEBUG_MODE
	std::cout << "IEnvironment destructor" << std::endl;
	#endif
}
#pragma endregion

#pragma region Casting:
Editor* IEnvironment::AsEditor(void)
{
	assert(this->type == Environments::e_editor);
	return (Editor*)this;
}

Match* IEnvironment::AsMatch(void)
{
	assert(this->type == Environments::e_match);
	return (Match*)this;
}

IGame* IEnvironment::AsIGame(void)
{
	assert(this->type == Environments::e_editor || this->type == Environments::e_match);
	return (IGame*)this;
}
#pragma endregion

const bool IEnvironment::IsEditor(void) const
{
	return this->type == Environments::e_editor;
}
const bool IEnvironment::IsMatch(void) const
{
	return this->type == Environments::e_match;
}
const bool IEnvironment::IsMenu(void) const
{
	return this->type == Environments::e_menu;
}
IEnvironment::Environments IEnvironment::GetType(void) const
{
	return this->type;
}

void IEnvironment::Run(void)
{
	return; //Don't execute anything.
}

void IEnvironment::Quit(void)
{
	return; //Don't execute anything.
}
