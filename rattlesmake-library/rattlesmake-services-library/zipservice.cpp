#include "zipservice.h"
#include "fileservice.h"

// rattlesmake utils library
#include <encode_utils.h>

#include <sstream>
#include <fstream>
#include <ZipFile.h>
#include <png.h>
#include <unordered_map>
#include <assert.h>

namespace rattlesmake
{
	namespace services
	{
		// private functions
		namespace
		{
			std::unordered_map<std::string, ZipArchive::Ptr> CACHE;

			std::string ImageBuffer = "";
			void ImageBufferToString(void* context, void* data, int len)
			{
				ImageBuffer.resize(len);
				memcpy(&ImageBuffer[0], data, len);
			}

			ZipArchive::Ptr* GetZipArchive(const std::string& path)
			{
				if (CACHE.contains(path)) return &(CACHE[path]);
				CACHE[path] = ZipFile::Open(path);
				return &(CACHE[path]);
			}
			void DeleteZipFromCache(const std::string& path)
			{
				if (CACHE.contains(path)) CACHE.erase(path);
			}
		};

		zip_service zip_service::instance;
		zip_service& zip_service::get_instance(void)
		{
			return instance;
		}
		zip_service::zip_service() { }
		zip_service::~zip_service() { }

		#pragma region Miscellaneous
		void zip_service::clear_cache(void)
		{
			if (CACHE.empty())
				return;
			CACHE.clear();
		}
		void zip_service::initialize(const std::map<std::string, std::string> keys_paths)
		{
			for (auto const& [key, val] : keys_paths)
			{
				if (file_service::get_instance().check_if_file_exists(val, false) == false)
				{
					std::cout << "[DEBUG] " << key << " doesn't exist" << std::endl;
				}
				this->add_zipfile_shortcut(key, val);
			}
		}
		void zip_service::add_zipfile_shortcut(const std::string& key, std::string path)
		{
			Encode::NormalizePath(path);
			this->zipfiles_shortcuts[key] = path;
		}
		std::string zip_service::get_zipfile_shortcut(const std::string& key) const
		{
			if (this->zipfiles_shortcuts.contains(key) == false)
			{
				std::cout << "[DEBUG] " << key << " not in zip shortcuts" << std::endl;
			}
			return this->zipfiles_shortcuts.at(key);
		}
		#pragma endregion

		#pragma region Checks
		bool zip_service::check_if_file_exists(const std::string& zipKey, std::string internalFilePath)
		{
			const std::string& zip_path = this->get_zipfile_shortcut(zipKey);
			auto archive = GetZipArchive(zip_path);

			Encode::NormalizePath(internalFilePath);
			ZipArchiveEntry::Ptr entry = (*archive)->GetEntry(internalFilePath);
			bool result = (entry != nullptr);
			DeleteZipFromCache(zip_path);
			return result;
		}
		bool zip_service::check_if_folder_exists(const std::string& zipKey, std::string internalFolderPath)
		{
			const std::string& zip_path = this->get_zipfile_shortcut(zipKey);
			auto archive = GetZipArchive(zip_path);

			Encode::NormalizePath(internalFolderPath);
			auto listOfFiles = (*archive)->GetEntriesInInternalFolder(internalFolderPath, "*");
			bool result = listOfFiles.size() > 0;
			DeleteZipFromCache(zip_path);
			return result;
		}
		#pragma endregion

