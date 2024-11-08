#include "filemanager.h"

#include <filesystem>
#include <algorithm>

namespace FileManager
{
	std::vector<std::string> GetAllFilesNamesWithinSubfolders(std::string const& folder_name, std::string const& file_extension)
	{
		std::vector<std::string> output;
		std::string type = file_extension;
		std::transform(type.begin(), type.end(), type.begin(), ::tolower);
		for (const auto& entry : std::filesystem::recursive_directory_iterator(folder_name)) {

			if (entry.is_directory()) continue;

			std::string fi = entry.path().string();
			std::string fullName = entry.path().filename().string();

			if (type == "*")
			{
				output.push_back(fi);
			}
			else
			{
				std::string fileExt = fullName.substr(fullName.find_last_of(".") + 1);
				std::transform(fileExt.begin(), fileExt.end(), fileExt.begin(), ::tolower);

				if (fileExt == type)
				{
					output.push_back(fi);
				}
			}
		}
		return output;
	}

	std::string FileManager::GetFileFolderPath(const char* path)
	{
		try
		{
			std::string path_str = (std::string)path;
			std::string::size_type pos = path_str.find_last_of("\\/");
			return path_str.substr(0, pos);
		}
		catch (std::exception e)
		{
			throw e;
		}
	}
};
