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
		class file_service
		{
		public:

			/*
				This function has to be called in Init function of
				your engine instance (before while loop)
			*/
			void initialize(const char* exe_root, const std::map<std::string, std::string> keys_relativepaths);
			


			void add_folder_shortcut(const std::string& key, const std::string& relative_path);
			[[nodiscard]] std::string get_folder_shortcut(const std::string& key) const;
			[[nodiscard]] std::string get_main_root(void) const;

			/// <summary>
			/// This function reads and returns a text file content.
			/// </summary>
			/// <param name="fileLocation">Text file path. Supports only char* variables or std::strings converted to c_str.</param>
			std::string read_file(const char* fileLocation);

			/// <summary>
			/// This function returns all files within a specified folder path.
			/// </summary>
			/// <param name="folder">Folder path. Supports std::strings.</param>
			/// <param name="type">The file extension required. Default value is "*", which means that every file extension it will be researched for.</param>
			std::vector<std::string> get_all_files_names_within_folder(const std::string folder, std::string type = "*");

			/// <summary>
			/// This function returns all folders within a specified folder path.
			/// </summary>
			/// <param name="folder">Folder path. Supports std::strings.</param>
			std::vector<std::string> get_all_folders_names_within_folder(const std::string folder);

			/// <summary>
			/// This function returns current date and time.
			/// </summary>
			/// <param name="format">This parameter, which supports std::strings, can be customized by using the following variables (no specified requirement):
			/// %Y = Current year
			/// %m = Current month
			/// %d = Current day
			/// %H = Current hour
			/// %M = Current minute
			/// %S = Current seconds
			/// E.g. => %d\/%m\/%Y	01/01/1980.</param>
			std::string get_current_datetime(const char* format);

			/// <summary>
			/// This boolean function checks if a precise file exists in a specified folder path.
			/// </summary>
			/// <param name="filePath">File path. Supports std::strings</param>
			/// <param name="relativePath">Parameter to check if path is relative or not. Supports boolean values</param>
			bool check_if_file_exists(const std::string& filePath, const bool relativePath = false);

			/// <summary>
			/// This boolean function checks if a precise folder exists in a specified folder path.
			/// </summary>
			/// <param name="folderPath">Folder path. Supports std::strings</param>
			/// <param name="relativePath">Parameter to check if path is relative or not. Supports boolean values</param>
			bool check_if_folder_exists(std::string folderPath, const bool relativePath = false);

			/// <summary>
			/// This function removes a file into a specified path.
			/// </summary>
			/// <param name="filePath">File path. Supports std::strings.</param>
			bool delete_file(const std::string filePath);

			void rename_file(const std::string oldFilePath, const std::string newFilePath);

			bool copy_file(const std::string oldFilePath, const std::string newFilePath);

			/// <summary>
			/// This function creates a folder into a specified path.
			/// </summary>
			/// <param name="folderPath">Folder path. Supports std::strings.</param>
			bool create_folder(const std::string folderPath);

			/// <summary>
			/// This function duplicates a folder into a specified destination path.
			/// </summary>
			/// <param name="sourcePath">Source folder path. Supports std::strings.</param>
			void copy_folder(const std::string sourcePath, const std::string destinationPath);

			/// <summary>
			/// This function deletes a folder from a specified path.
			/// </summary>
			/// <param name="folderPath">Folder path. Supports std::strings.</param>
			bool delete_folder(const std::string folderPath);

			/// <summary>
			/// This function returns the relative file folder path.
			/// </summary>
			/// <param name="*path">Folder path. Supports only referenced char variables or std::strings converted to c_str.</param>
			std::string get_file_folder_path(const char* path);
			std::string get_file_folder_path(const std::string& path);

			#pragma region Singleton
			///Copy constructor and operator = are not allowed in order to avoid accidentally copies of the singleton appearing.
			file_service(const file_service& other) = delete;
			file_service& operator=(file_service const& other) = delete;
			[[nodiscard]] static file_service& get_instance(void);
			~file_service();
			#pragma endregion

		private:
			file_service();
			

			std::unordered_map<std::string, std::string> folders_shortcuts;
			std::string exe_root;
			std::string main_root;

			static file_service instance;
		};
	};
};


