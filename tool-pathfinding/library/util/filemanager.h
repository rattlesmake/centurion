/*
* ---------------------------
* CLASS READER AND EDITOR FOR CENTURION
* [2019] - [2020] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <vector>
#include <string>

namespace FileManager
{
	// This function returns all files into all subfolders within a specified folder path.
	std::vector<std::string> GetAllFilesNamesWithinSubfolders(std::string const& folder_name, std::string const& file_extension = "");

	std::string GetFileFolderPath(const char* path);
};
