/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <header.h>

class Settings;

class PlayersSettings
{
	friend class MatchPreferences;
	friend class EditorPreferences;

public:
	class PlayerSettings
	{
		friend class PlayersSettings;

	public:
		std::string Race = "rome";
		std::string Name = "player";
		bool Active = true;
		int Id = 0;
	private:
		void Deserialize(tinyxml2::XMLElement* p);
		void Serialize(std::ofstream& oFile, size_t Id);
	};


	PlayersSettings();
	explicit PlayersSettings(const size_t nPlayers);
	void ResetToDefault(void);
	[[nodiscard]] PlayersSettings::PlayerSettings& operator[](const uint32_t idx) noexcept;

private:
	void Deserialize(tinyxml2::XMLElement* el);
	void Serialize(std::ofstream& oFile);
	std::vector<PlayersSettings::PlayerSettings> playerSettingsArray;
};

class RandomMapSettings
{
	friend class MatchPreferences;
	friend class EditorPreferences;
public:
	std::string ScenarioSize = "medium";
	std::string ScenarioType = "continental";
private:
	void Deserialize(tinyxml2::XMLElement* el);
	void Serialize(std::ofstream& oFile);
};

class GlobalPreferences
{
	friend class Settings;
public:
	void SetCameraMovespeed(float speed);
	void SetCameraMaxZoom(float zoom);
	void SetLanguage(std::string lang);
	void SetLastAdventure(std::string advName);
	[[nodiscard]] float GetCameraMaxZoom(void) const;
	[[nodiscard]] float GetCameraMovespeed(void) const;
	[[nodiscard]] const glm::vec2& GetWindowSize(void) const;
	[[nodiscard]] std::string GetLanguage(void) const;
	[[nodiscard]] std::string GetLastAdventure(void);
	[[nodiscard]] bool IsDebugActive(void) const;
	[[nodiscard]] bool IsFullscreenActive(void) const;
private:
	void SetWindowSize(int width, int height);
	std::string Language = "English";
	glm::vec2 WindowSize = glm::vec2(1366, 768);
	float CameraMaxZoom = 20.f;
	float CameraMovespeed = 10.f;
	bool bIsDebugActive = true;
	bool bIsFullscreenActive = false;
	std::string LastAdventure = "unsaved";
	void Deserialize(tinyxml2::XMLElement* el);
	void Serialize(std::ofstream& oFile);
};

class MatchPreferences
{
	friend class Settings;
public:
	MatchPreferences();
	PlayersSettings& GetPlayersSettings(void);
	RandomMapSettings& GetRandomMapSettings(void);
private:
	void Deserialize(tinyxml2::XMLElement* el);
	void Serialize(std::ofstream& oFile);
	PlayersSettings playersSettings;
	RandomMapSettings randomMapSettings;
};

class EditorPreferences
{
	friend class Settings;
public:
	EditorPreferences();
	RandomMapSettings& GetRandomMapSettings(void);
private:
	void Deserialize(tinyxml2::XMLElement* el);
	void Serialize(std::ofstream& oFile);
	RandomMapSettings randomMapSettings;
};

class Settings
{
public:

#pragma region Singleton
	///Copy constructor and operator = are not allowed in order to avoid accidentally copies of the singleton appearing.
	Settings(const Settings& other) = delete;
	Settings& operator=(Settings const& other) = delete;
	static Settings& GetInstance(void);
	~Settings();
#pragma endregion

	void Initialize(void);
	void Save(void);

	GlobalPreferences& GetGlobalPreferences(void);
	MatchPreferences& GetMatchPreferences(void);
	EditorPreferences& GetEditorPreferences(void);

	const std::string GetGameNameStr(void) { return "Centurion"; }
	const char* GetGameName(void) { return "Centurion"; }

private:
	Settings();

	GlobalPreferences globalPreferences;
	MatchPreferences matchPreferences;
	EditorPreferences editorPreferences;

	static Settings* instance;
};