		#pragma region Get from zip
		std::vector<std::string> zip_service::get_all_folders_within_folder(const std::string& zipKey, const std::string& folderName, const bool cacheZip)
		{
			const std::string& zip_path = this->get_zipfile_shortcut(zipKey);
			auto archive = GetZipArchive(zip_path);

			std::vector<std::string> entriesList = (*archive)->GetEntriesInInternalFolder(folderName + "/", "*");
			std::vector<std::string> foldersList;

			for (auto const& i : entriesList)
			{
				size_t nameLength = folderName.size() + 1;
				std::string j = i.substr(nameLength);
				size_t finalCharPos = j.find("/");
				j = j.substr(0, finalCharPos);
				foldersList.push_back(j);
			}
			foldersList.resize(std::distance(foldersList.begin(), std::unique(foldersList.begin(), foldersList.end())));

			if (cacheZip == false) DeleteZipFromCache(zip_path);
			return foldersList;
		}
		std::vector<std::string> zip_service::get_all_files_within_folder(const std::string& zipKey, std::string targetFileInternalPath, const std::string& extension, const bool cacheZip)
		{
			const std::string& zip_path = this->get_zipfile_shortcut(zipKey);
			auto archive = GetZipArchive(zip_path);

			auto list = (*archive)->GetEntriesInInternalFolder(targetFileInternalPath, extension);
			if (cacheZip == false) DeleteZipFromCache(zip_path);
			return list;
		}
		std::string zip_service::get_text_file(const std::string& zipKey, std::string targetFileInternalPath, const bool cacheZip)
		{
			const std::string& zip_path = this->get_zipfile_shortcut(zipKey);
			auto archive = GetZipArchive(zip_path);

			Encode::NormalizePath(targetFileInternalPath);
			ZipArchiveEntry::Ptr entry = (*archive)->GetEntry(targetFileInternalPath);
			if (entry == nullptr)
				return "";

			if (entry->IsPasswordProtected() == true)
			{
				entry->SetPassword("pongolezzomdsmona");
			}

			std::istream* decompressStream = entry->GetDecompressionStream();
			if (decompressStream != nullptr)
			{
				std::string s(std::istreambuf_iterator<char>(*decompressStream), {});
				if (cacheZip == false)
					DeleteZipFromCache(zip_path);
				return s;
			}
			if (cacheZip == false)
				DeleteZipFromCache(zip_path);
			return "";
		}
		void zip_service::get_text_file_stream(const std::string& zipKey, std::string targetFileInternalPath, std::stringstream& streamBuf, const bool cacheZip)
		{
			const std::string& zip_path = this->get_zipfile_shortcut(zipKey);
			auto archive = GetZipArchive(zip_path);

			Encode::NormalizePath(targetFileInternalPath);
			ZipArchiveEntry::Ptr entry = (*archive)->GetEntry(targetFileInternalPath);
			if (entry == nullptr)
				return;

			if (entry->IsPasswordProtected() == true)
			{
				entry->SetPassword("pongolezzomdsmona");
			}

			std::istream* decompressStream = entry->GetDecompressionStream();
			if (decompressStream != nullptr)
			{
				std::string s(std::istreambuf_iterator<char>(*decompressStream), {});
				if (cacheZip == false)
					DeleteZipFromCache(zip_path);
				streamBuf << std::move(s);
				return;
			}
			if (cacheZip == false)
				DeleteZipFromCache(zip_path);
			return;
		}
		void zip_service::get_bytes(const std::string& zipKey, std::string targetFileInternalPath, std::vector<char>& bytes, const bool cacheZip)
		{
			const std::string& zip_path = this->get_zipfile_shortcut(zipKey);
			auto archive = GetZipArchive(zip_path);

			Encode::NormalizePath(targetFileInternalPath);
			ZipArchiveEntry::Ptr entry = (*archive)->GetEntry(targetFileInternalPath);
			if (entry == nullptr)
				return;

			if (entry->IsPasswordProtected() == true)
			{
				entry->SetPassword("pongolezzomdsmona");
			}

			std::istream* decompressStream = entry->GetDecompressionStream();
			if (decompressStream != nullptr)
			{
				//std::string s(std::istreambuf_iterator<char>(*decompressStream), {});
				std::vector<char> byte(std::istreambuf_iterator<char>(*decompressStream), {});
				bytes = std::move(byte);

				if (cacheZip == false)
					DeleteZipFromCache(zip_path);
				//streamBuf << std::move(s);
				return;
			}
			if (cacheZip == false)
				DeleteZipFromCache(zip_path);
			return;
		}
		unsigned char* zip_service::get_image_data(const std::string& zipKey, std::string targetFileInternalPath, int* width, int* height, int* nChannels, int req_comp, const bool cacheZip)
		{
			const std::string& zip_path = this->get_zipfile_shortcut(zipKey);
			auto archive = GetZipArchive(zip_path);

			Encode::NormalizePath(targetFileInternalPath);
			ZipArchiveEntry::Ptr entry = (*archive)->GetEntry(targetFileInternalPath);
			if (entry == nullptr)
				return nullptr;

			if (entry->IsPasswordProtected() == true)
			{
				entry->SetPassword("pongolezzomdsmona");
			}

			std::istream* decompressStream = entry->GetDecompressionStream();
			if (decompressStream != nullptr)
			{
				std::string s(std::istreambuf_iterator<char>(*decompressStream), {});
				if (cacheZip == false)
					DeleteZipFromCache(zip_path);
				return rattlesmake::image::stb::load_from_memory((unsigned char*)s.data(), int(s.size() * sizeof(unsigned char)), width, height, nChannels, req_comp);
			}
			if (cacheZip == false)
				DeleteZipFromCache(zip_path);
			return nullptr;
		}
		#pragma endregion

