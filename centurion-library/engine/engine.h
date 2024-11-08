/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <header.h>
#include <settings.h>

#include <environments/ienvironment.h>
#include <environments/game/game_time.h>

namespace rattlesmake
{
	namespace peripherals { 
		class viewport; 
		class mouse; 
		class keyboard; 
		class camera;
	};
	namespace services
	{
		class file_service;
		class zip_service;
	};
};

namespace centurion
{
	namespace fb
	{
		class shadows_fb;
	};
};

class DurationLogger;
class LoadingScreen;
class PyService;
class PyConsole;
class ColorsArray;
class UserInput;
class Surface;

class Engine
{
public:
	#pragma region Singleton
	///Copy constructor and operator = are not allowed in order to avoid accidentally copies of the singleton appearing.
	Engine(const Engine& other) = delete;
	Engine& operator=(Engine const& other) = delete;
	[[nodiscard]] static Engine& GetInstance(void);
	~Engine(void); 
	#pragma endregion

	class Fps
	{
	public:
		/// <summary>
		/// This function returns the game Frames Per Second.
		/// </summary>
		/// <returns>A FPS.</returns>
		[[nodiscard]] uint16_t GetFps(void) const;

		/// <summary>
		/// This function returns milliseconds to process each frame.
		/// </summary>
		/// <returns>A MPFS.</returns>
		[[nodiscard]] uint16_t GetMpfs(void) const;

		void ToggleSleepFps(void);
		void Initialize(void);
		void Update(void);
		void SleepFps(void);
	private:
		glm::uint nframes = 0;
		uint16_t _Fps = 0;
		uint16_t Mpfs = 0;
		uint16_t RealMpfs = 0;
		double RealMpfsSum = 0;
		double currentTime = 0.0;
		std::chrono::steady_clock::time_point start;
		std::chrono::steady_clock::time_point frameBeginTime;
		std::chrono::steady_clock::time_point end;
		bool bSleepFps = true;
		std::condition_variable cvSleepFPS;
		std::mutex cvSleepFPS_mutex;
	}	fps;

	static Time& GetGameTime(void);
	

	[[nodiscard]] std::weak_ptr<Surface> GetSurface(void) const;

	/// <summary>
	/// This functions sets the environment in which the game is (for instance: editor, match, menu, MDS).
	/// </summary>
	/// <param name="_environments">The ID of the environment.</param>
	void SetEnvironment(const IEnvironment::Environments _environment);
	
	/// <summary>
	/// This functions returns the environment in which the game is (for instance editor, strategy, menu). 
	/// <returns>A value corresponding to the current environment.</returns>
	/// </summary>
	[[nodiscard]] IEnvironment::Environments GetEnvironmentId(void) const;

	[[nodiscard]] const std::shared_ptr<ColorsArray> GetDefaultColorsArray(void) const;

	/// <summary>
	/// This function lauches the game.
	/// </summary>
	uint8_t Launch(void);

	void FuncDurationProfilingStart(void);
	void FuncDurationProfilingStop(void);
	void FuncDurationProfilingStamp(void);

	bool Initialize(const char* exe_root);

	#pragma region To scripts members:
	void DeleteImage(const std::string image_relative_path);
	void GameClose(void);
	[[nodiscard]] std::vector<std::string> GetAllRacesNames(void) const;
	[[nodiscard]] std::vector<std::string> GetAvailableLanguages(void) const;
	[[nodiscard]] std::shared_ptr<IEnvironment> GetEnvironment(void) const;
	[[nodiscard]] std::vector<std::string> GetListOfSaveFiles(void) const;
	[[nodiscard]] std::vector<std::string> GetListOfValidAdventures(void) const;
	[[nodiscard]] std::vector<std::string> GetListOfValidAdventuresWithDetails(void) const;
	[[nodiscard]] std::vector<std::string> GetScenarioSizes(void) const;
	[[nodiscard]] std::vector<std::string> GetScenarioTypes(void) const;
	[[nodiscard]] float GetViewportHeight(void) const;
	[[nodiscard]] float GetViewportWidth(void) const;
	[[nodiscard]] bool IsKeyPressed(std::string keyName) const;
	void LoadImage(const std::string image_relative_path);
	void OpenEnvironment(string environment);
	[[nodiscard]] dbTranslation_t Translate(std::string word) const;
	[[nodiscard]] std::unordered_map<dbWord_t, dbTranslation_t> TranslateWords(std::list<dbWord_t> wordsToTranslate) const;
	#pragma endregion //End To scripts members

private:
	Engine(void);

	//Private methods
	void ReadDataXml(void);
	void ReadFontsXml(void);
	std::string GetCppVersion(void) const;
	std::string GetOpenglVersion(void) const;
	std::string GetMaxArrayTextureLayers(void) const;
	void FuncDurationProfilingBegin(void);
	void FuncDurationProfilingEnd(void);

	void ChangeEnvironment(void);

	rattlesmake::peripherals::viewport& viewport;
	rattlesmake::peripherals::mouse& mouse;
	Settings& settings;
	rattlesmake::peripherals::camera& camera;
	rattlesmake::peripherals::keyboard& keyboard;
	PyService& pyService;
	PyConsole& pyConsole;
	rattlesmake::services::file_service& fileService;
	rattlesmake::services::zip_service& zipService;
	centurion::fb::shadows_fb& shadowsFrameBuffer;

	#if SFML_ENABLED
	OggService& oggService;
	#endif

	//Environment
	std::shared_ptr<IEnvironment> environment;
	IEnvironment::Environments nextEnvironment{ IEnvironment::Environments::e_menu };
	uint8_t pageToOpen = 0; //For the moment, it is used only for the menu environment
	bool bChangeEnvironment = false;

	bool FUNC_DURATION_PROFILING_STAMP = false;
	bool FUNC_DURATION_PROFILING = false;
	DurationLogger* D_LOG = nullptr;

	//Game time:
	Time game_time;

	//Color array:
	std::shared_ptr<ColorsArray> defaultColorsArray;

	static Engine instance;
};
