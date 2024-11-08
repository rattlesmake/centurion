/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <string>
#include <map>
#include <unordered_map>
#include <vector>

namespace rattlesmake
{
	namespace services
	{
		// Service that wraps ZipLib library
		class zip_service
		{
		public:

			/*
				This function has to be called in Init function of
				your engine instance (before while loop)
			*/
			void initialize(const std::map<std::string, std::string> keys_paths);


			#pragma region Miscellaneous
			void clear_cache(void);
			void add_zipfile_shortcut(const std::string& key, std::string path);
			std::string get_zipfile_shortcut(const std::string& key) const;
			#pragma endregion

			#pragma region Checks
			bool check_if_file_exists(const std::string& zipKey, std::string internalFilePath);
			bool check_if_folder_exists(const std::string& zipKey, std::string internalFolderPath);
			#pragma endregion

			#pragma region Read from zip
			[[nodiscard]] std::vector<std::string> get_all_files_within_folder(const std::string& zipKey, std::string targetFileInternalPath, const std::string& extension = "*", const bool cacheZip = false);
			[[nodiscard]] std::string get_text_file(const std::string& zipKey, std::string targetFileInternalPath, const bool cacheZip = false);
			void get_text_file_stream(const std::string& zipKey, std::string targetFileInternalPath, std::stringstream& streamBuf, const bool cacheZip = false);
			void get_bytes(const std::string& zipKey, std::string targetFileInternalPath, std::vector<char>& bytes, const bool cacheZip = false);
			[[nodiscard]] unsigned char* get_image_data(const std::string& zipKey, std::string targetFileInternalPath, int* width, int* height, int* nChannels, int req_comp, const bool cacheZip = false);
			[[nodiscard]] std::vector<std::string> get_all_folders_within_folder(const std::string& zipKey, const std::string& folderName, const bool cacheZip = false);
			#pragma endregion

			#pragma region Add to zip
			void add_file(const std::string& zipKey, std::string targetFileInternalPath, std::stringstream& fileContentStream);
			void add_image(const std::string& zipKey, std::string targetFileInternalPath, unsigned char* imageData, int width, int height, int nChannels);
			#pragma endregion

			#pragma region Remove from zip
			void remove_folder(const std::string& zipKey, std::string targetFolderInternalPath);
			void remove_file(const std::string& zipKey, std::string targetFileInternalPath);
			#pragma endregion


			#pragma region Save zip
			void save_and_close(const std::string& zipKey);
			#pragma endregion

			#pragma region Singleton
			///Copy constructor and operator = are not allowed in order to avoid accidentally copies of the singleton appearing.
			zip_service(const zip_service& other) = delete;
			zip_service& operator=(zip_service const& other) = delete;
			[[nodiscard]] static zip_service& get_instance(void);
			~zip_service();
			#pragma endregion
		private:
			zip_service();
			
			std::unordered_map<std::string, std::string> zipfiles_shortcuts;

			static zip_service instance;
		};
	};
};
