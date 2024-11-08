#include "fileservice.h"

// rattlesmake utils library
#include <encode_utils.h>

//#include <sys/stat.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <queue>
#include <ctime>
#include <filesystem>

namespace fs = std::filesystem;

namespace rattlesmake
{
	namespace services
	{
		file_service file_service::instance;
		file_service& file_service::get_instance(void)
		{
			return file_service::instance;
		}
		file_service::file_service()
		{
		}
		file_service::~file_service()
		{
		}
		void file_service::initialize(const char* exe_root, const std::map<std::string, std::string> keys_relativepaths)
		{
			this->exe_root = exe_root;
			this->main_root = file_service::get_file_folder_path(exe_root);
			Encode::NormalizePath(this->main_root);

			for (auto const& [key, val] : keys_relativepaths)
			{
				this->add_folder_shortcut(key, val);
			}
		}
		void file_service::add_folder_shortcut(const std::string& key, const std::string& relative_path)
		{
			std::string new_shortcut = this->main_root + "/" + relative_path;
			Encode::NormalizePath(new_shortcut);
			
			if (this->check_if_folder_exists(new_shortcut) == false)
				this->create_folder(new_shortcut);

			this->folders_shortcuts[key] = new_shortcut;
		}
		std::string file_service::get_folder_shortcut(const std::string& key) const
		{
			if (this->folders_shortcuts.contains(key) == false)
			{
				return "";
			}
			return this->folders_shortcuts.at(key);
		}
		std::string file_service::get_main_root(void) const
		{
			return this->main_root + "/"; 
		}

		std::string file_service::read_file(const char* fileLocation)
		{
			std::string content;
			std::ifstream fileStream(fileLocation, std::ios::in);
			if (!fileStream.is_open())
			{
				std::stringstream ss;

				ss << "Failed to read " << fileLocation << "! File doesn't exist.";
				//todo Logger::Warn(ss.str());
				return "";
			}
			std::string line = "";
			while (!fileStream.eof())
			{
				getline(fileStream, line);
				content.append(line + "\n");
			}
			fileStream.close();
			return content;
		}

		std::vector<std::string> file_service::get_all_files_names_within_folder(const std::string folder, std::string type)
		{
			std::vector<std::string> names;
			if (check_if_folder_exists(folder) == false)
			{
				//todo Logger::LogMessage msg = //todo Logger::LogMessage("Unable to find the folder \"" + folder + "\"", "Error", "Utils", "", __FUNCTION__);
				//todo Logger::Error(msg);
				return names;
			}
			transform(type.begin(), type.end(), type.begin(), ::tolower);
			for (const auto& entry : std::filesystem::directory_iterator(folder))
			{
				if (entry.is_directory())
					continue;

				std::string fileName = entry.path().filename().string();
				if (type == "*")
				{
					names.push_back(fileName);
				}
				else
				{
					std::string fileExt = fileName.substr(fileName.find_last_of(".") + 1);
					std::transform(fileExt.begin(), fileExt.end(), fileExt.begin(), ::tolower);
					if (fileExt == type)
					{
						names.push_back(fileName);
					}
				}
			}
			return names;
		}

		std::vector<std::string> file_service::get_all_folders_names_within_folder(const std::string folder)
		{
			std::vector<std::string> names;
			if (check_if_folder_exists(folder) == false)
			{
				//todo Logger::LogMessage msg = //todo Logger::LogMessage("Unable to find the folder \"" + folder + "\"", "Error", "Utils", "", __FUNCTION__);
				//todo Logger::Error(msg);
				return names;
			}

			for (const auto& entry : std::filesystem::directory_iterator(folder))
			{
				if (!entry.is_directory()) 
					continue;

				std::string fileName = entry.path().filename().string();
				names.push_back(fileName);
			}
			std::sort(names.begin(), names.end());
			return names;
		}

		std::string file_service::get_current_datetime(const char* format)
		{
			time_t     now = time(0);
			struct tm  tstruct;
			char       buf[80];
			tstruct = *localtime(&now);
			strftime(buf, sizeof(buf), format, &tstruct);
			return buf;
		}

		bool file_service::check_if_file_exists(const std::string& filePath, const bool relativePath)
		{
			return std::filesystem::exists(filePath);
		}

		bool file_service::check_if_folder_exists(std::string folderPath, const bool relativePath)
		{
			// struct stat info;
			if (relativePath == true)
				folderPath = this->main_root + "/" + std::move(folderPath);
			return fs::is_directory(folderPath);
			/*
			// old code if fs::is_directory does not work (needed tests)
			if (stat(folderPath.c_str(), &info) == 0)
				return true;
			return false;
			*/
		}

		bool file_service::delete_file(const std::string filePath)
		{
			return std::filesystem::remove(filePath);
		}

		void file_service::rename_file(const std::string oldFilePath, const std::string newFilePath)
		{
			std::filesystem::rename(oldFilePath, newFilePath);
		}

		bool file_service::copy_file(const std::string oldFilePath, const std::string newFilePath)
		{
			return std::filesystem::copy_file(oldFilePath, newFilePath);
		}

		bool file_service::create_folder(const std::string folderPath)
		{
			return std::filesystem::create_directories(folderPath);
		}

		void file_service::copy_folder(const std::string sourcePath, const std::string destinationPath)
		{
			if (check_if_folder_exists(destinationPath) == false)
				create_folder(destinationPath);

			std::filesystem::copy(sourcePath, destinationPath, std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);
		}

		bool file_service::delete_folder(const std::string folderPath)
		{
			std::filesystem::remove_all(folderPath);
			return true;
		}
		std::string file_service::get_file_folder_path(const std::string& path)
		{
			try
			{
				std::string::size_type pos = path.find_last_of("\\/");
				return path.substr(0, pos);
			}
			catch (std::exception& ex)
			{
				std::cout << "[DEBUG] error on get_file_folder_path. string: " << path << std::endl;
				throw ex;
			}
		}
		std::string file_service::get_file_folder_path(const char* path)
		{
			return get_file_folder_path(std::string(path));
		}
	};
};