#include "sqlservice.h"

#include <settings.h>
#include <services/logservice.h>

#include <sqlite3.h>

#ifndef MAX_DATABASE_CACHE_SIZE
#define MAX_DATABASE_CACHE_SIZE       35
#endif // !MAX_DATABASE_CACHE_SIZE

#ifndef DEFAULT_FONT
#define DEFAULT_FONT		"tahoma_15"
#endif // !DEFAULT_FONT


#ifndef DATABASE_PATH
#define DATABASE_PATH       "assets/centurion.db"
#endif // !DATABASE_PATH


SqlService SqlService::instance;

SqlService::SqlService(void)
{
}
SqlService::~SqlService(void)
{
}

#pragma region Static members:
SqlService& SqlService::GetInstance(void)
{
	return SqlService::instance;
}
#pragma endregion

#pragma region Public members:
dbTranslation_t SqlService::GetTranslation(const dbWord_t& id, const bool bStoreInCache)
{
	if (id.empty() == true)
		return "";
	std::list<dbWord_t> word{ id };
	auto translations = this->GetTranslations(std::move(word), bStoreInCache);
	//ASSERTION: I must get at most one translation (since I have only a word)
	assert(translations.size() <= 1);
	return (translations.empty() == false) ? translations.begin()->second : id;
}

std::unordered_map<dbWord_t, dbTranslation_t> SqlService::GetTranslations(std::list<dbWord_t>&& wordsList, const bool bStoreInCache)
{
	std::unordered_map<dbWord_t, dbTranslation_t> translations;

	const size_t numberOfWordsToTranslate = wordsList.size();
	size_t translatedWords = 0;
	//Prepare where clause of the sql query and check if the cache has already some pair<word, translation>
	std::string whereStatement = "WHERE ID IN(";
	std::list<dbWord_t> wordsToTranslate;
	for (auto& word : wordsList)
	{
		if (word.size() >= 2 && (word[0] == '#' && word[1] == '#'))
		{
			translations.insert({ word, std::move(word) });
			translatedWords += 1;
		}
		else if (word.empty() == false)
		{
			auto translationInCache = this->TryToGetTranslationFromCache(word);
			if (translationInCache.has_value() == false)
			{
				//Translation isn't in the cache, so it have to be recovered from the db.
				whereStatement += '?' + std::to_string(wordsToTranslate.size() + 1) + ',';
				wordsToTranslate.push_back(std::move(word));
			}
			else
			{
				//Translation is in the cache, so use it without query the database.
				translations.insert({ std::move(word), std::move(translationInCache.value()) });
				translatedWords += 1;
			}
		}
		else //word is empty
		{
			translations.insert({ std::move(word), "" });
			translatedWords += 1;
		}
	}
	whereStatement.pop_back();
	whereStatement.append(");");

	if (wordsToTranslate.empty() == false)
	{
		//Get current language
		const std::string lang = Settings::GetInstance().GetGlobalPreferences().GetLanguage();

		int sqlResult = SQLITE_OK;
		sqlite3* centurionDB = nullptr; //Database handle

		//Open db connection
		sqlResult = sqlite3_open(DATABASE_PATH, &centurionDB);
		if (sqlResult != SQLITE_ERROR)
		{
			//Prepare query
			const std::string query = "SELECT ID," + lang + " FROM TRANSLATIONS " + std::move(whereStatement);
			sqlite3_stmt* statement = nullptr; //SQL statement
			sqlResult = sqlite3_prepare_v2(centurionDB, query.c_str(), -1, &statement, nullptr);

			if (sqlResult != SQLITE_ERROR)
			{
				//Bind params
				{
					int par = 1;
					for (auto const& word : wordsToTranslate)
					{
						sqlResult = sqlite3_bind_text(statement, par, word.c_str(), -1, SQLITE_STATIC);
						if (sqlResult == SQLITE_ERROR)
							break;
						par += 1;
					}
				}

				//Run query and get results
				while (sqlResult != SQLITE_ERROR && (sqlResult = sqlite3_step(statement)) == SQLITE_ROW)
				{
					dbWord_t currentWord = dbWord_t(reinterpret_cast<const char*>(sqlite3_column_text(statement, 0)));
					if (translations.contains(currentWord) == false)
					{
						dbTranslation_t currentTranslation = dbTranslation_t(reinterpret_cast<const char*>(sqlite3_column_text(statement, 1)));
						translations.insert({ std::move(currentWord), std::move(currentTranslation) });
						translatedWords += 1;
					}
					else
					{
						//Duplicate row
						Logger::LogMessage msg = Logger::LogMessage("Word \"" + std::move(currentWord) + "\" translation entry is multiple.", "Warn", "TranslationsTable", "", __FUNCTION__);
						Logger::Warn(msg);
					}
				}

				//Delete sql statement from memory
				sqlResult = sqlite3_finalize(statement); 
				statement = nullptr;
			}

			//Close db connection
			sqlResult = (sqlResult != SQLITE_ERROR) ? sqlite3_close(centurionDB) : sqlResult;
		}

		if (sqlResult == SQLITE_ERROR)
		{
			std::string err = sqlite3_errmsg(centurionDB);
			Logger::LogMessage msg = Logger::LogMessage("The error " + err + " occurred!", "Error", "TranslationsTable", "", __FUNCTION__);
			Logger::Error(msg);
		}
		centurionDB = nullptr;
	}

	//Finalize translations
	if (translatedWords != numberOfWordsToTranslate)
	{
		//If here, some translation was not found
		for (auto& word : wordsToTranslate)
		{
			if (translations.contains(word) == false)
			{
				//If here, word wasn't found in the database
				Logger::LogMessage msg = Logger::LogMessage("Word \"" + word + "\" translation entry is missing!", "Warn", "TranslationsTable", "", __FUNCTION__);
				Logger::Warn(msg);
				translations.insert({ word, std::move(word) });
			}
		}
	}

	//Update cache
	if (bStoreInCache == true)
	{
		for (auto const& [word, translation] : translations)
		{
			this->UpdateCache(word, translation);
		}
	}

	return translations;
}

