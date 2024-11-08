/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/
#pragma once

#include <string>
#include <vector>

#include <tinyxml2_utils.h>

namespace gui
{
	class UI;

	class Menubar
	{
	public:
		Menubar(void);
		explicit Menubar(tinyxml2::XMLElement* el);
		Menubar(const Menubar& other) = delete;
		Menubar& operator=(const Menubar& other) = delete;
		Menubar(Menubar&& other) noexcept;
		[[nodiscard]] Menubar& operator=(Menubar&& other) noexcept;
		~Menubar(void);

		void Render(UI* myCreator);

		// N.B.: IF YOU ADD SOME ATTRIBUTE TO THIS CLASS, UPDATE CONSTRCUTOR BY MOVEMENT AND OPERATOR= BY MOVEMENT
		bool isBlocked = false;
		bool isHidden = false;
		std::vector<Menubar> options;
		std::string text;
		std::string icon;
		std::string onclick;
		std::string onclickOpen;
		std::string condition;
		std::string shortcut;
		std::string type;
	};
};
