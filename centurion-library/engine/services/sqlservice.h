/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <list>
#include <optional>
#include <string>
#include <vector>
#include <unordered_map>

#include "centurion_typedef.hpp"

class SqlService
{
public:
	SqlService(const SqlService& other) = delete;
	SqlService& operator=(const SqlService& other) = delete;
	~SqlService(void);

	[[nodiscard]] static SqlService& GetInstance(void);

	/// <summary>
	/// This function returns the translation of a specific word by a query to database or by retriving it from the database cache if it is already stored.
	/// </summary>
	/// <param name="id">Id of string you want to translate.</param>
	/// <param name="bool bStoreInCache">Indicates if the translation must be stored into a "cache".</param>
	/// <returns>The translation of a specific word.</returns>
	[[nodiscard]] dbTranslation_t GetTranslation(const dbWord_t& id, const bool bStoreInCache);

	/// <summary>
	/// This function returns a list of translations from the database or by retriving them from the cache if they are already stored.
	/// </summary>
	/// <param name="std::list">List of words you want to translate.</param>
	/// <param name="bool bStoreInCache">Indicate if the list must be stored into the database cache.</param>
	/// <returns>The translation of a specific word.</returns>
	[[nodiscard]] std::unordered_map<dbWord_t, dbTranslation_t> GetTranslations(std::list<dbWord_t>&& wordsList, const bool bStoreInCache);

	/// <summary>
	/// This function provides all the available languages by a query to database. 
	/// </summary>
	/// <returns>All the available languages.</returns>
	[[nodiscard]] std::vector<std::string> GetAllLanguages(void) const;

	/// <summary>
	/// This function returns a string from the loading table, usually ready to be translated
	/// </summary>
	/// <param name="id">String id</param>
	/// <returns>The string associated to the id.</returns>
	[[nodiscard]] std::string GetStringFromLoadingTableById(const uint32_t id) const;

	/// This function resets the cache of the database. 
	/// It shoud be called when language is changed.
	/// </summary>
	void ResetTranslationCache(void);

	/// <summary>
	/// This function returns the specified context font.
	/// </summary>
	/// <param name="context">The context.</param>
	/// <returns>The name of the font.</returns>
	[[nodiscard]] std::string GetFontByContext(const std::string& context) const;
private:
	[[nodiscard]] std::optional<dbTranslation_t> TryToGetTranslationFromCache(const dbWord_t& word) const;
	void UpdateCache(dbWord_t id, dbTranslation_t value);
	SqlService(void);

	struct Cache
	{
		/*
			The unordered_map has a word as keyand, as value, has an iterator(i.e.a pointer) to the element of following list that contains the translation of the word.
			The list contains a pointer to the word (i.e. the key) stored into the map and the translation of the word.
			This two data structures allows deletion, search and insertion with complexity O(1).
			When the cache is full (i.e. when has MAX_DATABASE_CACHE_SIZE elements), will be removed the first element in the list (obviously, even the map will be updated).
			If a word is already in the cache and we access again to it, word will be moved to the end of the list (again, even the map will be updated).
		*/

		std::unordered_map<dbWord_t, std::list<std::pair<const dbWord_t*, dbTranslation_t>>::iterator> cacheIndex;
		std::list<std::pair<const dbWord_t*, dbTranslation_t>> cacheFIFO;
	};
	Cache cache;

	static SqlService instance;
};