std::vector<std::string> SqlService::GetAllLanguages(void) const
{
	std::vector<string> availableLanguages;
	int sqlResult = SQLITE_OK;
	sqlite3* centurionDB = nullptr; //Database handle

	//Open db connection
	sqlResult = sqlite3_open(DATABASE_PATH, &centurionDB);
	if (sqlResult == SQLITE_OK) 
	{
		//Prepare query
		const std::string query = "PRAGMA table_info('TRANSLATIONS')";
		sqlite3_stmt* statement = nullptr; //SQL statement
		sqlResult = sqlite3_prepare_v2(centurionDB, query.c_str(), -1, &statement, nullptr);
	
		//Run query and get results
		if (sqlResult != SQLITE_ERROR)
		{
			uint32_t numberOfRow = 0;
			while ((sqlResult = sqlite3_step(statement)) == SQLITE_ROW)
			{
				if (numberOfRow >= 1)
				{
					std::string lang = std::string(reinterpret_cast<const char*>(sqlite3_column_text(statement, 1)));
					availableLanguages.push_back(std::move(lang));
				}
				numberOfRow++;
			}

			//Delete sql statement from memory
			sqlResult = sqlite3_finalize(statement); 
			statement = nullptr;
		}

		//Close db connection
		sqlResult = (sqlResult != SQLITE_ERROR) ? sqlite3_close(centurionDB) : sqlResult;
	}

	if (sqlResult == SQLITE_ERROR)
	{
		std::string err = sqlite3_errmsg(centurionDB);
		Logger::LogMessage msg = Logger::LogMessage("The error " + err + " occurred!", "Error", "TranslationsTable", "", __FUNCTION__);
		Logger::Error(msg);
	}
	centurionDB = nullptr;

	return availableLanguages;
}

std::string SqlService::GetStringFromLoadingTableById(const uint32_t id) const
{
	std::string word;
	int sqlResult = SQLITE_OK;
	sqlite3* centurionDB = nullptr; //Database handle

	//Open db connection
	sqlResult = sqlite3_open(DATABASE_PATH, &centurionDB);
	if (sqlResult == SQLITE_OK)
	{
		//Prepare query
		const std::string query = "SELECT String FROM LOADING_ELEMENTS WHERE ID = ?1";
		sqlite3_stmt* statement = nullptr; //SQL statement
		sqlResult = sqlite3_prepare_v2(centurionDB, query.c_str(), -1, &statement, nullptr);

		if (sqlResult != SQLITE_ERROR)
		{
			//Bind params
			sqlResult = sqlite3_bind_int(statement, 1, id);

			//Run query and get results
			if (sqlResult != SQLITE_ERROR)
			{
				uint32_t numberOfRow = 0;
				while ((sqlResult = sqlite3_step(statement)) == SQLITE_ROW)
				{
					numberOfRow += 1;
					if (numberOfRow > 1)
					{
						Logger::LogMessage msg = Logger::LogMessage("Multiple occurences of string with id " + id, "Warn", "TranslationsTable", "", __FUNCTION__);
						Logger::Warn(msg);
						break;
					}
					word = std::string(reinterpret_cast<const char*>(sqlite3_column_text(statement, 0)));
				}
			}

			//Delete sql statement from memory
			sqlResult = sqlite3_finalize(statement);
			statement = nullptr;

			//Close db connection
			sqlResult = (sqlResult != SQLITE_ERROR) ? sqlite3_close(centurionDB) : sqlResult;
		}
	}

	if (sqlResult == SQLITE_ERROR)
	{
		std::string err = sqlite3_errmsg(centurionDB);
		Logger::LogMessage msg = Logger::LogMessage("The error " + err + " occurred!", "Error", "TranslationsTable", "", __FUNCTION__);
		Logger::Error(msg);
	}
	else if (word == "")
	{
		Logger::LogMessage msg = Logger::LogMessage("id " + std::to_string(id) + "has no word", "Warn", "TranslationsTable", "", __FUNCTION__);
		Logger::Warn(msg);
	}
	centurionDB = nullptr;

	return word;
}