		#pragma region Add to zip
		void zip_service::add_file(const std::string& zipKey, std::string targetFileInternalPath, std::stringstream& fileContentStream)
		{
			const std::string& zip_path = this->get_zipfile_shortcut(zipKey);
			auto archive = GetZipArchive(zip_path);

			Encode::NormalizePath(targetFileInternalPath);
			assert(archive != nullptr);

			ZipArchiveEntry::Ptr entry = (*archive)->GetEntry(targetFileInternalPath);

			if (entry == nullptr)
			{
				entry = (*archive)->CreateEntry(targetFileInternalPath);
			}
			else
			{
				(*archive)->RemoveEntry(entry->GetFullName());
				entry = (*archive)->CreateEntry(targetFileInternalPath);
			}
			{
				DeflateMethod::Ptr ctx = DeflateMethod::Create();
				ctx->SetCompressionLevel(DeflateMethod::CompressionLevel::L1);

				entry->SetCompressionStream(   // data from contentStream are pumped here
					fileContentStream,
					ctx,
					ZipArchiveEntry::CompressionMode::Immediate
				);
				// ifs stream is destroyed here
			}
		}
		void zip_service::add_image(const std::string& zipKey, std::string targetFileInternalPath, unsigned char* imageData, int width, int height, int nChannels)
		{
			rattlesmake::image::stb::flip_vertically_on_write(1);
			rattlesmake::image::stb::write_png_to_func(ImageBufferToString, 0, width, height, nChannels, imageData, 0);
			std::stringstream imageBufferStream{ std::move(ImageBuffer) };
			add_file(zipKey, targetFileInternalPath, imageBufferStream);
			rattlesmake::image::stb::flip_vertically_on_write(0);
		}
		void zip_service::remove_folder(const std::string& zipKey, std::string targetFolderInternalPath)
		{
			const std::string& zip_path = this->get_zipfile_shortcut(zipKey);

			Encode::NormalizePath(targetFolderInternalPath);
			auto archive = GetZipArchive(zip_path);
			auto list = (*archive)->GetEntriesInInternalFolder(targetFolderInternalPath, "*");
			for (auto& el : list)
			{
				(*archive)->RemoveEntry(el);
			}
			ZipFile::SaveAndClose(*archive, zip_path);
			DeleteZipFromCache(zip_path);
		}
		void zip_service::remove_file(const std::string& zipKey, std::string targetFileInternalPath)
		{
			const std::string& zip_path = this->get_zipfile_shortcut(zipKey);

			Encode::NormalizePath(targetFileInternalPath);
			auto archive = GetZipArchive(zip_path);
			ZipArchiveEntry::Ptr entry = (*archive)->GetEntry(targetFileInternalPath);
			if (entry == nullptr)
				return;
			(*archive)->RemoveEntry(entry->GetFullName());
			ZipFile::SaveAndClose(*archive, zipKey);
			DeleteZipFromCache(zip_path);
		}
		#pragma endregion

		#pragma region Save
		void zip_service::save_and_close(const std::string& zipKey)
		{
			const std::string& zip_path = this->get_zipfile_shortcut(zipKey);

			auto archive = GetZipArchive(zip_path);
			ZipFile::SaveAndClose(*archive, zip_path);
			DeleteZipFromCache(zip_path);
		}
		#pragma endregion
	};
};
