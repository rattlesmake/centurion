#include "dialogWindows.h"
#include <icons.h>
#include <engine.h>
#include <environments/game/editor/editor.h>
#include <services/pyservice.h>
#include <services/sqlservice.h>
#include <viewport.h>

#ifndef QUESTION_WINDOW_ANSWER_WAITING
#define QUESTION_WINDOW_ANSWER_WAITING 0
#endif 

#ifndef QUESTION_WINDOW_ANSWER_YES
#define QUESTION_WINDOW_ANSWER_YES 1
#endif 

#ifndef QUESTION_WINDOW_ANSWER_NO
#define QUESTION_WINDOW_ANSWER_NO 2
#endif

namespace gui
{
	class DialogWindow
	{
	public:
		explicit DialogWindow(std::string&& _message) : message(std::move(_message)) { }
		virtual unsigned int Render(void) { return 0; }
	protected:
		std::string message, functionYes, functionNo, title;
		std::function<void()> stdFunctionYes, stdFunctionNo;
		std::string yes = "Yes", no = "No";
		ImVec2 buttonSize{ 60, 20 };
		ImVec2 windowMinSize{ 350, 50 };
		ImVec2 windowSize = windowMinSize;
		ImVec2 windowMaxSize{ 350, 500 };
	};

	class InfoWindow : public DialogWindow
	{
	public:
		InfoWindow(std::string&& message, IEnvironment::Environments env);
		unsigned int Render(void) override;
		~InfoWindow() {}
	};

	class QuestionWindow : public DialogWindow
	{
	public:
		void SetTranslations(void);
		unsigned int Render(void) override;
		QuestionWindow(std::string&& message, std::string functionYes, std::string functionNo, IEnvironment::Environments env);
		QuestionWindow(std::string&& message, std::function<void()>& functionYes, std::function<void()>& functionNo, IEnvironment::Environments env);
		~QuestionWindow();
	private:
		IEnvironment::Environments env;
	};

	namespace
	{
		std::deque<QuestionWindow> QuestionWindows;
		std::deque<InfoWindow> InfoWindows;
		bool isAnyDialogWindowActive = false;
	}

	void gui::NewQuestionWindow(std::string&& message, std::string&& functionYes, std::string&& functionNo, IEnvironment::Environments env)
	{
		std::string translation = SqlService::GetInstance().GetTranslation(message, false);
		if (translation != "")
			message = std::move(translation);
		QuestionWindow wind{ std::move(message), functionYes, functionNo, env };
		wind.SetTranslations();
		QuestionWindows.push_back(wind);
		isAnyDialogWindowActive = true;
	}

	void NewQuestionWindow(std::string&& message, std::function<void()>& functionYes, std::function<void()>& functionNo, IEnvironment::Environments env)
	{
		std::string translation = SqlService::GetInstance().GetTranslation(message, false);
		if (translation != "")
			message = std::move(translation);
		QuestionWindow wind{ std::move(message), functionYes, functionNo, env };
		wind.SetTranslations();
		QuestionWindows.push_back(wind);
		isAnyDialogWindowActive = true;
	}

	void NewInfoWindow(std::string&& message, IEnvironment::Environments env)
	{
		std::string translation = SqlService::GetInstance().GetTranslation(message, false);
		if (translation != "")
			message = std::move(translation);

		InfoWindow wind{ std::move(message), env };
		InfoWindows.push_back(wind);
		isAnyDialogWindowActive = true;
	}

	void RenderDialogWindows(void)
	{
		if (isAnyDialogWindowActive == false) 
			return;

		if (QuestionWindows.empty() == false)
		{
			unsigned int response = QuestionWindows.front().Render();

			if (response != QUESTION_WINDOW_ANSWER_WAITING)
			{
				QuestionWindows.pop_front();
			}
		}

		if (InfoWindows.empty() == false)
		{
			unsigned int response = InfoWindows.front().Render();
			if (response != QUESTION_WINDOW_ANSWER_WAITING)
			{
				InfoWindows.pop_front();
			}
		}

		isAnyDialogWindowActive = InfoWindows.empty() == false || QuestionWindows.empty() == false;
	}

	bool IsAnyDialogWindowActive(void)
	{
		return isAnyDialogWindowActive;
	}

	bool IsQuestionWindowActive(void)
	{
		return (QuestionWindows.empty() == false);
	}

	void QuestionWindow::SetTranslations(void)
	{
		std::list<dbWord_t> translationsToGet{ "w_yes", "w_no" };
		auto translations = SqlService::GetInstance().GetTranslations(std::move(translationsToGet), false);
		this->yes = translations.at("w_yes");
		this->no = translations.at("w_no");
	}

	unsigned int QuestionWindow::Render(void)
	{
		unsigned int answer = QUESTION_WINDOW_ANSWER_WAITING;
		{
			ImGui::SetNextWindowSizeConstraints(this->windowMinSize, this->windowMaxSize);
			ImGui::SetNextWindowPos(ImVec2(rattlesmake::peripherals::viewport::get_instance().GetWidth() / 2 - this->windowMinSize.x / 2, rattlesmake::peripherals::viewport::get_instance().GetHeight() / 2 - this->windowMinSize.y));
			ImGui::Begin(("##" + this->title).c_str(), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_HideTitleBar);
			ImGui::SetCursorPos(ImVec2(4, 3));
			ImGui::PushFont(ImGui::GetFontByName(FONT_ICON_FILE_NAME_FA));
			ImGui::Text((const char*)GetIconUTF8Reference("ICON_QUESTION_MARK"));
			ImGui::PopFont();
			ImGui::SameLine();
			ImGui::PushFont(ImGui::GetFontByName(SqlService::GetInstance().GetFontByContext("QuestionWindow")));
			ImGui::Text(this->title.c_str());
			ImGui::Spacing(); ImGui::Spacing();
			ImGui::TextWrapped(this->message.c_str());
			ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
			if (ImGui::Button(this->yes.c_str(), this->buttonSize))
			{
				if (this->stdFunctionYes.operator bool() == true)
				{
					this->stdFunctionYes();
				}
				else 
				{
					PyService::GetInstance().GetInterpreter(PySecurityLevel::Admin).Evaluate(this->functionYes);
				}
				answer = QUESTION_WINDOW_ANSWER_YES;
			}
			ImGui::SameLine();
			if (ImGui::Button(this->no.c_str(), this->buttonSize))
			{
				if (this->stdFunctionNo.operator bool() == true)
				{
					this->stdFunctionNo();
				}
				else
				{
					PyService::GetInstance().GetInterpreter(PySecurityLevel::Admin).Evaluate(this->functionNo);
				}
				answer = QUESTION_WINDOW_ANSWER_NO;
			}
			ImGui::PopFont();
			ImGui::End();
		}

		return answer;
	}

