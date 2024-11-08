#if SFML_ENABLED
#include <services/oggservice.h>
#include "zipservice.h"
#include "fileservice.h"
#include <tinyxml2.h>

#include <engine.h>

#include <SFML/Audio.hpp>

OggService OggService::instance;

OggService& OggService::GetInstance(void)
{
	return instance;
}

void OggService::Initialize(void)
{
	// read XML of environment music 
	auto& zs = rattlesmake::services::zip_service::get_instance();

	// music scope
	{
		auto zipPath = Folders::ASSETS + "music.zip";
		auto xmlText = zs.GetTextFile(zipPath, "music.xml");
		tinyxml2::XMLDocument xmlFile;
		auto error = xmlFile.Parse(xmlText.c_str());
		if (error != tinyxml2::XML_SUCCESS)
			throw std::exception("Music XML file is corrupted.");

		this->ReadMusic(xmlFile);
	}

}

uint32_t OggService::GetEnvironmentID(const std::string& envName)
{
	if (envName == "menu")
	{
		return (uint32_t)IEnvironment::e_menu;
	}
	else if (envName == "editor")
	{
		return (uint32_t)IEnvironment::e_editor;
	}
	else if (envName == "match")
	{
		return (uint32_t)IEnvironment::e_match;
	}
	return (uint32_t)IEnvironment::e_unknown;
}

void OggService::ReadMusic(tinyxml2::XMLDocument& xmlFile)
{
	tinyxml2::XMLElement* _music = xmlFile.FirstChildElement("music");
	assert(_music != nullptr);

	tinyxml2::XMLElement* _envs = _music->FirstChildElement("environments");
	if (_envs != nullptr)
	{
		for (tinyxml2::XMLElement* _env = _envs->FirstChildElement("environment"); _env != nullptr; _env = _env->NextSiblingElement())
		{
			auto envName = tinyxml2::TryParseStrAttribute(_env, "name");
			if (envName.empty()) continue;

			auto envID = this->GetEnvironmentID(envName);

			this->environmentPlaylist[envID] = std::vector<std::string>();

			tinyxml2::XMLElement* _playlist = _env->FirstChildElement("playlist");
			if (_playlist != nullptr)
			{
				for (tinyxml2::XMLElement* _song = _playlist->FirstChildElement("song"); _song != nullptr; _song = _song->NextSiblingElement())
				{
					std::string songName = _song->GetText();
					if (songName.empty()) continue;

					this->environmentPlaylist[envID].push_back(songName);
				}
			}
		}
	}
}

void OggService::AudioPlayer::Play(void) { }
void OggService::AudioPlayer::Stop(void) { }

OggService::Music::Music(uint8_t environment) : env(environment)
{
	this->bLoop = true;

	// temporary:
	if (OggService::GetInstance().environmentPlaylist.contains(this->env))
	{
		this->playlist = OggService::GetInstance().environmentPlaylist[env];
	}
}
OggService::Music::~Music(void)
{
	this->music.reset();
}
void OggService::Music::Play(void)
{
	if (this->playlist.empty()) return;

	auto& zs = rattlesmake::services::zip_service::get_instance();
	auto zipPath = Folders::ASSETS + "music.zip";
	zs.GetBytes(zipPath, this->playlist[this->currentSong], this->bytes);

	this->music = std::shared_ptr<sf::Music>(new sf::Music());
	this->music->openFromMemory(bytes.data(), sizeof(bytes[0]) * bytes.size());
	this->music->play();
}
void OggService::Music::Stop(void)
{
	if (this->music == nullptr)
		return;

	if (this->music->getStatus() == sf::Music::Status::Playing)
	{
		this->music->stop();
	}
}
void OggService::Music::Update(void)
{
	if (this->music == nullptr)
		this->Play(); // first song
	
	if (this->music->getStatus() == sf::Music::Status::Playing)
		return; // song is playing

	// song ended
	this->music.reset();

	if (this->currentSong == this->playlist.size() - 1)
		this->currentSong = 0;
	else
		this->currentSong++;
	
	// play next song
	this->Play();
}


void OggService::ObjectSound::Play(void) { }
void OggService::ObjectSound::Stop(void) { }

void OggService::NatureSound::Play(void) { }
void OggService::NatureSound::Stop(void) { }
#endif
