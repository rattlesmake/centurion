#include <services/logservice.h>
#include <fstream>
#include <settings.h>
#include <iostream>
#include <engine.h>
#include <fileservice.h>
#include <camera.h>
#include <viewport.h>
#include <mouse.h>

using namespace std;
using namespace glm;

namespace Logger 
{
	// Private members
	namespace
	{
		std::deque<LogMessage> Messages = std::deque<LogMessage>();
		std::string fileDebugName = "logs/logmessages/Debug " + rattlesmake::services::file_service::get_instance().get_current_datetime("%Y%m%d-%H%M%S") + ".xml";
		std::string fileParamsName = "logs/params/Params " + rattlesmake::services::file_service::get_instance().get_current_datetime("%Y%m%d-%H%M%S") + ".xml";

		/// <summary>
		/// This function adds an instance of a LogMessage into a LogMessages array.
		/// </summary>
		/// <param name="msg">This parameter adds a LogMessage into a LogMessages array.</param>
		void AddMessage(LogMessage msg)
		{
			Messages.push_back(msg);

			if (Messages.size() >= MAX_LOGGER_SIZE)
				Messages.pop_front();
		}

		/// <summary>
		/// This function prints a LogMessage text into debug console.
		/// </summary>
		/// <param name="msg">This parameter sets a LogMessage that should be printed into debug console.</param>
		void PrintLogMessage(LogMessage msg)
		{
			if (Settings::GetInstance().GetGlobalPreferences().IsDebugActive())
				cout << "[DEBUG] " << msg.text << endl;
		}
	};

	LogMessage::LogMessage(void) { }

	LogMessage::LogMessage(string txt, string typ, string nms, string clss, string mtd)
	{
		date = rattlesmake::services::file_service::get_instance().get_current_datetime("%Y/%m/%d - %X");
		type = typ;
		text = txt;
		cpp_stack = "";
		exception_type = "";
	}

	LogMessage::LogMessage(CenturionException* e)
	{
		date = rattlesmake::services::file_service::get_instance().get_current_datetime("%Y/%m/%d - %X");
		type = "Error";
		text = e->Message;
		cpp_stack = e->GetStack(4);
		exception_type = e->ExceptionType;
	}

	void Logger::Info(LogMessage msg)
	{
		Logger::AddMessage(msg);
		Logger::PrintLogMessage(msg);
		Logger::SaveDebugXML();
	}

	void Logger::Info(string msg)
	{
		LogMessage message = LogMessage(msg, "Info", "", "", "");
		Logger::AddMessage(message);
		Logger::PrintLogMessage(message);
		Logger::SaveDebugXML();
	}

	void Logger::Warn(LogMessage msg)
	{
		Logger::AddMessage(msg);
		Logger::PrintLogMessage(msg);
		Logger::SaveDebugXML();
	}

	void Logger::Warn(string msg)
	{
		LogMessage message = LogMessage(msg, "Warn", "", "", "");
		Logger::AddMessage(message);
		Logger::PrintLogMessage(message);
		Logger::SaveDebugXML();
	}

	void Logger::Error(LogMessage msg)
	{
		Logger::AddMessage(msg);
		Logger::PrintLogMessage(msg);
		Logger::SaveDebugXML();
	}

	void Logger::Error(string msg)
	{
		LogMessage message = LogMessage(msg, "Error", "", "", "");
		Logger::AddMessage(message);
		Logger::PrintLogMessage(message);
		Logger::SaveDebugXML();
	}

	void Error(CenturionException* ex)
	{
		LogMessage message = LogMessage(ex);
		Logger::AddMessage(message);
		Logger::PrintLogMessage(message);
		Logger::SaveDebugXML();
	}

	void Logger::CleanLogs(void)
	{
		try
		{
			std::string path1 = "logs/logmessages/";
			std::string path2 = "logs/params/";
			std::string path3 = "logs/durationlogger/";

			// checks if folders exist --> if not, create them
			if (rattlesmake::services::file_service::get_instance().check_if_folder_exists(path1) == false)
				rattlesmake::services::file_service::get_instance().create_folder(path1);

			if (rattlesmake::services::file_service::get_instance().check_if_folder_exists(path2) == false)
				rattlesmake::services::file_service::get_instance().create_folder(path2);

			vector<string> debugFiles = rattlesmake::services::file_service::get_instance().get_all_files_names_within_folder(path1);
			if (debugFiles.size() >= 10) 
			{
				uint8_t index = 0;
				rattlesmake::services::file_service::get_instance().delete_file(path1 + debugFiles[index++]);
			}
			vector<string> paramsFiles = rattlesmake::services::file_service::get_instance().get_all_files_names_within_folder(path2);
			if (paramsFiles.size() >= 10) 
			{
				uint8_t index = 0;
				rattlesmake::services::file_service::get_instance().delete_file(path2 + paramsFiles[index++]);
			}
			vector<string> durationFiles = rattlesmake::services::file_service::get_instance().get_all_files_names_within_folder(path3, "csv");
			if (durationFiles.size() > 0)
			{
				for (auto s : durationFiles)
				{
					rattlesmake::services::file_service::get_instance().delete_file(path3 + s);
				}
			}
		}
		catch (CenturionException* e)
		{
			e->AddFuncSignature(__FUNCSIG__);
			throw e;
		}
		catch (std::exception e)
		{
			EngineException* ex = new EngineException(e, __FUNCSIG__);
			throw ex->AsCenturionException();
		}
	}

