/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/
#pragma once


#include <functional>
#include <string>
#include <vector>

#include <tinyxml2_utils.h>
#include <environments/ienvironment.h>

namespace gui
{
	class Iframe;

	class Shortcuts
	{
	public:
		Shortcuts(tinyxml2::XMLElement* el, IEnvironment::Environments _env);
		Shortcuts(const Shortcuts& other) = delete;
		Shortcuts& operator=(const Shortcuts& other) = delete;
		~Shortcuts(void);

		void SetOpenIframeFun(std::function<bool(std::string& iframeToOpen)> _function);
		void Run(void);
	private:
		class Shortcut
		{
		public:
			explicit Shortcut(tinyxml2::XMLElement* el);
			Shortcut(const Shortcut& other) = delete;
			Shortcut& operator=(const Shortcut& other) = delete;
			Shortcut(Shortcut&& other) noexcept;
			[[nodiscard]] Shortcut& operator=(Shortcut&& other) noexcept;
			~Shortcut(void);

			[[nodiscard]] bool IsOk(void) const;
			void Run(std::function<bool(std::string& iframeToOpen)>& openIframeFun, IEnvironment::Environments env);

			// N.B.: IF YOU ADD SOME ATTRIBUTE TO THIS CLASS, UPDATE CONSTRCUTOR BY MOVEMENT AND OPERATOR= BY MOVEMENT
			std::vector<std::string> keys;
			bool bOpenIframe = false;
			std::string iframeToOpen;
			std::string functionToExecute;
		};

		std::vector<Shortcut> shortcuts;
		std::function<bool(std::string& iframeToOpen)> openIframeFun;
		IEnvironment::Environments env;
	};
};