	QuestionWindow::QuestionWindow(std::string&& _message, std::string functionYes, std::string functionNo, IEnvironment::Environments _env) :
		DialogWindow(std::move(_message)), env(_env)
	{
		this->functionYes = functionYes + PyService::GetInstance().GetArgs(this->env);
		this->functionNo = functionNo + PyService::GetInstance().GetArgs(this->env);
		switch (this->env)
		{
		case IEnvironment::Environments::e_unknown:
			this->title = SqlService::GetInstance().GetTranslation("w_question_window", false);
			break;
		case IEnvironment::Environments::e_menu:
			this->title = SqlService::GetInstance().GetTranslation("w_menu_question_window", false);
			break;
		case IEnvironment::Environments::e_editor:
			this->title = SqlService::GetInstance().GetTranslation("w_editor_question_window", false);
			break;
		case IEnvironment::Environments::e_match:
			this->title = SqlService::GetInstance().GetTranslation("w_match_question_window", false);
			break;
		case IEnvironment::Environments::e_mds:
			this->title = SqlService::GetInstance().GetTranslation("w_mds_question_window", false);
			break;
		}
	}
	QuestionWindow::QuestionWindow(std::string&& _message, std::function<void()>& functionYes, std::function<void()>& functionNo, IEnvironment::Environments _env)
		: DialogWindow(std::move(_message)), env(_env)
	{
		this->stdFunctionYes = functionYes;
		this->stdFunctionNo = functionNo;
		switch (this->env)
		{
		case IEnvironment::Environments::e_unknown:
			this->title = SqlService::GetInstance().GetTranslation("w_question_window", false);
			break;
		case IEnvironment::Environments::e_menu:
			this->title = SqlService::GetInstance().GetTranslation("w_menu_question_window", false);
			break;
		case IEnvironment::Environments::e_editor:
			this->title = SqlService::GetInstance().GetTranslation("w_editor_question_window", false);
			break;
		case IEnvironment::Environments::e_match:
			this->title = SqlService::GetInstance().GetTranslation("w_match_question_window", false);
			break;
		case IEnvironment::Environments::e_mds:
			this->title = SqlService::GetInstance().GetTranslation("w_mds_question_window", false);
			break;
		}
	}
	QuestionWindow::~QuestionWindow()
	{
	}

	InfoWindow::InfoWindow(std::string&& _message, IEnvironment::Environments env) :
		DialogWindow(std::move(_message))
	{
		switch (env)
		{
		case IEnvironment::Environments::e_unknown:
			this->title = SqlService::GetInstance().GetTranslation("w_infobox_window", false);
			break;
		case IEnvironment::Environments::e_menu:
			this->title = SqlService::GetInstance().GetTranslation("w_menu_infobox_window", false);
			break;
		case IEnvironment::Environments::e_editor:
			this->title = SqlService::GetInstance().GetTranslation("w_editor_infobox_window", false);
			break;
		case IEnvironment::Environments::e_match:
			this->title = SqlService::GetInstance().GetTranslation("w_match_infobox_window", false);
			break;
		case IEnvironment::Environments::e_mds:
			this->title = SqlService::GetInstance().GetTranslation("w_mds_infobox_window", false);
			break;
		}
	}

	unsigned int InfoWindow::Render(void)
	{
		ImGui::SetNextWindowSizeConstraints(this->windowMinSize, this->windowMaxSize);
		ImGui::SetNextWindowPos(ImVec2(rattlesmake::peripherals::viewport::get_instance().GetWidth() / 2 - this->windowMinSize.x / 2, rattlesmake::peripherals::viewport::get_instance().GetHeight() / 2 - this->windowMinSize.y));
		ImGui::Begin(("##" + this->title).c_str(), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_HideTitleBar);
		ImGui::SetCursorPos(ImVec2(4, 3));
		ImGui::PushFont(ImGui::GetFontByName(FONT_ICON_FILE_NAME_FA));
		ImGui::Text((const char*)GetIconUTF8Reference("ICON_EXCLAMATION_TRIANGLE"));
		ImGui::PopFont();
		ImGui::SameLine();
		ImGui::PushFont(ImGui::GetFontByName(SqlService::GetInstance().GetFontByContext("InfoWindow")));
		ImGui::Text(this->title.c_str());
		ImGui::Spacing(); ImGui::Spacing();
		ImGui::TextWrapped(this->message.c_str());
		ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
		if (ImGui::Button("Ok", this->buttonSize))
		{
			ImGui::PopFont();
			ImGui::End();
			return QUESTION_WINDOW_ANSWER_YES;
		}
		ImGui::PopFont();
		ImGui::End();
		return QUESTION_WINDOW_ANSWER_WAITING;
	}
};