	void Logger::SaveDebugXML(void)
	{
		return; // causes too lag

		try
		{
			//Saving all debug informations
			ofstream logFile(fileDebugName);

			if (logFile.is_open())
			{
				logFile << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << endl <<
					"<Log>" << endl <<
					"\t<LogMessages>" << endl;

				for (uint8_t i = 0; i < Messages.size(); i++)
				{
					logFile << "\t\t<LogMessage>" << endl;
					logFile << "\t\t\t<Date>" << Messages[i].date << "</Date>" << endl;
					if (!Messages[i].type.empty()) logFile << "\t\t\t<MessageType>" << Messages[i].type << "</MessageType>" << endl;
					if (!Messages[i].exception_type.empty()) logFile << "\t\t\t<ExceptionType>" << Messages[i].exception_type << "</ExceptionType>" << endl;
					if (!Messages[i].cpp_stack.empty()) logFile << "\t\t\t<Stack>" << endl << Messages[i].cpp_stack << endl << "\t\t\t</Stack>" << endl;
					if (!Messages[i].text.empty()) logFile << "\t\t\t<Text>" << Messages[i].text << "</Text>" << endl;
					logFile << "\t\t</LogMessage>" << endl;
				}

				logFile << "\t</LogMessages>" << endl;
				logFile << "</Log>" << endl;
			}
			logFile.close();
		}
		catch (CenturionException* e)
		{
			e->AddFuncSignature(__FUNCSIG__);
			throw e;
		}
		catch (std::exception e)
		{
			EngineException* ex = new EngineException(e, __FUNCSIG__);
			throw ex->AsCenturionException();
		}
	}

	void Logger::SaveParamsXML(void)
	{
		try
		{
			//Saving all parameters
			ofstream logFile(fileParamsName);
			if (logFile.is_open())
			{
				logFile << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<Log>\n\t<Params>\n" <<
					"\t\t<Camera>\n" <<
					"\t\t\t<xPosition>" << rattlesmake::peripherals::camera::get_instance().GetXPosition() << "</xPosition>\n" <<
					"\t\t\t<yPosition>" << rattlesmake::peripherals::camera::get_instance().GetYPosition() << "</yPosition>\n" <<
					"\t\t</Camera>\n" <<
					"\t\t<Mouse>\n" <<
					"\t\t\t<scrollValue>" << rattlesmake::peripherals::mouse::get_instance().ScrollValue << "</scrollValue>\n" <<
					"\t\t\t<xLeftClick>" << rattlesmake::peripherals::mouse::get_instance().GetXLeftClick() << "</xLeftClick>\n" <<
					"\t\t\t<xPosition>" << rattlesmake::peripherals::mouse::get_instance().GetXPosition() << "</xPosition>\n" <<
					"\t\t\t<xRightClick>" << rattlesmake::peripherals::mouse::get_instance().GetXRightClick() << "</xRightClick>\n" <<
					"\t\t\t<y2dPosition>" << rattlesmake::peripherals::mouse::get_instance().GetY2DPosition() << "</y2dPosition>\n" <<
					"\t\t\t<y2dRightClick>" << rattlesmake::peripherals::mouse::get_instance().GetY2DRightClick() << "</y2dRightClick>\n" <<
					"\t\t\t<yLeftClick>" << rattlesmake::peripherals::mouse::get_instance().GetYLeftClick() << "</yLeftClick>\n" <<
					"\t\t\t<yPosition>" << rattlesmake::peripherals::mouse::get_instance().GetYPosition() << "</yPosition>\n" <<
					"\t\t\t<yRightClick>" << rattlesmake::peripherals::mouse::get_instance().GetYRightClick() << "</yRightClick>\n" <<
					"\t\t\t<leftClick>" << rattlesmake::peripherals::mouse::get_instance().LeftClick << "</leftClick>\n" <<
					"\t\t\t<leftHold>" << rattlesmake::peripherals::mouse::get_instance().LeftHold << "</leftHold>\n" <<
					"\t\t\t<release>" << rattlesmake::peripherals::mouse::get_instance().Release << "</release>\n" <<
					"\t\t\t<rightClick>" << rattlesmake::peripherals::mouse::get_instance().RightClick << "</rightClick>\n" <<
					"\t\t\t<scrollBool>" << rattlesmake::peripherals::mouse::get_instance().ScrollBool << "</scrollBool>\n" <<
					"\t\t</Mouse>\n" <<
					"\t\t<Window>\n" <<
					"\t\t\t<heightZoomed>" << rattlesmake::peripherals::viewport::get_instance().GetHeightZoomed() << "</heightZoomed>\n" <<
					"\t\t\t<ratio>" << rattlesmake::peripherals::viewport::get_instance().GetViewportRatio() << "</ratio>\n" <<
					"\t\t\t<widthZoomed>" << rattlesmake::peripherals::viewport::get_instance().GetWidthZoomed() << "</widthZoomed>\n" <<
					"\t\t</Window>\n\t</Params>\n" <<
					"</Log>";
			}
			logFile.close();
		}
		catch (CenturionException* e)
		{
			e->AddFuncSignature(__FUNCSIG__);
			throw e;
		}
		catch (std::exception e)
		{
			EngineException* ex = new EngineException(e, __FUNCSIG__);
			throw ex->AsCenturionException();
		}
	}

	LogMessage::~LogMessage(void) { }
};