void SqlService::ResetTranslationCache(void)
{
	this->cache.cacheIndex.clear();
	this->cache.cacheFIFO.clear();
}

std::string SqlService::GetFontByContext(const std::string& context) const
{
	if (context.empty() == true)
		return DEFAULT_FONT;

	std::string font;
	int sqlResult = SQLITE_OK;
	sqlite3* centurionDB = nullptr; //Database handle
	//Open db connection
	sqlResult = sqlite3_open(DATABASE_PATH, &centurionDB);
	if (sqlResult == SQLITE_OK)
	{
		//Prepare query
		const std::string query = "SELECT FontName FROM FONTS WHERE Context = ?1 COLLATE NOCASE;";
		sqlite3_stmt* statement = nullptr; //SQL statement
		sqlResult = sqlite3_prepare_v2(centurionDB, query.c_str(), -1, &statement, nullptr);

		if (sqlResult != SQLITE_ERROR)
		{
			//Bind params
			sqlResult = sqlite3_bind_text(statement, 1, context.c_str(), -1, SQLITE_STATIC);

			//Run query and get results
			if (sqlResult != SQLITE_ERROR)
			{
				uint32_t numberOfRow = 0;
				while ((sqlResult = sqlite3_step(statement)) == SQLITE_ROW)
				{
					numberOfRow += 1;
					if (numberOfRow > 1)
					{
						Logger::LogMessage msg = Logger::LogMessage("There are multiple fonts for the context \"" + context + "\"", "Warn", "TranslationsTable", "", __FUNCTION__);
						Logger::Warn(msg);
						break;
					}
					font = std::string(reinterpret_cast<const char*>(sqlite3_column_text(statement, 0)));
				}
			}

			//Delete sql statement from memory
			sqlResult = sqlite3_finalize(statement);
			statement = nullptr;

			//Close db connection
			sqlResult = (sqlResult != SQLITE_ERROR) ? sqlite3_close(centurionDB) : sqlResult;
		}
	}
	if (sqlResult == SQLITE_ERROR)
	{
		std::string err = sqlite3_errmsg(centurionDB);
		Logger::LogMessage msg = Logger::LogMessage("The error " + err + " occurred!", "Error", "TranslationsTable", "", __FUNCTION__);
		Logger::Error(msg);
	}
	else if (font == "") 
	{
		//Missing font name for the context.
		Logger::LogMessage msg = Logger::LogMessage("Font of the context \"" + context + "\" is missing. Consequently, default font was retrieved", "Warn", "TranslationsTable", "", __FUNCTION__);
		Logger::Warn(msg);
	}

	return (font.empty() == false) ? font : DEFAULT_FONT;
}
#pragma endregion

#pragma region Private members:
std::optional<dbTranslation_t> SqlService::TryToGetTranslationFromCache(const dbWord_t& word) const
{
	return (this->cache.cacheIndex.contains(word) == true) ? (*this->cache.cacheIndex.at(word)).second : std::optional<dbTranslation_t>();
}

void SqlService::UpdateCache(dbWord_t id, dbTranslation_t translation)
{
	//ASSERTION: the size of the two cache data structures must be equals.
	assert(this->cache.cacheIndex.size() == this->cache.cacheFIFO.size());

	if (this->cache.cacheIndex.contains(id) == true) //Word or translation is already in the cache
	{
		const dbWord_t& key = this->cache.cacheIndex.find(id)->first;
		auto& iterator = this->cache.cacheIndex.at(id);
		this->cache.cacheFIFO.erase(iterator);
	}
	else if (this->cache.cacheFIFO.size() == MAX_DATABASE_CACHE_SIZE)
	{
		//Cache is full, so firstly remove the first entry from the cache
		this->cache.cacheIndex.erase((*this->cache.cacheFIFO.front().first));
		this->cache.cacheFIFO.pop_front();
	}

	//Push the translation into the cache
	this->cache.cacheIndex[id]; //Add the id (if missing)
	const dbWord_t& key = this->cache.cacheIndex.find(id)->first; //Get a reference to the stored id.
	this->cache.cacheFIFO.push_back({ &key, std::move(translation) });
	this->cache.cacheIndex[id] = --this->cache.cacheFIFO.end();

	//ASSERTION: the size of the two cache data structures must be equals.
	assert(this->cache.cacheIndex.size() == this->cache.cacheFIFO.size());
}
#pragma endregion
