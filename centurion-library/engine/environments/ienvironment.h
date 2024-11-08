/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <services/oggservice.h>
#include <cstdint>

class Editor;
class Match;
class IGame;

class IEnvironment
{
public:
	enum class Environments : uint8_t
	{
		// If you modify or add something here, please give a look to the following functions: Engine::OpenEnvironment, QuestionWindow::QuestionWindow (constructors),
		// ImageService::ReadEnvironmentImages
		e_unknown = 0,
		e_menu = 1,
		e_editor = 2,
		e_match = 3,
		e_mds = 4,
	};

	IEnvironment(const IEnvironment& other) = delete;
	IEnvironment& operator = (const IEnvironment& other) = delete;
	virtual ~IEnvironment(void);

	#pragma region Casting (to be used with caution)
	[[nodiscard]] Editor* AsEditor(void);
	[[nodiscard]] Match* AsMatch(void);
	[[nodiscard]] IGame* AsIGame(void);
	#pragma endregion

	const bool IsEditor(void) const;
	const bool IsMatch(void) const;
	const bool IsMenu(void) const;

	[[nodiscard]] IEnvironment::Environments GetType(void) const;

	virtual void Run(void);
	virtual void Quit(void);
protected:
	explicit IEnvironment(const IEnvironment::Environments type);

	#if SFML_ENABLED
	std::shared_ptr<OggService::Music> envMusic;
	#endif
	
	IEnvironment::Environments type;
};
