/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/


#pragma once
#include <header.h>

#if SFML_ENABLED

namespace sf { class Music; }

class OggService
{
public:

	static OggService& GetInstance(void);

	void Initialize(void);

	class AudioPlayer
	{
	public:
		virtual void Play(void);
		virtual void Stop(void);
	protected:
		bool bLoop;
	};

	class Music : public AudioPlayer
	{
	public:
		Music(uint8_t environment);
		~Music(void);
		void Play(void) override;
		void Stop(void) override;
		void Update(void);
	private:
		std::shared_ptr<sf::Music> music;
		uint8_t env;

		std::vector<std::string> playlist;
		uint32_t currentSong = 0;
		std::vector<char> bytes;

	};

	class ObjectSound : public AudioPlayer
	{
	public:
		
		void Play(void) override;
		void Stop(void) override;
	};

	class NatureSound : public AudioPlayer
	{
	public:
		void Play(void) override;
		void Stop(void) override;

	};

private:
	std::shared_ptr<Music> musicPtr;
	std::set<std::shared_ptr<ObjectSound>> objectSoundSet;

	std::unordered_map<uint32_t, std::vector<std::string>> environmentPlaylist;

	void ReadMusic(tinyxml2::XMLDocument& xmlFile);
	uint32_t GetEnvironmentID(const std::string& envName);

	void ReadOGGStream(const std::string& filePathInZip);

	static OggService instance;
};
#endif
