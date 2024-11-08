#include "memory_logger.h"
#include <fstream>
#include <fileservice.h>


using namespace std;

#define PATH_1 "logs/memorylogger/constructions/" 
#define PATH_2 "logs/memorylogger/destructions/"

namespace MemoryLogger
{
	#pragma region Private variables:
	namespace
	{
		typedef std::vector<std::pair<const std::string, const std::string>> memoryLoggerParams;
		std::string fileDebugConstructionName = PATH_1 + rattlesmake::services::file_service::get_instance().get_current_datetime("%Y%m%d-%H%M%S") + ".xml";
		std::string fileDebugDestructionName = PATH_2 + rattlesmake::services::file_service::get_instance().get_current_datetime("%Y%m%d-%H%M%S") + ".xml";
		unsigned int constructionLogMessages = 0;
		unsigned int destructionLogMessages = 0;
	};
	#pragma endregion

	#pragma region Private functions:
	void ManageFolders(const std::string& path)
	{
		if (rattlesmake::services::file_service::get_instance().check_if_folder_exists(path) == false)
			rattlesmake::services::file_service::get_instance().create_folder(path);

		std::vector<std::string> files = rattlesmake::services::file_service::get_instance().get_all_files_names_within_folder(path);
		if (files.size() >= MAX_LOG_FILES)
		{
			unsigned idx = 0;
			rattlesmake::services::file_service::get_instance().delete_file(path + files[idx++]);
		}

	}
	#pragma endregion

	void AddMemoryConstructionLog(const std::string& _className, const unsigned int * const _address, const std::string& _creator, const memoryLoggerParams& _otherParams)
	{	
		ManageFolders(PATH_1);

		ofstream logFile(fileDebugConstructionName, ofstream::out | ofstream::app);
		if (logFile.is_open() == true)
		{
			MemoryLogger::constructionLogMessages += 1;
			if(MemoryLogger::constructionLogMessages == 1)
				logFile << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n" << endl;
			//Write new message:
			logFile << "<LogConstructorMessage>" << endl
					<< "\t<MessageNumber>" << MemoryLogger::constructionLogMessages << "</MessageNumber>" << endl
					<< "\t<ClassName>" << _className << "</ClassName>" << endl
					<< "\t<Address>" << _address << "</Address>" << endl
					<< "\t<Caller>" << _creator << "</Caller>" << endl;
			for (auto const& element : _otherParams)
				logFile << "\t<" << element.first << ">" << element.second << "</" << element.first << ">" << endl;
			logFile << "</LogDestructorMessage>" << endl;
		}
		logFile.close();
	}

	void AddMemoryDestructionLog(const std::string& _className, const unsigned int * const _address, const memoryLoggerParams& _otherParams)
	{
		ManageFolders(PATH_2);

		ofstream logFile(fileDebugDestructionName, ofstream::out | ofstream::app);
		if (logFile.is_open() == true)
		{
			MemoryLogger::destructionLogMessages += 1;
			if (MemoryLogger::destructionLogMessages == 1)
				logFile << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n" << endl;
			//Write new message:
			logFile << "<LogDestructorMessage>" << endl
					<< "\t<MessageNumber>" << MemoryLogger::destructionLogMessages << "</MessageNumber>" << endl
					<< "\t<ClassName>" << _className << "</ClassName>" << endl
					<< "\t<Address>" << _address << "</Address>" << endl;
			for (auto const& element : _otherParams)
				logFile << "\t<" << element.first << ">" << element.second << "</" << element.first << ">" << endl;
			logFile << "</LogDestructorMessage>" << endl;
		}
		logFile.close();
	}
};

