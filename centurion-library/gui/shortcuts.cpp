#include "shortcuts.h"

#include <engine.h>
#include <keyboard.h>


gui::Shortcuts::Shortcuts(tinyxml2::XMLElement* el, IEnvironment::Environments _env) : env(_env)
{
	for (tinyxml2::XMLElement* child = el->FirstChildElement("shortcut"); child != nullptr; child = child->NextSiblingElement())
	{
		gui::Shortcuts::Shortcut shortcut{ child };
		if (shortcut.IsOk() == true)
			this->shortcuts.push_back(std::move(shortcut));
	}
}

gui::Shortcuts::~Shortcuts(void)
{
}

void gui::Shortcuts::SetOpenIframeFun(std::function<bool(std::string& iframeToOpen)> _function)
{
	this->openIframeFun = std::move(_function);
}

void gui::Shortcuts::Run(void)
{	
	// if you are writing into a textarea don't execute shortcuts
	if (ImGui::GetIO().WantCaptureKeyboard == true)
		return;

	for (auto& shortcut : this->shortcuts)
	{
		shortcut.Run(this->openIframeFun, this->env);
	}
}

gui::Shortcuts::Shortcut::Shortcut(tinyxml2::XMLElement* el)
{
	this->iframeToOpen = tinyxml2::TryParseStrAttribute(el, "iframe_to_open");
	this->functionToExecute = tinyxml2::TryParseStrAttribute(el, "execute_fun");
	this->bOpenIframe = !this->iframeToOpen.empty();
	if (el->FirstChildElement("key") == nullptr)
		return;

	for (tinyxml2::XMLElement* child = el->FirstChildElement("key"); child != nullptr; child = child->NextSiblingElement())
	{
		std::string key = tinyxml2::TryParseStrAttribute(child, "name");
		if (key.empty() == false)
			this->keys.push_back(key);
	}
}

gui::Shortcuts::Shortcut::Shortcut(Shortcut&& other) noexcept :
	keys(std::move(other.keys)), bOpenIframe(other.bOpenIframe), 
	iframeToOpen(std::move(other.iframeToOpen)), functionToExecute(std::move(other.functionToExecute))
{
}

gui::Shortcuts::Shortcut& gui::Shortcuts::Shortcut::operator=(Shortcut&& other) noexcept
{
	if (this != &other)
	{
		this->keys = std::move(other.keys);
		this->bOpenIframe = other.bOpenIframe;
		this->iframeToOpen = std::move(other.iframeToOpen);
		this->functionToExecute = std::move(other.functionToExecute);
	}
	return (*this);
}

gui::Shortcuts::Shortcut::~Shortcut(void)
{
}

bool gui::Shortcuts::Shortcut::IsOk(void) const
{
	return (this->keys.empty() == false) && (!this->functionToExecute.empty() == true || this->bOpenIframe);
}

void gui::Shortcuts::Shortcut::Run(std::function<bool(std::string& iframeToOpen)>& openIframeFun, IEnvironment::Environments env)
{
	if (this->keys.empty() == true) 
		return;	

	bool toOpen = true;
	for (auto const& key : this->keys)
	{
		// check that ALL shortcuts are pressed simultaneously
		toOpen &= Engine::GetInstance().IsKeyPressed(key);
	}
	if (toOpen)
	{
		// finally execute 
		if (this->bOpenIframe)
		{
			if (openIframeFun)
			{
				bool resetKeys = openIframeFun(this->iframeToOpen); 
				// this boolean returns true only if the iframe has been opened
				// (if it was already opened, it returns false)
				if (resetKeys)
				{
					rattlesmake::peripherals::keyboard& keyboard = rattlesmake::peripherals::keyboard::get_instance();
					// reset keys to avoid interactions with other iframes
					for (auto const& key : this->keys)
					{
						keyboard.SetKeyStatus(keyboard.GetGlfwKeyIdFromString(key), 0);
					}
				}
			}
		}
		else
		{
			// execute py function
			//todo
		}
	}
}
